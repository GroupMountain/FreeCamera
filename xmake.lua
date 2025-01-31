add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("groupmountain-repo https://github.com/GroupMountain/xmake-repo.git")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

-- Option 1: Use the latest version of LeviLamina released on GitHub.
add_requires("levilamina 1.0.0")
add_requires("levibuildscript 0.3.0")
add_requires("gmlib 0.13.10")

target("FreeCamera") -- Change this to your mod name.
    add_cxflags(
        "/EHa",
        "/utf-8"
    )
    add_files(
        "src/**.cpp"
    )
    add_includedirs(
        "src"
    )
    add_packages(
        "levilamina",
        "gmlib"
    )
    add_defines(
        "NOMINMAX", 
        "UNICODE", 
        "_HAS_CXX17",
        "_HAS_CXX20",
        "_HAS_CXX23"
    )
    add_rules("@levibuildscript/linkrule")
    set_exceptions("none")
    set_kind("shared")
    set_languages("cxx20")

    after_build(function (target)
        local mod_packer = import("scripts.after_build")

        local mod_define = {
            modName = target:name(),
            modFile = path.filename(target:targetfile()),
        }
        
        mod_packer.pack_mod(target,mod_define)
    end)