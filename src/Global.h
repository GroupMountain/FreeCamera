#pragma once
#include "Plugin.h"

extern ll::Logger logger;
extern std::unordered_set<uint64> FreeCamList;
extern void RegisterCommand(CommandRegistry &registry);

namespace FreeCamera {
    extern void DisableFreeCamera(Player* pl);
    extern void EnableFreeCamera(Player* pl);
}