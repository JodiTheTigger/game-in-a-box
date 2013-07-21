/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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

#ifndef GAMEINABOX_H
#define GAMEINABOX_H

#include <cstdint>

#include "IStateManager.hpp"

namespace GameInABox { namespace Unused {

class GameSimple : public GameInABox::Network::IStateManager
{
public:
    GameSimple() {};
    virtual ~GameSimple() {};
    
private:
    std::array<uint64_t, 256> myFrequencies;

    std::array<uint64_t, 256> PrivateGetHuffmanFrequencies() const override;    
    std::vector<uint8_t> PrivateStateInfo(const boost::optional<GameInABox::Network::ClientHandle>& client) const override;

    boost::optional<GameInABox::Network::ClientHandle> PrivateConnect(
            std::vector<uint8_t> connectData,
            std::string& failReason) override;

    bool PrivateCanSend(boost::optional<GameInABox::Network::ClientHandle> client, std::size_t bytes) override;
    bool PrivateCanReceive(boost::optional<GameInABox::Network::ClientHandle> client, std::size_t bytes) override;

    void PrivateDisconnect(GameInABox::Network::ClientHandle playerToDisconnect) override;
    bool PrivateIsConnected(GameInABox::Network::ClientHandle client) const override;

    virtual GameInABox::Network::Delta PrivateDeltaCreate(
            GameInABox::Network::ClientHandle client,
            boost::optional<GameInABox::Network::Sequence> lastAcked) const override;

    virtual GameInABox::Network::Sequence PrivateDeltaParse(
            GameInABox::Network::ClientHandle client,
            const GameInABox::Network::Delta& payload) override;
};

}} // namespace

#endif // GAMEINABOX_H
