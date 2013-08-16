/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
    This file is part of Game-in-a-box
    
    Game-in-a-box is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.
    
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "DeltaStateGameSnapshot.hpp"

#include <Common/BitStream.hpp>
#include <Common/BitStreamReadOnly.hpp>

using GameInABox::Common::BitStream;
using GameInABox::Common::BitStreamReadOnly;

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Make this a helper class.
constexpr unsigned BitsNeeded(unsigned value, unsigned bits)
{
    return ((1U << bits) > value) ? bits : BitsNeeded(value, bits + 1);
}

constexpr Bits BitsNeeded(unsigned value)
{
    return Bits{static_cast<signed>(BitsNeeded(value, 1))};
}

template<typename T>
class BitsForEnum
{
public:
    static constexpr Bits Result()
    {
        return BitsNeeded(static_cast<unsigned>(T::MaxValue));
    }
};

DeltaStateGameSnapshot::DeltaStateGameSnapshot(Research settings)
    : myCoderPlayer({
        std::vector<DeltaMapItem>
        {
            {MAKE_OFFSET(StatePlayer, position.x), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
            {MAKE_OFFSET(StatePlayer, position.y), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
            {MAKE_OFFSET(StatePlayer, position.z), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},

            {MAKE_OFFSET(StatePlayer, lookAndDo.orientation.x), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
            {MAKE_OFFSET(StatePlayer, lookAndDo.orientation.y), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
            {MAKE_OFFSET(StatePlayer, lookAndDo.orientation.z), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},

            {MAKE_OFFSET(StatePlayer, health), MapUnsigned{10_bits}},
            {MAKE_OFFSET(StatePlayer, energy), MapUnsigned{10_bits}},

            {MAKE_OFFSET(StatePlayer, lookAndDo.flags), MapUnsigned{BitsForEnum<FlagsPlayer>::Result()}},
        },
        settings})
    , myCoderMissle({
        std::vector<DeltaMapItem>
        {
            // RAM: TODO: Find max bits.
            {MAKE_OFFSET(StateMissle, owner), MapUnsigned{32_bits}},

            {MAKE_OFFSET(StateMissle, source.x), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
            {MAKE_OFFSET(StateMissle, source.y), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
            {MAKE_OFFSET(StateMissle, source.z), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},

            {MAKE_OFFSET(StateMissle, orientation.x), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
            {MAKE_OFFSET(StateMissle, orientation.y), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
            {MAKE_OFFSET(StateMissle, orientation.z), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},

            // RAM: TODO: Find max bits, wrapping counter?
            {MAKE_OFFSET(StateMissle, lastAction), MapUnsigned{32_bits}},

            {MAKE_OFFSET(StateMissle, flags), MapUnsigned{BitsForEnum<FlagsMissle>::Result()}},
        },
        settings})
{
}

std::vector<uint8_t> DeltaStateGameSnapshot::operator()(
        const StateGameSnapshot& base,
        const StateGameSnapshot& target)
{
    // NOTE: Ideas for further improvement:
    // Instead of sending all player state and missle state, use the q3 method
    // and only send "active" units. Because if only 1 missle changes, then ontop
    // of sending whatever delta the one missle requires, we also send #missles bits
    // stating that nothing else has changed. That's 128*3*4 bits = 1536 bits = 192 bytes.
    // for now, assume compressions magically deals with all this :-/

    // Players always exist.
    // Guessing on the size of the delta buffer.
    // RAM: TODO: Find an optimum size.
    BitStream changedPlayers(BitsNeeded(base.players.size()).value / 8);
    BitStream deltas(1 << 16);

    for (unsigned i = 0; i < base.players.size(); ++i)
    {
        auto& playerBase = base.players[i];
        auto& playerTarget = target.players[i];

        if (
                (playerBase.position != playerTarget.position) ||
                (playerBase.lookAndDo.orientation != playerTarget.lookAndDo.orientation) ||
                (playerBase.lookAndDo.flags != playerTarget.lookAndDo.flags) ||
                (playerBase.jetDirection != playerTarget.jetDirection) ||
                (playerBase.health != playerTarget.health) ||
                (playerBase.energy != playerTarget.energy)
            )
        {
            changedPlayers.Push(true);
            myCoderPlayer.DeltaEncode(playerBase, playerTarget, deltas);
        }
        else
        {
            changedPlayers.Push(false);
        }
    }

    // missles always exist.
    BitStream changedMissles(BitsNeeded(base.missles.size()).value / 8);

    for (unsigned i = 0; i < base.missles.size(); ++i)
    {
        auto& missleBase = base.missles[i];
        auto& missleTarget = target.missles[i];

        if (
               (missleBase.owner != missleTarget.owner) ||
               (missleBase.source != missleTarget.source) ||
               (missleBase.orientation != missleTarget.orientation) ||
               (missleBase.lastAction != missleTarget.lastAction) ||
               (missleBase.flags != missleTarget.flags)
           )
        {
            changedMissles.Push(true);
            myCoderMissle.DeltaEncode(missleBase, missleTarget, deltas);
        }
        else
        {
            changedMissles.Push(false);
        }
    }

    // Noticing a pattern here.
    // RAM: TODO: Deduplicate code please.
    BitStream changedString(BitsNeeded(base.playerNames.size()).value / 8);

    for (unsigned i = 0; i < base.playerNames.size(); ++i)
    {
        auto& nameBase = base.playerNames[i];
        auto& nameTarget = target.playerNames[i];

        if (nameBase != nameTarget)
        {
            changedString.Push(true);

            // just xor the strings together.
            for (unsigned j = 0; j < nameBase.size(); ++j)
            {
                deltas.Push(static_cast<uint8_t>(nameBase[j] ^ nameTarget[j]), 8);
            }
        }
        else
        {
            changedString.Push(false);
        }
    }

    // write it all out.
    auto result = changedPlayers.TakeBuffer();
    auto missles = changedMissles.TakeBuffer();
    auto names = changedString.TakeBuffer();
    auto data = deltas.TakeBuffer();
    result.insert(end(result), begin(missles), end(missles));
    result.insert(end(result), begin(names), end(names));
    result.insert(end(result), begin(data), end(data));

    return result;
}


StateGameSnapshot DeltaStateGameSnapshot::operator()(
         const StateGameSnapshot& base,
         const std::vector<uint8_t>& payload)
{
    StateGameSnapshot target;

    BitStream data(payload);
    BitStream changedPlayers(BitsNeeded(base.players.size()).value / 8);
    BitStream changedMissles(BitsNeeded(base.missles.size()).value / 8);
    BitStream changedString(BitsNeeded(base.playerNames.size()).value / 8);

    // Bah, this is kack handed. Need to do it a better way.
    for (auto i = 0; i < (BitsNeeded(base.players.size()).value / 8); ++i)
    {
        changedPlayers.Push(data.PullU8(8), 8);
    }

    for (auto i = 0; i < (BitsNeeded(base.missles.size()).value / 8); ++i)
    {
        changedMissles.Push(data.PullU8(8), 8);
    }

    for (auto i = 0; i < (BitsNeeded(base.playerNames.size()).value / 8); ++i)
    {
        changedString.Push(data.PullU8(8), 8);
    }

    // right players.
    for (unsigned i = 0; i < base.players.size(); ++i)
    {
        auto& playerBase = base.players[i];
        auto& playerTarget = target.players[i];

        if (changedPlayers.Pull1Bit())
        {
            myCoderPlayer.DeltaDecode(playerBase, playerTarget, data);
        }
        else
        {
            playerTarget = playerBase;
        }
    }

    // missles
    for (unsigned i = 0; i < base.players.size(); ++i)
    {
        auto& missleBase = base.missles[i];
        auto& missleTarget = target.missles[i];

        if (changedMissles.Pull1Bit())
        {
            myCoderMissle.DeltaDecode(missleBase, missleTarget, data);
        }
        else
        {
            missleTarget = missleBase;
        }
    }

    // names
    for (unsigned i = 0; i < base.playerNames.size(); ++i)
    {
        auto& nameBase = base.playerNames[i];
        auto& nameTarget = target.playerNames[i];

        if (changedMissles.Pull1Bit())
        {
            // just xor the strings together.
            for (unsigned j = 0; j < nameBase.size(); ++j)
            {
                nameTarget[j] = nameBase[j] ^ data.PullU8(8);
            }
        }
        else
        {
            nameTarget = nameBase;
        }
    }

    return target;
}


}}} // namespace
