set_xmakever("2.5.5")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
set_languages("c++20")
set_arch("x64")

target("cppio")
    set_kind("static")
    add_files("src/cppio/*.cpp")
    add_files("src/cppio/impl/*.cpp")
    add_files("src/cppio/network/*.cpp")
    add_files("src/cppio/network/http/*.cpp")
    add_files("src/cppio/network/redis/*.cpp")

    add_headerfiles("src/cppio/*.hpp", "src/cppio/*.inl")
    add_headerfiles("src/cppio/impl/*.hpp", "src/cppio/impl/*.inl")
    add_headerfiles("src/cppio/network/*.hpp", "src/cppio/network/*.inl")
    add_headerfiles("src/cppio/network/http/*.hpp", "src/cppio/network/http/*.inl")
    add_headerfiles("src/cppio/network/redis/*.hpp", "src/cppio/network/redis/*.inl")

    add_includedirs("src/", {public = true})
    add_includedirs("vendor/outcome/single-header", {public = true})

    if is_plat("windows") then
        add_headerfiles("src/cppio/network/win32/*.hpp", "src/cppio/network/win32/*.inl")
        add_headerfiles("src/cppio/impl/win32/*.hpp", "src/cppio/impl/win32/*.inl")

        add_files("src/cppio/network/win32/*.cpp")
        add_files("src/cppio/impl/win32/*.cpp")

        add_syslinks("ws2_32", "Mswsock")
    else
        add_headerfiles("src/cppio/network/posix/*.hpp", "src/ncppio/etwork/posix/*.inl")
        add_headerfiles("src/cppio/impl/posix/*.hpp", "src/cppio/impl/posix/*.inl")

        add_files("src/cppio/network/posix/*.cpp")
        add_files("src/cppio/impl/posix/*.cpp")

        add_syslinks("pthread")
    end


target("tests")
    set_kind("binary")
    add_files("tests/**.cpp")
    add_headerfiles("tests/**.hpp")
    add_includedirs("tests/")
    add_deps("cppio")
