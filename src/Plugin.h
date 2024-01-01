#pragma once

#include <ll/api/plugin/NativePlugin.h>
#include <ll/api/event/ListenerBase.h>

namespace plugin {

class Plugin {
public:
    Plugin(ll::plugin::NativePlugin& self);

    Plugin(Plugin&&)                 = delete;
    Plugin(const Plugin&)            = delete;
    Plugin& operator=(Plugin&&)      = delete;
    Plugin& operator=(const Plugin&) = delete;

    ~Plugin() = default;

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

private:
    ll::plugin::NativePlugin& mSelf;
    ll::event::ListenerPtr mSetupCommandEventListener;
    ll::event::ListenerPtr mPlayerLeftEventListener;
};

} // namespace plugin
