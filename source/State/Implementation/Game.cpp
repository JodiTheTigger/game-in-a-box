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

#include "Game.hpp"

#include "Types.hpp"
#include "TypesDelta.hpp"
#include "DeltaMapItem.hpp"
#include "Units.hpp"

#include <vector>

// offsetof()
#include <cstddef>


namespace GameInABox { namespace State { namespace Implementation {

Game::Game()
{
    // Just a scratch class for now.
    /*
        template<unsigned MaxPlayersPerTeam>
        struct StateGame
        {
            const unsigned maxPlayersPerTeam = MaxPlayersPerTeam;
            const unsigned maxPlayersPerGame = MaxPlayersPerTeam * 3;
            const unsigned maxMisslesPerGame = maxPlayersPerGame * 4;

            std::array<StatePlayerNetwork, maxPlayersPerGame>   playersNetwork;
            std::array<StatePlayer, maxPlayersPerGame>          players;
            std::array<StateMissle, maxMisslesPerGame>          missles;

            StateMode mode;

            // names are UTF8 encoded, maximum 16 code points, not null terminated.
            // padded with spaces.
            std::array<std::array<uint8_t, 16>, maxPlayersPerGame> playerNames;
        };
    */

    // offsetof (type,member)
    // std::is_standard_layout<A>::value

    // Right, how do I want to define my delta?

    /*
        struct StatePlayer
        {
            ServerId        id;
            Vec3            position;
            Vec3            orientation;
            std::uint32_t   health;
            std::uint32_t   energy;
            FlagsPlayer     flags;
            ServerTick      lastShot;
        };
    */

    auto mapPlayer = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(StatePlayer, position.x), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
        {MAKE_OFFSET(StatePlayer, position.y), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
        {MAKE_OFFSET(StatePlayer, position.z), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},

        {MAKE_OFFSET(StatePlayer, orientation.x), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
        {MAKE_OFFSET(StatePlayer, orientation.y), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
        {MAKE_OFFSET(StatePlayer, orientation.z), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},

        {MAKE_OFFSET(StatePlayer, health), MapUnsigned{10_bits}},
        {MAKE_OFFSET(StatePlayer, energy), MapUnsigned{10_bits}},

        {MAKE_OFFSET(StatePlayer, flags), MapUnsigned{32_bits}},
    };

    /*
        struct StateMissle
        {
            ServerId id;
            ServerId owner;

            // Where it was fired from, or where it exploded
            Vec3 source;

            // Initial velocity, up to game how to predict the path.
            Vec3 orientation;

            // Time it was fired/exploded
            ServerTick lastAction;

            // Exploded or not?
            FlagsMissle flags;
        };
    */

    auto mapMissle = std::vector<DeltaMapItem>
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

        {MAKE_OFFSET(StateMissle, flags), MapUnsigned{2_bits}},
    };

   /* auto mapping =
    {
        {std::offsetof(StateGame, playersNetwork), maxPlayersPerGame,
    };*/
}

}}} // namspace
