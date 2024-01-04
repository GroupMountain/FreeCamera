#include "include_all.h"
#include "Global.h"

std::unordered_set<uint64> FreeCamList;

namespace FreeCamera {

void EnableFreeCameraPacket(Player* pl) {
    auto pkt = UpdatePlayerGameTypePacket(GameType::Spectator, pl->getOrCreateUniqueID());
    pl->sendNetworkPacket(pkt);
}

void SendFakePlayerPacket(Player* pl) {
    // Client Player
    auto pkt1         = AddPlayerPacket(*pl);
    pkt1.mEntityId.id = pkt1.mEntityId.id + 114514;
    auto randomUuid   = mce::UUID::random();
    pkt1.mUuid        = randomUuid;
    pl->sendNetworkPacket(pkt1);
    // Update Skin
    auto         skin = pl->getSkin();
    BinaryStream bs;
    bs.writeUnsignedInt64(randomUuid.a, 0i64, 0i64);
    bs.writeUnsignedInt64(randomUuid.b, 0i64, 0i64);
    skin.write(bs);
    bs.writeString("", 0i64, 0i64);
    bs.writeString("", 0i64, 0i64);
    bs.writeBool(true, 0i64, 0i64);
    auto pkt2 = MinecraftPackets::createPacket(MinecraftPacketIds::PlayerSkin);
    pkt2->read(bs);
    pl->sendNetworkPacket(*pkt2);
}

void DisableFreeCameraPacket(Player* pl) {
    auto pkt1 = UpdatePlayerGameTypePacket(pl->getPlayerGameType(), pl->getOrCreateUniqueID());
    pl->sendNetworkPacket(pkt1);
    auto auid = pl->getOrCreateUniqueID();
    auid.id   = auid.id + 114514;
    auto pkt2 = RemoveActorPacket(auid);
    pl->sendNetworkPacket(pkt2);
}

void EnableFreeCamera(Player* pl) {
    FreeCamList.insert(pl->getNetworkIdentifier().mGuid.g);
    EnableFreeCameraPacket(pl);
    SendFakePlayerPacket(pl);
}

void DisableFreeCamera(Player* pl) {
    auto pos   = pl->getFeetPos();
    auto dimid = pl->getDimensionId();
    FreeCamList.erase(pl->getNetworkIdentifier().mGuid.g);
    DisableFreeCameraPacket(pl);
    pl->teleport(pos, dimid);
}

} // namespace FreeCamera


LL_AUTO_TYPED_INSTANCE_HOOK(
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

LL_AUTO_TYPED_INSTANCE_HOOK(
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

LL_AUTO_TYPED_INSTANCE_HOOK(
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

LL_AUTO_TYPED_INSTANCE_HOOK(
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

LL_AUTO_TYPED_INSTANCE_HOOK(
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

LL_AUTO_TYPED_INSTANCE_HOOK(
    PlayerLeftEvent,
    ll::memory::HookPriority::Normal,
    ServerPlayer,
    "?disconnect@ServerPlayer@@QEAAXXZ",
    void
) {
    FreeCamList.erase(getNetworkIdentifier().mGuid.g);
    return origin();
}