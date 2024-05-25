#include "Entry.h"
#include "Global.h"
#include "Language.h"
ll::Logger logger(PLUGIN_NAME);

namespace FreeCamera {

std::unique_ptr<Entry>& Entry::getInstance() {
    static std::unique_ptr<Entry> instance;
    return instance;
}

bool Entry::load() {
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
    mConfig.emplace();
    if (!ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / u8"config.json")) {
        ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / u8"config.json");
    }
    mI18n.emplace(getSelf().getLangDir(), mConfig->language);
    mI18n->updateOrCreateLanguage("en_US", en_US);
    mI18n->updateOrCreateLanguage("zh_CN", zh_CN);
    mI18n->loadAllLanguages();
    mI18n->chooseLanguage(mConfig->language);
    RegisterCommand();
    return true;
}

bool Entry::disable() {
    mConfig.reset();
    FreeCamList.clear();
    ll::service::getLevel().get().forEachPlayer([](Player& pl) -> bool {
        FreeCamera::DisableFreeCamera(&pl);
        return true;
    });
    return true;
}

Config& Entry::getConfig() { return mConfig.value(); }

LangI18n& Entry::getI18n() { return *mI18n; }

} // namespace FreeCamera

LL_REGISTER_PLUGIN(FreeCamera::Entry, FreeCamera::Entry::getInstance());

std::string tr(std::string const& key, std::vector<std::string> const& data) {
    return FreeCamera::Entry::getInstance()->getI18n().get(key, data);
}