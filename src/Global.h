#pragma once
#include "Plugin.h"
#include <ll/api/command/DynamicCommand.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/command/SetupCommandEvent.h>
#include <ll/api/plugin/NativePlugin.h>
#include <ll/api/memory/Hook.h>
#include "include_mcapi.h"

extern std::unordered_set<uint64> FreeCamList;
extern void RegisterCommand(CommandRegistry &registry);

namespace FreeCamera {
    extern void DisableFreeCamera(Player* pl);
    extern void EnableFreeCamera(Player* pl);
}