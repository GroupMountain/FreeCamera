#include "Entry.h"
#include "Global.h"
ll::Logger logger(PLUGIN_NAME);

namespace FreeCamera {

std::unique_ptr<Entry>& Entry::getInstance() {
    static std::unique_ptr<Entry> instance;
    return instance;
}

bool Entry::load() {
    initLanguage();
    if (Version::getProtocolVersion() != TARGET_PROTOCOL) {
        logger.error(tr("error.protocolMismatch.info"));
        logger.error(
            tr("error.protocolMismatch.version",
               {std::to_string(TARGET_PROTOCOL), std::to_string(Version::getProtocolVersion())})
        );
        return false;
    }
    return true;
}

bool Entry::enable() {
    RegisterCommand();
    return true;
}

bool Entry::disable() {
    FreeCamList.clear();
    ll::service::getLevel().get().forEachPlayer([](Player& pl) -> bool {
        FreeCamera::DisableFreeCamera(&pl);
        return true;
    });
    return true;
}

} // namespace FreeCamera

LL_REGISTER_PLUGIN(FreeCamera::Entry, FreeCamera::Entry::getInstance());