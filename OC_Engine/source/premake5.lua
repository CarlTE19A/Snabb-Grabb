include "../premake/extensions.lua"

workspace "Crack Engine"
	location "../"
	startproject "Game"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Retail"
	}

-- include for common stuff 
include "../premake/common.lua"

include (dirs.external)
include (dirs.engine)

-------------------------------------------------------------
project "Game"
	location (dirs.projectfiles)
	dependson { "External", "Engine" }
		
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	links {"External", "Engine"}

	includedirs { dirs.external, dirs.engine, dirs.game }

	files {
		"../source/**.h",
		"../source/**.cpp",
		"../source/**.hpp",
		--"**.hlsl",
        --"**.hlsli"
	}

	excludes {
		"engine/**",
		"external/**",
	}

	libdirs { dirs.dependencies } -- dirs.lib,

	--links { "d3d11.lib", "assimp-vc143-mt.lib", "fmod_vc.lib", "fmodstudio_vc.lib", "libfbxsdk.lib", "libxml2-md.lib", "zlib-md.lib" }

	verify_or_create_settings("Game")
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		links { "freetype_Debug_x64.lib", "urlmon.lib", "wininet.lib" }
		--files {"tools/**"}
		--includedirs {"tools/"}
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		links{"freetype_Release_x64.lib", "urlmon.lib", "wininet.lib"}
		--files {"tools/**"}
		--includedirs {"tools/"}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"
		links{"freetype_Release_x64.lib", "urlmon.lib", "wininet.lib"}

	filter "system:windows"
--		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		
		defines {
			"WIN32",
			"_LIB", 
			"OC_SYSTEM_WINDOWS" 
		}
		
	--shadermodel("5.0")
    --local shader_dir = dirs.shaders.relative
    --os.mkdir(shader_dir)

    --filter("files:**.hlsl")
        --flags("ExcludeFromBuild")
        --shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")

   -- filter("files:**PS.hlsl")
        --removeflags("ExcludeFromBuild")
        --shadertype("Pixel")

    --filter("files:**VS.hlsl")
        --removeflags("ExcludeFromBuild")
        --shadertype("Vertex")

    --filter("files:**GS.hlsl")
        --removeflags("ExcludeFromBuild")
       -- shadertype("Geometry")