include "../../premake/common.lua"

project "Engine"
	location (dirs.projectfiles)

	language "C++"
	cppdialect "C++17"
	kind "StaticLib"

	targetdir (dirs.lib)
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	pchheader "stdafx.h"
	pchsource "stdafx.cpp"

	files {
		"**.h",
		"**.cpp",
		"**.hpp",
		"**.hlsl",
		"**.hlsli",
	}

	linkoptions { "-IGNORE:4006"}

	includedirs {
		".",
		dirs.external,
		dirs.external .. "imgui/",
	}

	libdirs { dirs.dependencies }
	
	links { "d3d11.lib", "assimp-vc143-mt.lib", "fmod_vc.lib", "fmodstudio_vc.lib", "libfbxsdk.lib", "libxml2-md.lib", "zlib-md.lib" }

	filter "configurations:Debug"
	defines {"_DEBUG"}
	libdirs {dirs.px_debug}
	links { "DirectXTK-d.lib", "PhysX_64.lib", "PhysXCommon_64.lib", "PhysXFoundation_64.lib", "PVDRuntime_64.lib", "PhysXExtensions_static_64.lib", "PhysXPvdSDK_static_64.lib", "PhysXCooking_64.lib", "PhysXCharacterKinematic_static_64.lib"}
	runtime "Debug"
	symbols "on"
filter "configurations:Release"
	defines "_RELEASE"
	libdirs {dirs.px_release}
	links { "DirectXTK.lib","PhysX_64.lib", "PhysXCommon_64.lib", "PhysXFoundation_64.lib", "PVDRuntime_64.lib", "PhysXExtensions_static_64.lib", "PhysXPvdSDK_static_64.lib", "PhysXCooking_64.lib", "PhysXCharacterKinematic_static_64.lib"}
	runtime "Release"
	optimize "on"
filter "configurations:Retail"
	defines "_RETAIL"
	libdirs {dirs.px_release}
	links { "DirectXTK.lib","PhysX_64.lib", "PhysXCommon_64.lib", "PhysXFoundation_64.lib", "PVDRuntime_64.lib", "PhysXExtensions_static_64.lib", "PhysXPvdSDK_static_64.lib", "PhysXCooking_64.lib", "PhysXCharacterKinematic_static_64.lib"}
	runtime "Release"
	optimize "on"

filter "system:windows"
	kind "StaticLib"
	staticruntime "off"
	symbols "On"		
	systemversion "latest"
	warnings "Extra"
	sdlchecks "true"
	--conformanceMode "On"
	--buildoptions { "/permissive" }
	flags { 
	--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		"FatalCompileWarnings",
		"MultiProcessorCompile"
	}
	links {
	--	"DXGI",
	--	"avcodec.lib",
	--	"avdevice.lib",
	--	"avfilter.lib",
	--	"avformat.lib",
	--	"avutil.lib",
	--	"swscale.lib",
	--	"swresample.lib"
	}

	defines {
		"WIN32",
		"_CRT_SECURE_NO_WARNINGS", 
		"_LIB", 
		"_WIN32_WINNT=0x0601",
		"OC_SYSTEM_WINDOWS" 
	}



	shadermodel("5.0")
    local shader_dir = dirs.shaders.relative
    os.mkdir(shader_dir)

    filter("files:**.hlsl")
        flags("ExcludeFromBuild")
        shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")

    filter("files:**PS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Pixel")

    filter("files:**VS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Vertex")

    filter("files:**GS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Geometry")
    filter("files:**CS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Compute")
