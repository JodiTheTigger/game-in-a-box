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

#ifndef GAME_HPP
#define GAME_HPP

#include <Common/No.hpp>
#include <Network/IStateManager.hpp>

#include <btBulletDynamicsCommon.h>

#include <memory>

namespace GameInABox { namespace State { namespace Implementation {

class PhysicsCube;
class PhysicsPlayers;

class Game : GameInABox::Network::IStateManager
{
public:
    Game();
    ~Game();

    // All game stuff happens here.
    // RAM: TODO: Need to pass max allowed time? How do we control the time
    // allowed?
    void Tick();

private:
    btDbvtBroadphase myPhysicsBroadPhase;
    btDefaultCollisionConfiguration myCollisionConfiguration;
    btCollisionDispatcher myPhysicsDispatcher;
    btSequentialImpulseConstraintSolver myPhyiscsSolver;
    btDiscreteDynamicsWorld myPhysicsWorld;

    std::unique_ptr<PhysicsCube> myPhysicsLevel;
    std::unique_ptr<PhysicsPlayers> myPhysicsPlayers;

    // IStateManager
    std::vector<uint8_t> PrivateStateInfo(const boost::optional<GameInABox::Network::ClientHandle>& client) const override;

    boost::optional<GameInABox::Network::ClientHandle> PrivateConnect(
            std::vector<uint8_t> connectData,
            std::string& failReason) override;

    void PrivateDisconnect(GameInABox::Network::ClientHandle playerToDisconnect) override;

    bool PrivateCanReceive(boost::optional<GameInABox::Network::ClientHandle> client, std::size_t bytes) override;
    bool PrivateCanSend(boost::optional<GameInABox::Network::ClientHandle> client, std::size_t bytes) override;

    GameInABox::Network::Delta PrivateDeltaCreate(
            GameInABox::Network::ClientHandle client,
            boost::optional<GameInABox::Network::Sequence> lastAcked) const override;

    boost::optional<GameInABox::Network::Sequence> PrivateDeltaParse(
            GameInABox::Network::ClientHandle client,
            const GameInABox::Network::Delta& payload) override;
};

}}} // namespace

#endif // GAME_HPP
