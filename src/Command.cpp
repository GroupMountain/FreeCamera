#include "Entry.h"
#include "Global.h"
#include "gmlib/mc/world/actor/Actor.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/service/Bedrock.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/actor/player/Player.h"

void RegisterCommand() {
    auto& cmd = ll::command::CommandRegistrar::getInstance()
                    .getOrCreateCommand("freecamera", "freecamera.command.desc"_tr(), CommandPermissionLevel::Any);
    ll::service::getCommandRegistry()->registerAlias("freecamera", "fc");
    cmd.overload().execute([&](CommandOrigin const& origin, CommandOutput& output) {
        auto entity = (gmlib::GMActor*)origin.getEntity();
        if (entity && entity->isPlayer()) {
            auto pl   = (Player*)entity;
            auto guid = pl->getNetworkIdentifier().mGuid.g;
            if (!FreeCamList.count(guid)) {
                FreeCamera::EnableFreeCamera(pl);
                return output.success("freecamera.command.enabled"_tr());
            } else {
                FreeCamera::DisableFreeCamera(pl);
                return output.success("freecamera.command.disabled"_tr());
            }
        }
        return output.error("freecamera.command.console"_tr());
    });
}