#include "Global.h"
#include "gmlib/mc/network/BinaryStream.h"
#include "gmlib/mc/world/actor/Player.h"
#include "ll/api/memory/Hook.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/legacy/ActorUniqueID.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/packet/AddPlayerPacket.h"
#include "mc/network/packet/RemoveActorPacket.h"
#include "mc/network/packet/UpdateAbilitiesPacket.h"
#include "mc/server/ServerPlayer.h"
#include "mc/world/actor/player/SerializedSkin.h"

std::unordered_set<uint64> FreeCamList;

namespace FreeCamera {

void EnableFreeCameraPacket(Player* pl) {
    ((gmlib::world::actor::GMPlayer*)pl)->setClientGamemode(GameType::Spectator);
}

void SendFakePlayerPacket(Player* pl) {
    // Client Player
    auto pkt1             = AddPlayerPacket(*pl);
    pkt1.mEntityId->rawID = pkt1.mEntityId->rawID + 114514;
    auto randomUuid       = mce::UUID::random();
    pkt1.mUuid            = randomUuid;
    pl->sendNetworkPacket(pkt1);
    // Update Skin
    auto                           skin = SerializedSkin(pl->getConnectionRequest());
    gmlib::network::GMBinaryStream bs;
    bs.writePacketHeader(MinecraftPacketIds::PlayerSkin);
    bs.writeUuid(randomUuid);
    bs.writeSkin(skin);
    bs.writeString("");
    bs.writeString("");
    bs.writeBool(true);
    bs.sendTo(
        *(gmlib::world::actor::GMPlayer*)pl,
        NetworkPeer::Reliability::ReliableOrdered,
        Compressibility::Compressible
    );
}

void DisableFreeCameraPacket(Player* pl) {
    ((gmlib::world::actor::GMPlayer*)pl)->setClientGamemode(pl->getPlayerGameType());
    auto uniqueId  = pl->getOrCreateUniqueID();
    uniqueId.rawID = uniqueId.rawID + 114514;
    // RemoveActorPacket(uniqueId).sendTo(*pl);
    auto pkt      = RemoveActorPacket();
    pkt.mEntityId = uniqueId;
    pkt.sendTo(*pl);
    UpdateAbilitiesPacket(pl->getOrCreateUniqueID(), pl->getAbilities()).sendTo(*pl);
}

/*
void SendActorLinkPacket(Player* pl) {
    auto links = pl->getLinks();
    for (auto& link : links) {
        GMLIB_BinaryStream bs;
        if (ll::service::getLevel()->getPlayer(link.A)) {
            bs.writeVarInt64(link.A.id + 114514);
        } else {
            bs.writeVarInt64(link.A.id);
        }
        if (ll::service::getLevel()->getPlayer(link.B)) {
            bs.writeVarInt64(link.B.id + 114514);
        } else {
            bs.writeVarInt64(link.B.id);
        }
        bs.writeUnsignedChar((uchar)link.type);
        bs.writeBool(link.mImmediate);
        bs.writeBool(link.mPassengerInitiated);
        GMLIB::Server::NetworkPacket<(int)MinecraftPacketIds::SetActorLink> pkt(bs.getAndReleaseData());
        pl->sendNetworkPacket(pkt);
    }
}
*/

void EnableFreeCamera(Player* pl) {
    FreeCamList.insert(pl->getNetworkIdentifier().mGuid.g);
    EnableFreeCameraPacket(pl);
    SendFakePlayerPacket(pl);
    // SendActorLinkPacket(pl);
}

void DisableFreeCamera(Player* pl) {
    auto pos   = pl->getFeetPos();
    auto dimid = pl->getDimensionId();
    // auto links = pl->getLinks();
    FreeCamList.erase(pl->getNetworkIdentifier().mGuid.g);
    DisableFreeCameraPacket(pl);
    pl->teleport(pos, dimid);
    // for (auto& link : links) {
    //     auto ride  = ll::service::getLevel()->fetchEntity(link.A);
    //     auto rider = ll::service::getLevel()->fetchEntity(link.B);
    //     if (ride && rider) {
    //        rider->startRiding(*ride);
    //    }
    //}
}

LL_TYPE_INSTANCE_HOOK(
    ServerPlayerMoveHandleEvent,
    ll::memory::HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::$handle,
    void,
    NetworkIdentifier const&     id,
    PlayerAuthInputPacket const& pkt
) {
    if (FreeCamList.contains(id.mGuid.g)) {
        return;
    } else {
        return origin(id, pkt);
    }
}

LL_TYPE_INSTANCE_HOOK(
    PlayerGamemodeChangeEvent,
    ll::memory::HookPriority::Normal,
    ServerPlayer,
    &ServerPlayer::$setPlayerGameType,
    void,
    ::GameType gamemode
) {
    origin(gamemode);
    if (FreeCamList.contains(getNetworkIdentifier().mGuid.g)) {
        FreeCamera::DisableFreeCamera(this);
    }
}

LL_TYPE_INSTANCE_HOOK(
    PlayerHurtEvent,
    ll::memory::HookPriority::Normal,
    Mob,
    &Mob::getDamageAfterResistanceEffect,
    float,
    class ActorDamageSource const& a1,
    float                          a2
) {
    auto res = origin(a1, a2);
    if (this->isType(ActorType::Player) && res != 0) {
        auto pl = (Player*)this;
        if ((pl->isSurvival() || pl->isAdventure()) && FreeCamList.contains(pl->getNetworkIdentifier().mGuid.g)) {
            FreeCamera::DisableFreeCamera(pl);
        }
    }
    return res;
}

LL_TYPE_INSTANCE_HOOK(
    PlayerDieEvent,
    ll::memory::HookPriority::Normal,
    Player,
    &Player::$die,
    void,
    class ActorDamageSource const& a1
) {
    if (FreeCamList.contains(getNetworkIdentifier().mGuid.g)) {
        FreeCamera::DisableFreeCamera(this);
    }
    return origin(a1);
}

LL_TYPE_INSTANCE_HOOK(
    PlayerLeftEvent,
    ll::memory::HookPriority::Normal,
    ServerPlayer,
    &ServerPlayer::disconnect,
    void
) {
    FreeCamList.erase(getNetworkIdentifier().mGuid.g);
    return origin();
}

struct Impl {
    ll::memory::HookRegistrar<
        ServerPlayerMoveHandleEvent,
        PlayerGamemodeChangeEvent,
        PlayerHurtEvent,
        PlayerDieEvent,
        PlayerLeftEvent>
        r;
};

std::unique_ptr<Impl> impl;

void freecameraHook(bool enable) {
    if (enable) {
        if (!impl) impl = std::make_unique<Impl>();
    } else {
        impl.reset();
    }
};

} // namespace FreeCamera