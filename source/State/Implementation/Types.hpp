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

#ifndef TYPES_HPP
#define TYPES_HPP

#include "Vector3.hpp"

#include <cstdint>
#include <array>
#include <vector>

namespace GameInABox { namespace State { namespace Implementation {

// //////////////
// Data Types
// //////////////
using ServerTick = uint32_t;
using ServerId = uint32_t;

// //////////////
// Player
// //////////////
enum class FlagsPlayer : uint32_t
{
    TeamR = 0,
    TeamG = 1,
    TeamB = 2,
    Jet = 4,
    Shoot = 8,
    Forward = 16,
    Backward = 32,
    Left = 64,
    Right = 128,

    Default = TeamR,
    MaxValue = Right
};

struct StatePlayerClient
{
    Vector3 orientation;
    FlagsPlayer flags;
};

// multiples of 16 bytes please.
struct StatePlayer
{
    ServerId            id;
    Vector3              position;

    StatePlayerClient   lookAndDo;

    Vector3              jetDirection;
    std::uint32_t       health;

    std::uint32_t       energy;
    ServerTick          lastShot;

    // network stuff.
    std::uint32_t   lastAck;
    std::uint32_t   latencyInMs; // argh, needs a millisecond datatype please.
};

// //////////////
// Missle
// //////////////
enum class FlagsMissle : uint32_t
{
    Inactive = 0,
    Armed = 1,
    Exploded = 2,

    Default = Inactive,
    MaxValue = Exploded
};

struct StateMissle
{
    ServerId id;
    ServerId owner;

    // Where it was fired from, or where it exploded
    Vector3 source;

    // Initial velocity, up to game how to predict the path.
    Vector3 orientation;

    // Time it was fired/exploded
    ServerTick lastAction;

    // Exploded or not?
    FlagsMissle flags;
};

// //////////////
// Game State
// //////////////
enum class StateMode : std::uint32_t
{
    Loading,
    Playing,
    EndCredits,

    Default = Loading,
    MaxValue = EndCredits
};

template<unsigned MaxPlayersPerTeam>
struct StateGameSnapshotGeneral
{
    static const unsigned maxPlayersPerTeam = MaxPlayersPerTeam;
    static const unsigned maxPlayersPerGame = MaxPlayersPerTeam * 3;
    static const unsigned maxMisslesPerGame = maxPlayersPerGame * 4;

    std::array<StatePlayer, maxPlayersPerGame>          players;
    std::array<StateMissle, maxMisslesPerGame>          missles;

    StateMode mode;

    // names are UTF8 encoded, maximum 16 bytes, not null terminated.
    // padded with spaces.
    std::array<std::array<uint8_t, 16>, maxPlayersPerGame> playerNames;
};

typedef StateGameSnapshotGeneral<128> StateGameSnapshot;

}}} // namespace


#endif // TYPES_HPP
