#include "Global.h"
#include "include_all.h"

ll::Logger logger(PLUGIN_NAME);

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    initLanguage();
}

bool Plugin::enable() {
    auto registry = ll::service::getCommandRegistry();
    if (registry) {
        RegisterCommand(registry);
    } else {
        logger.error("Fail to register freecamera command!");
    }
    logger.info("FreeCamera Loaded!");
    logger.info("Author: Tsubasa6848");
    logger.info("Repository: https://github.com/GroupMountain/FreeCamera");
    return true;
}

bool Plugin::disable() {
    auto& logger = mSelf.getLogger();
    logger.info("Disabling FreeCamera...");
    // Code for disabling the plugin goes here.
    FreeCamList.clear();
    ll::service::getLevel().get().forEachPlayer([](Player& pl) -> bool {
        FreeCamera::DisableFreeCamera(&pl);
        return true;
    });
    auto registry = ll::service::getCommandRegistry();
    if (registry) {
        registry->unregisterCommand("freecamera");
    } else {
        logger.error("Fail to unregister freecamera command!");
    }
    logger.info("FreeCamera Disabled!");
    return true;
}

} // namespace plugin