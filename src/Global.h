#pragma once
#include "Plugin.h"
#include <GMLIB/Files/JsonLanguage.h>
#include <GMLIB/Server/BinaryStreamAPI.h>
#define PLUGIN_NAME "FreeCamera"

using namespace GMLIB;

extern ll::Logger                 logger;
extern std::unordered_set<uint64> FreeCamList;
extern void                       RegisterCommand();
extern void                       initLanguage();

extern std::string tr(std::string key);

namespace FreeCamera {
extern void DisableFreeCamera(Player* pl);
extern void EnableFreeCamera(Player* pl);
} // namespace FreeCamera