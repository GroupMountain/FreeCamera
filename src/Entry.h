#pragma once
#include "Config.h"
#include "Global.h"

namespace FreeCamera {

class Entry {

public:
    static std::unique_ptr<Entry>& getInstance();

    Entry(ll::mod::NativeMod& self) : mSelf(self) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the mod is loaded successfully.
    bool load();

    /// @return True if the mod is enabled successfully.
    bool enable();

    /// @return True if the mod is disabled successfully.
    bool disable();

    bool unload();

    Config& getConfig();

    gmlib::i18n::LangI18n& getI18n();

private:
    ll::mod::NativeMod&     mSelf;
    std::optional<Config>   mConfig;
    std::optional<gmlib::i18n::LangI18n> mI18n;
};

} // namespace FreeCamera
