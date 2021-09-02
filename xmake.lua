set_xmakever("2.5.5")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
set_languages("c++20")
set_arch("x64")

target("cppio")
    set_kind("static")
    add_files("src/*.cpp")
    add_headerfiles("src/*.hpp", "src/*.inl")
    add_includedirs("src/", {public = true})
    add_includedirs("vendor/outcome/single-header", {public = true})

    if is_plat("windows") then
        add_files("src/win32/*.cpp")
        add_headerfiles("src/win32/*.hpp", "src/win32/*.inl")

        add_syslinks("ws2_32", "Mswsock")
    else
        add_files("src/posix/*.cpp")
        add_headerfiles("src/posix/*.hpp", "src/posix/*.inl")

        add_syslinks("pthread")
    end


target("tests")
    set_kind("binary")
    add_files("tests/**.cpp")
    add_headerfiles("tests/**.hpp")
    add_includedirs("tests/")
    add_deps("cppio")
