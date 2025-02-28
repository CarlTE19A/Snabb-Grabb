project "External"
	location "%{wks.location}/local/"--(dirs.projectfiles)
		
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"

	targetdir ("../../lib/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../temp/%{prj.name}/%{cfg.buildcfg}")

	files {
		"**.h",
		"**.hpp",
		"**.inl",
		"**.c",
		"**.cpp",
	}

	excludes {
		--"ffmpeg-2.0/**.h",
		--"ffmpeg-2.0/**.c",
		--"ffmpeg-2.0/**.cpp",
	}

	includedirs {
		".",
		"../source/",
		"imgui/",
	}

	libdirs { "lib/" }

	defines {"_CONSOLE"}

	filter "configurations:Debug"
	defines {"_DEBUG" } --"FBXSDK_SHARED"
	runtime "Debug"
	symbols "on"
	
	--libdirs { "TGAFBXImporter/FBXSDK/lib/debug" }
	links {
	--	"libfbxsdk",
	}

filter "configurations:Release"
	defines {"_RELEASE"}
	runtime "Release"
	optimize "on"

	--libdirs { "TGAFBXImporter/FBXSDK/lib/release" }
	links {
	--	"libfbxsdk",
	}

filter "configurations:Retail"
	defines {"_RETAIL"}
	runtime "Release"
	optimize "on"

	--libdirs { "TGAFBXImporter/FBXSDK/lib/release" }
	links {
	--	"libfbxsdk",
	}

filter "system:windows"
	kind "StaticLib"
	staticruntime "off"
	symbols "On"		
	systemversion "latest"
	warnings "Off"
	--conformanceMode "On"
	--buildoptions { "/permissive" }
	flags { 
	--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
	--	"FatalCompileWarnings",
		"MultiProcessorCompile"
	}
	links {
	--	"DXGI",
	--	"dxguid",
	}

	defines { "_WIN32_WINNT=0x0601" }
