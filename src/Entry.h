#pragma once
#include "Config.h"
#include "Global.h"

namespace FreeCamera {

using namespace GMLIB::Files::I18n;

class Entry {

public:
    static std::unique_ptr<Entry>& getInstance();

    Entry(ll::plugin::NativePlugin& self) : mSelf(self) {}

    [[nodiscard]] ll::plugin::NativePlugin& getSelf() const { return mSelf; }

    /// @return True if the plugin is loaded successfully.
    bool load();

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

    bool unload();

    Config& getConfig();

    LangI18n& getI18n();

private:
    ll::plugin::NativePlugin& mSelf;
    std::optional<Config>     mConfig;
    std::optional<LangI18n>   mI18n;
};

} // namespace FreeCamera
