#pragma once
#include <include_all.h>

#define PLUGIN_NAME "FreeCamera"
#define TARGET_PROTOCOL 662

using namespace GMLIB;

extern ll::Logger                 logger;
extern std::unordered_set<uint64> FreeCamList;
extern void                       RegisterCommand();
extern void                       initLanguage();

extern std::string tr(std::string key, std::vector<std::string> data = {});

namespace FreeCamera {
extern void DisableFreeCamera(Player* pl);
extern void EnableFreeCamera(Player* pl);
} // namespace FreeCamera