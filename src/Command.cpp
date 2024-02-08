#include "Global.h"
#include <GMLIB/Server/ActorAPI.h>

using namespace GMLIB::Files::JsonLanguage;
using namespace ll::command;


void RegisterCommand() {
    auto& cmd = CommandRegistrar::getInstance().getOrCreateCommand(
        "fc", tr("freecamera.command.desc"),
        CommandPermissionLevel::Any,
        CommandFlagValue::None
    );
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