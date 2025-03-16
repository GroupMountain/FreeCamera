#include "Entry.h"
#include "Global.h"
#include "Language.h" // IWYU pragma: keep
#include "gmlib/gm/i18n/LangI18n.h"
#include "ll/api/Config.h"
#include "ll/api/Versions.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/Level.h"

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
        getSelf().getLogger().error(tr("error.protocolMismatch.info"));
        getSelf().getLogger().error(
            tr("error.protocolMismatch.version",
               {std::to_string(TARGET_PROTOCOL), std::to_string(ll::getNetworkProtocolVersion())})
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

std::string tr(std::string const& key, std::vector<std::string> const& data) {
    return FreeCamera::Entry::getInstance().getI18n().get(key, data);
}
