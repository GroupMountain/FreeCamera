#pragma once
#include "Plugin.h"
#include <GMLIB/Server/BinaryStreamAPI.h>
#include <GMLIB/Files/JsonLanguage.h>
#define PLUGIN_NAME "FreeCamera"

using namespace GMLIB;

extern ll::Logger logger;
extern std::unordered_set<uint64> FreeCamList;
extern void RegisterCommand();
extern void initLanguage();

extern nlohmann::json Language;

namespace FreeCamera {
    extern void DisableFreeCamera(Player* pl);
    extern void EnableFreeCamera(Player* pl);
}