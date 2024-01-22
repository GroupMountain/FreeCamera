#include "Global.h"
#include "include_all.h"
#include <GMLIB/Server/ActorAPI.h>

using namespace GMLIB::Files::JsonLanguage;

void RegisterCommand() {
    auto registry = ll::service::getCommandRegistry();
    auto command = DynamicCommand::createCommand(
        registry,
        "freecamera",
        tr("freecamera.command.desc"),
        CommandPermissionLevel::Any
    );
    command->setAlias("fc");
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    command,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
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
    });
    DynamicCommand::setup(registry, std::move(command));
}