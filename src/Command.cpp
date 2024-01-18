#include "Global.h"
#include "include_all.h"
#include <GMLIB/Server/ActorAPI.h>

void RegisterCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(
        registry,
        "freecamera",
        "Enable or disable free camera mode",
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
                return output.success("Free camera enabled");
            } else {
                FreeCamera::DisableFreeCamera(pl);
                return output.success("Free camera disabled");
            }
        }
        return output.error("Only players can execute this command!");
    });
    DynamicCommand::setup(registry, std::move(command));
}