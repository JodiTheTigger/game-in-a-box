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

#include "GameSimple.hpp"
#include "PlayerFunctions.hpp"
#include "Flags.hpp"

namespace GameInABox { namespace State { namespace Implementation { namespace GameSimple {

Entity ThinkPlayer(Entity target, const EntityConstants&);
Entity ThinkMissle(Entity target, const EntityConstants&);

// /////////////////
// Tests
// /////////////////

bool CanThink(const Entity& target)
{
    // Items that update their state via the constants.
    return
            (target.type == EntityType::Player) ||
            (target.type == EntityType::Missle);
}

bool CanCreate(const Entity& target)
{
    return (target.type == EntityType::Player);
}

bool CanCollide(const Entity& target)
{
    return
            (target.type == EntityType::Player) ||
            (target.type == EntityType::Missle) ||
            (target.type == EntityType::Map);
}

bool CanMove(const Entity& target)
{
    return
            (target.type == EntityType::Player) ||
            (target.type == EntityType::Missle);
}

bool Collides(const Entity& first, const Entity& second)
{
    // RAM: TODO: Get an Assert class and start using it.
    // Assert(CanCollide(first));
    // Assert(CanCollide(second));

    if ((first.type == EntityType::Map) && (second.type == EntityType::Map))
    {
        // maps don't collide with maps.
        return false;
    }

    // world - player
    // world - missle
    // player - player
    // player - missle
    // missle - missle
    const Entity* a = &first;
    const Entity* b = &second;

    if (first.type != EntityType::Map)
    {
        if ((second.type == EntityType::Map) || (second.type == EntityType::Player))
        {
            a = &second;
            b = &first;
        }
    }

    // b will never be a EntityMap.
    auto position =
            b->type == EntityType::Player ?
                b->player.position :
                b->missle.position;

    auto minLength =
            b->type == EntityType::Player ?
                b->player.collisionRadius :
                b->missle.collisionRadius;

    switch (a->type)
    {
        case EntityType::Map:
        {
            auto distance = Length{DotF(
                        a->map.normal.value,
                        position.value - a->map.point.value)};

            if (distance < minLength)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        case EntityType::Player:
        case EntityType::Missle:
        {
            using namespace GameInABox::State::Implementation::Units;

            auto area = PlaneArea(a->player.position, position);

            if (area < (minLength * minLength))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        default:
        {
            // RAM: TODO: Error
            return false;
        }
    }
}

// /////////////////
// Actions
// /////////////////

Entity Think(Entity target, const EntityConstants& constants)
{
    if (target.type == EntityType::Player)
    {
        return ThinkPlayer(target, constants);
    }
    else
    {
        return ThinkMissle(target, constants);
    }
}

Entity ThinkPlayer(Entity target, const EntityConstants& constants)
{
    if (!FlagIsSet(target.player.allowedAction, FlagsPlayerAction::Dead))
    {
        // Refill timebased fuel/ammo counts.
        // RAM: TODO: unit maths!
        target.player.energyShoot.value += constants.regenerationPerTickAmmo.value;
        target.player.fuel += constants.regenerationPerTickJetEnergy;
        target.player.health.value += constants.regenerationPerTickHealth.value;

        // Can I Jet?
        if (FlagIsSet(target.player.input.action, FlagsPlayerAction::Jet))
        {
            if (target.player.fuel > constants.jetEnergyUsedPerTick)
            {
                target.player.fuel -= constants.jetEnergyUsedPerTick;

                FlagSet(target.player.allowedAction, FlagsPlayerAction::Jet);
            }
        }

        // Jump, Move, Jet
        target.player.acceleration = PlayerMovement(target.player, constants);

        // Shoot
        if (FlagIsSet(target.player.input.action, FlagsPlayerAction::Fire))
        {
            if (target.player.energyShoot.value > constants.ammoPerShot.value)
            {
                target.player.energyShoot.value -= constants.ammoPerShot.value;

                FlagSet(target.player.allowedAction, FlagsPlayerAction::Fire);
            }
        }

        // Die
        if (target.player.health.value < constants.deadHealth.value)
        {
            FlagSet(target.player.allowedAction, FlagsPlayerAction::Dead);
        }
    }

    return target;
}

Entity ThinkMissle(Entity target, const EntityConstants& constants)
{
    // Be simple, just apply thrust, ignore gravity.
    switch (target.missle.state)
    {
        case EntityStateMissle::Flying:
        {
            // don't do anything
            return target;
        }

        case EntityStateMissle::Exploding:
        {
            target.missle.collisionRadius += constants.explodeSizeDeltaPerTick;

            if (target.missle.collisionRadius >= constants.explodeMaxSize)
            {
                target.missle.state = EntityStateMissle::Scoring;
            }

            return target;
        }

        case EntityStateMissle::Scoring:
        {
            // done scoring, we're nothing now.
            return
            {
                EntityType::None,
                EntityNone{}
            };
        }
    }

    // RAM: TODO: Error
    return target;
}

std::pair<Entity, Entity> Create(Entity target)
{
    // RAM: TODO:
    return
    {
        {
            EntityType::None,
            EntityNone{}
        },
        target
    };
}

std::pair<Entity, Entity> Resolve(Entity first, Entity second)
{
    // RAM: TODO:
    return {first, second};
}

Entity Move(Entity target)
{
    // RAM: TODO:
    return target;
}

}}}} // namespace
