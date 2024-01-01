#include "Plugin.h"
#include <ll/api/command/DynamicCommand.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/command/SetupCommandEvent.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/plugin/NativePlugin.h>
#include <mc/entity/utilities/ActorType.h>
#include <mc/network/NetworkIdentifier.h>
#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/packet/SerializedAbilitiesData.h>
#include <mc/network/packet/SubChunkRequestPacket.h>
#include <mc/network/packet/UpdateAbilitiesPacket.h>
#include <mc/network/packet/UpdateAdventureSettingsPacket.h>
#include <mc/network/packet/AddPlayerPacket.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/LayeredAbilities.h>
#include <mc/network/packet/SubChunkPacket.h>
#include <utility>
#include <mc/world/level/Level.h>
#include <mc/network/packet/RemoveActorPacket.h>

void EnableFreeCameraPacket(Player* pl) {
    LayeredAbilities abilities = pl->getAbilities();
    abilities.getAbility(AbilitiesLayer(1), AbilitiesIndex::Flying).setBool(true);
    abilities.setAbility(AbilitiesIndex::Flying, true);
    abilities.setAbility(AbilitiesIndex::MayFly, true);
    abilities.setAbility(AbilitiesIndex::NoClip, true);
    UpdateAbilitiesPacket pkt1(pl->getOrCreateUniqueID(), abilities);
    auto                  pkt2 = UpdateAdventureSettingsPacket(AdventureSettings());
    pl->sendNetworkPacket(pkt2);
    pl->sendNetworkPacket(pkt1);
    auto pkt3 = AddPlayerPacket(*pl);
    pkt3.mEntityId.id = pkt3.mEntityId.id + 114514;
    pkt3.mUuid = mce::UUID::random();
    pl->sendNetworkPacket(pkt3);
}

void DisableFreeCameraPacket(Player* pl) {
    LayeredAbilities abilities = pl->getAbilities();
    UpdateAbilitiesPacket pkt1(pl->getOrCreateUniqueID(), abilities);
    auto                  pkt2 = UpdateAdventureSettingsPacket(AdventureSettings());
    pl->sendNetworkPacket(pkt2);
    pl->sendNetworkPacket(pkt1);
    auto auid = pl->getOrCreateUniqueID();
    auid.id = auid.id + 114514;
    auto pkt3 = RemoveActorPacket(auid);
    pl->sendNetworkPacket(pkt3);
}

std::unordered_set<uint64> FreeCamList;

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    auto& logger = mSelf.getLogger();
    logger.info("Loading FreeCamera...");

    auto& eventBus             = ll::event::EventBus::getInstance();
    mSetupCommandEventListener = eventBus.emplaceListener<
        ll::event::command::SetupCommandEvent>([&logger](ll::event::command::SetupCommandEvent& event) {
        auto command = DynamicCommand::createCommand(
            event.registry(),
            "freecamera",
            "Enable or disable free camera mode",
            CommandPermissionLevel::Any
        );
        command->setAlias("fc");
        command->addOverload();
        command->setCallback(
            [&logger](DynamicCommand const&, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>&) {
                auto* entity = origin.getEntity();
                if (entity == nullptr || !entity->isType(ActorType::Player)) {
                    output.error("Only players can execute this command!");
                    return;
                }
                auto pl = (Player*)entity;
                auto guid = pl->getNetworkIdentifier().mGuid.g;
                if (!FreeCamList.count(guid)) {
                    FreeCamList.insert(guid);
                    EnableFreeCameraPacket(pl);
                    output.success("Free camera enabled");
                } else {
                    FreeCamList.erase(guid);
                    DisableFreeCameraPacket(pl);
                    // todo 
                    // 客户端返回原位置
                    pl->resendAllChunks();
                    output.success("Free camera disabled");
                }
            }
        );
        DynamicCommand::setup(event.registry(), std::move(command));
    });


    // Code for loading the plugin goes here.
}

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();
    //logger.info("Enabling FreeCamera...");

    // Code for enabling the plugin goes here.
    return true;
}

bool Plugin::disable() {
    auto& logger = mSelf.getLogger();
    logger.info("Disabling FreeCamera...");

    // Code for disabling the plugin goes here.
    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.removeListener(mSetupCommandEventListener);
    FreeCamList.clear();
    ll::service::getLevel().get().forEachPlayer([](Player& pl)->bool {
        DisableFreeCameraPacket(&pl);
        return true;
    });
    return true;
}

} // namespace plugin

LL_AUTO_TYPED_INSTANCE_HOOK(
    SubChunkRequestPacketHook,
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

// todo
// Free Camera状态下拦截服务端实际移动