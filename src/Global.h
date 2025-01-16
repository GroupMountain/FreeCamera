#pragma once
#include <include_all.h>

#define MOD_NAME        "FreeCamera"
#define TARGET_PROTOCOL 766

extern std::unordered_set<uint64> FreeCamList;

extern void RegisterCommand();

extern std::string tr(std::string const& key, std::vector<std::string> const& data = {});

namespace FreeCamera {
extern void DisableFreeCamera(Player* pl);
extern void EnableFreeCamera(Player* pl);
extern void freecameraHook(bool enable);
} // namespace FreeCamera
