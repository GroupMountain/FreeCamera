#include "Language.h"
#include "Global.h"
#include <GMLIB/Files/JsonConfig.h>

GMLIB::Files::JsonConfig* Config = nullptr;
nlohmann::json            Language;

void initLanguage() {
    Config = new GMLIB::Files::JsonConfig("./plugins/FreeCamera/config/config.json", defaultConfig);
    Config->initConfig();
    std::string langPath = "./plugins/FreeCamera/language/{language}.json";
    std::string language = Config->getValue<std::string>({"language"}, "zh_CN");
    ll::utils::string_utils::replaceAll(langPath, "{language}", language);
    Language = GMLIB::Files::JsonLanguage::initLanguage(langPath, defaultLanguage);
}

std::string tr(std::string key) { return GMLIB::Files::JsonLanguage::translate(Language, key); }