#include "Entry.h"
#include "Global.h"
#include "gmlib/gm/i18n/LangI18n.h"
#include "ll/api/Config.h"
#include "ll/api/Versions.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/Level.h"
#include "resource.h"

namespace FreeCamera {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() { return true; }

bool Entry::enable() {
    mConfig.emplace();
    if (!ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / u8"config.json")) {
        ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / u8"config.json");
    }
    mI18n.emplace(getSelf().getLangDir(), mConfig->language);
    mI18n->updateOrCreateLanguage("en_US", en_US);
    mI18n->updateOrCreateLanguage("zh_CN", zh_CN);
    mI18n->loadAllLanguages();
    if (ll::getNetworkProtocolVersion() != TARGET_PROTOCOL) {
        getSelf().getLogger().error("error.protocolMismatch.info"_tr());
        getSelf().getLogger().error(
            "error.protocolMismatch.version"_tr(TARGET_PROTOCOL, ll::getNetworkProtocolVersion())
        );
        return false;
    }
    FreeCamera::freecameraHook(true);
    RegisterCommand();
    return true;
}

bool Entry::disable() {
    FreeCamList.clear();
    ll::service::getLevel()->forEachPlayer([](Player& pl) -> bool {
        FreeCamera::DisableFreeCamera(&pl);
        return true;
    });
    return true;
}

bool Entry::unload() {
    mConfig.reset();
    mI18n.reset();
    FreeCamera::freecameraHook(false);
    return true;
}

Config& Entry::getConfig() { return mConfig.value(); }

gmlib::i18n::LangI18n& Entry::getI18n() { return mI18n.value(); }

} // namespace FreeCamera

LL_REGISTER_MOD(FreeCamera::Entry, FreeCamera::Entry::getInstance());