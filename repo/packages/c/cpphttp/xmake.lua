package("cpphttp")
    set_homepage("https://github.com/ink19/cpphttp")
    set_description("A http library for CPP.")

    set_urls("https://github.com/ink19/cpphttp.git")

    on_install("linux", function (package)
        import("package.tools.xmake").install(package)
        os.cp("include/*", package:installdir("include", "cpphttp"))
    end)
