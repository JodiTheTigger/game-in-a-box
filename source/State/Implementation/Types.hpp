#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <array>
#include <vector>

namespace GameInABox { namespace State { namespace Implementation

// //////////////
// Data Types
// //////////////
union Vec3
{
    std::array<float, 3> values;
    struct
    {
        float x;
        float y;
        float z;
    };
};

union Vec4
{
    std::array<float, 4> values;
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
};

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

    MaxValue = Jet
};

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

struct StatePlayerNetwork
{
    std::uint32_t   lastAck;
    std::uint32_t   latencyInMs; // argh, needs a millisecond datatype please.
    ServerId        playerId;
};

// //////////////
// Missle
// //////////////
enum class FlagsMissle : uint32_t
{
    Inactive = 0,
    Armed = 1,
    Exploded = 2,

    MaxValue = Exploded
};

struct StateMissle
{
    ServerId id;
    ServerId owner;

    // Where it was fired from, or where it exploded
    Vec3 source;

    // Initial velocity, up to game how to predict the path.
    Vec3 velocity;

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

    MaxValue = EndCredits
};

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


}}} // namespace


#endif // TYPES_HPP
