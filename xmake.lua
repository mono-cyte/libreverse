add_rules("mode.debug", "mode.release")


target("libreverse")
    set_kind("static")
    add_includedirs("include")
    add_headerfiles("include/**")
    add_files("src/*.cpp")




package("libreverse")
    set_description("The reverse lib package")
    set_license("Apache-2.0")

    on_load(function (package)
        package:set("installdir",path.join(
            os.scriptdir(),
            package:plat(),
            package:arch(),
            package:mode())
        )
    end)

    on_fetch(function (package)
        local result = {}
        result.links = "libreverse"
        result.linkdirs = package:installdir("lib")
        result.includedirs = package:installdir("include")
        return result
    end)