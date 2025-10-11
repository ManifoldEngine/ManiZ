workspace "ManiZ"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    configurations { "Debug" }
    startproject "Sandbox"
    architecture "x64"
    language "C++"
    cppdialect "C++20"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    symbols "On"

    includedirs { "include/" }
    includedirs { "ThirdParties/ManiTests/include" }

project "TestModule"
    kind "StaticLib"
    location "Sandbox/%{prj.name}"
    files { "Sandbox/%{prj.name}/**.h", "Sandbox/%{prj.name}/**.cpp" }

project "Sandbox"
    kind "ConsoleApp"
    location "%{prj.name}"
    
    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    includedirs { "%{prj.name}/**" }
    links { "TestModule" }
