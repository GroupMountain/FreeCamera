#include "Global.h"

std::unordered_set<uint64> FreeCamList;

namespace FreeCamera {

void EnableFreeCameraPacket(Player* pl) { ((GMLIB_Player*)pl)->setClientGamemode(GameType::Spectator); }

void SendFakePlayerPacket(Player* pl) {
    // Client Player
    auto pkt1         = AddPlayerPacket(*pl);
    pkt1.mEntityId.id = pkt1.mEntityId.id + 114514;
    auto randomUuid   = mce::UUID::random();
    pkt1.mUuid        = randomUuid;
    pl->sendNetworkPacket(pkt1);
    // Update Skin
    auto               skin = pl->getSkin();
    GMLIB_BinaryStream bs;
    bs.writeUuid(randomUuid);
    bs.writeSkin(skin);
    bs.writeString("");
    bs.writeString("");
    bs.writeBool(true);
    auto pkt2 = MinecraftPackets::createPacket(MinecraftPacketIds::PlayerSkin);
    pkt2->read(bs);
    pl->sendNetworkPacket(*pkt2);
}

void DisableFreeCameraPacket(Player* pl) {
    ((GMLIB_Player*)pl)->setClientGamemode(pl->getPlayerGameType());
    auto uniqueId = pl->getOrCreateUniqueID();
    uniqueId.id   = uniqueId.id + 114514;
    auto pkt2     = RemoveActorPacket(uniqueId);
    pl->sendNetworkPacket(pkt2);
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

} // namespace FreeCamera


LL_AUTO_TYPE_INSTANCE_HOOK(
    SubChunkRequestEvent,
    ll::memory::HookPriority::Normal,
    ServerNetworkHandler,
    "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSubChunkRequestPacket@@@Z",
    void,
    NetworkIdentifier const&     id,
    SubChunkRequestPacket const& pkt
) {
    if (FreeCamList.count(id.mGuid.g)) {
        return;
    } else {
        return origin(id, pkt);
    }
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    ServerPlayerMoveHandleEvent,
    ll::memory::HookPriority::Normal,
    ServerNetworkHandler,
    "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVPlayerAuthInputPacket@@@Z",
    void,
    NetworkIdentifier const&     id,
    PlayerAuthInputPacket const& pkt
) {
    if (FreeCamList.count(id.mGuid.g)) {
        return;
    } else {
        return origin(id, pkt);
    }
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerGamemodeChangeEvent,
    ll::memory::HookPriority::Normal,
    Player,
    "?setPlayerGameType@Player@@UEAAXW4GameType@@@Z",
    void,
    ::GameType gamemode
) {
    origin(gamemode);
    if (FreeCamList.count(getNetworkIdentifier().mGuid.g)) {
        FreeCamera::DisableFreeCamera(this);
    }
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerHurtEvent,
    ll::memory::HookPriority::Normal,
    Mob,
    "?getDamageAfterResistanceEffect@Mob@@QEBAMAEBVActorDamageSource@@M@Z",
    float,
    class ActorDamageSource const& a1,
    float                          a2
) {
    auto res = origin(a1, a2);
    if (this->isType(ActorType::Player) && res != 0) {
        auto pl = (Player*)this;
        if ((pl->isSurvival() || pl->isAdventure()) && FreeCamList.count(pl->getNetworkIdentifier().mGuid.g)) {
            FreeCamera::DisableFreeCamera(pl);
        }
    }
    return res;
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerDieEvent,
    ll::memory::HookPriority::Normal,
    Player,
    "?die@Player@@UEAAXAEBVActorDamageSource@@@Z",
    void,
    class ActorDamageSource const& a1
) {
    if (FreeCamList.count(getNetworkIdentifier().mGuid.g)) {
        FreeCamera::DisableFreeCamera(this);
    }
    return origin(a1);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerLeftEvent,
    ll::memory::HookPriority::Normal,
    ServerPlayer,
    "?disconnect@ServerPlayer@@QEAAXXZ",
    void
) {
    FreeCamList.erase(getNetworkIdentifier().mGuid.g);
    return origin();
}