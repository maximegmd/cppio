set_xmakever("2.5.5")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
set_languages("c++20")
set_arch("x64")

target("cppio")
    set_kind("static")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src/", {public = true})
    add_syslinks("ws2_32", "Mswsock")

target("tests")
    set_kind("binary")
    add_files("tests/**.cpp")
    add_headerfiles("tests/**.h")
    add_includedirs("tests/")
    add_deps("cppio")
