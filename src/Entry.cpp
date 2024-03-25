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