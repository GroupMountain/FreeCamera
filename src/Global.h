#pragma once
#include <mc/world/actor/player/Player.h>
#include <string>
#include <unordered_set>

#define MOD_NAME        "FreeCamera"
#define TARGET_PROTOCOL 819

extern std::unordered_set<uint64> FreeCamList;

extern void RegisterCommand();

namespace FreeCamera {
extern void DisableFreeCamera(Player* pl);
extern void EnableFreeCamera(Player* pl);
extern void freecameraHook(bool enable);
} // namespace FreeCamera
