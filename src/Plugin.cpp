#include "Global.h"

ll::Logger logger("FreeCamera");

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    auto& eventBus             = ll::event::EventBus::getInstance();
    mSetupCommandEventListener = eventBus.emplaceListener<ll::event::command::SetupCommandEvent>([](ll::event::command::SetupCommandEvent& event) {
        RegisterCommand(event.registry());
    });  
}

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();
    logger.info("FreeCamera Loaded!");
    logger.info("Author: Tsubasa6848");
    logger.info("Repository: https://github.com/GroupMountain/FreeCamera");
    return true;
}

bool Plugin::disable() {
    auto& logger = mSelf.getLogger();
    logger.info("Disabling FreeCamera...");
    // Code for disabling the plugin goes here.
    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.removeListener(mSetupCommandEventListener);
    FreeCamList.clear();
    ll::service::getLevel().get().forEachPlayer([](Player& pl) -> bool {
        FreeCamera::DisableFreeCamera(&pl);
        return true;
    });
    logger.info("FreeCamera Disabled!");
    return true;
}

} // namespace plugin