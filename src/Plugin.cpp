#include "Global.h"
#include "ll/api/service/Bedrock.h"

ll::Logger logger(PLUGIN_NAME);

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    initLanguage();
}

bool Plugin::enable() {
    auto requireLibVersion = SemVersion(0, 7, 2, "", "");
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
    logger.info("Author: GroupMountain");
    logger.info("Repository: https://github.com/GroupMountain/FreeCamera");
    return true;
}

bool Plugin::disable() {
    // Code for disabling the plugin goes here.
    FreeCamList.clear();
    ll::service::getLevel().get().forEachPlayer([](Player& pl) -> bool {
        FreeCamera::DisableFreeCamera(&pl);
        return true;
    });
    return true;
}

} // namespace plugin