#include "Global.h"

void RegisterCommand() {
    auto& cmd = ll::command::CommandRegistrar::getInstance()
                    .getOrCreateCommand("freecamera", tr("freecamera.command.desc"), CommandPermissionLevel::Any);
    ll::service::getCommandRegistry()->registerAlias("freecamera", "fc");
    cmd.overload().execute<[&](CommandOrigin const& origin, CommandOutput& output) {
        auto entity = (GMLIB_Actor*)origin.getEntity();
        if (entity && entity->isPlayer()) {
            auto pl   = (Player*)entity;
            auto guid = pl->getNetworkIdentifier().mGuid.g;
            if (!FreeCamList.count(guid)) {
                FreeCamera::EnableFreeCamera(pl);
                return output.success(tr("freecamera.command.enabled"));
            } else {
                FreeCamera::DisableFreeCamera(pl);
                return output.success(tr("freecamera.command.disabled"));
            }
        }
        return output.error(tr("freecamera.command.console"));
    }>();
}