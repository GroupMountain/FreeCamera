#include "Config.h"
#include "Global.h"
#include "Language.h"

GMLIB::Files::JsonConfig*     Config   = nullptr;
GMLIB::Files::I18n::JsonI18n* Language = nullptr;

void initLanguage() {
    Config = new GMLIB::Files::JsonConfig("./plugins/FreeCamera/config/config.json", defaultConfig);
    Config->init();
    std::string languageCode = Config->getValue<std::string>({"language"}, "en_US");
    std::string langPath     = "./plugins/FreeCamera/language/";
    Language                 = new GMLIB::Files::I18n::JsonI18n(langPath);
    Language->updateOrCreateLanguage("en_US", defaultLanguage_en_US);
    Language->updateOrCreateLanguage("zh_CN", defaultLanguage_zh_CN);
    Language->loadAllLanguages();
    Language->chooseLanguage(languageCode);
}

std::string tr(std::string key, std::vector<std::string> data) { return Language->translate(key, data); }