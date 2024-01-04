#include "include_all.h"
#include "Global.h"

void RegisterCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(
        registry,
        "freecamera",
        "Enable or disable free camera mode",
        CommandPermissionLevel::Any
    );
    command->setAlias("fc");
    command->addOverload();
    command->setCallback([](DynamicCommand const&,
                            CommandOrigin const& origin,
                            CommandOutput&       output,
                            std::unordered_map<std::string, DynamicCommand::Result>&) {
        auto* entity = origin.getEntity();
        if (entity == nullptr || !entity->isType(ActorType::Player)) {
            output.error("Only players can execute this command!");
            return;
        }
        auto pl   = (Player*)entity;
        auto guid = pl->getNetworkIdentifier().mGuid.g;
        if (!FreeCamList.count(guid)) {
            FreeCamera::EnableFreeCamera(pl);
            output.success("Free camera enabled");
        } else {
            FreeCamera::DisableFreeCamera(pl);
            output.success("Free camera disabled");
        }
    });
    DynamicCommand::setup(registry, std::move(command));
}