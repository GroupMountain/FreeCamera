#include "Global.h"
#include "include_all.h"

ll::Logger logger(PLUGIN_NAME);

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    initLanguage();
}

bool Plugin::enable() {
    auto requireLibVersion = SemVersion(0, 5, 4, "", "");
    if (!GMLIB::Version::checkLibVersionMatch(requireLibVersion)) {
        logger.error("GMLIB Version is outdated! Please update your GMLIB!");
        logger.error(
            "Current GMLIB Version {}, Required Lowest GMLIB Version {}",
            GMLIB::Version::getLibVersionString(),
            requireLibVersion.asString()
        );
    }
    RegisterCommand();
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