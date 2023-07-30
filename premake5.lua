workspace "OpenGL"
	architecture "x64"
	startproject "Skybox"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["SFML"] = "vendor/SFML/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["GLM"] = "vendor/GLM/include"
IncludeDir["assimp"] = "vendor/assimp/include"
IncludeDir["assimpBuild"] = "vendor/assimp/Build/include"

group "Dependencies"
	include "vendor/Glad"

group ""

project "Skybox"
	location "Skybox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.SFML}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.assimpBuild}"
	}

	libdirs 
	{
		 "vendor/SFML/lib",
		 "vendor/SFML/lib/Debug",
		 "vendor/assimp/Build/lib/Release",
		 "vendor/assimp/Build/lib/Debug",
		 "vendor/assimp/Build/contrib/zlib/Debug",
		 "vendor/assimp/Build/contrib/zlib/Release"
	}

	defines
	{
		"SFML_STATIC"
	}

	links
	{
		"Glad",
		"opengl32.lib",
		"freetype.lib",
		"winmm.lib",
		"gdi32.lib",
		"openal32.lib",
		"flac.lib",
		"vorbisenc.lib",
		"vorbisfile.lib",
		"vorbis.lib",
		"ogg.lib",
		"ws2_32.lib"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		links
		{
			"sfml-audio-s-d.lib",
			"sfml-graphics-s-d.lib",
			"sfml-network-s-d.lib",
			"sfml-system-s-d.lib",
			"sfml-window-s-d.lib",
			"assimp-vc143-mtd.lib",
			"zlibstaticd.lib"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		links
		{
			"sfml-audio-s.lib",
			"sfml-graphics-s.lib",
			"sfml-network-s.lib",
			"sfml-system-s.lib",
			"sfml-window-s.lib",
			"assimp-vc143-mt.lib",
			"zlibstatic.lib"
		}

-- project "Sandbox"
-- 	location "Sandbox"
-- 	kind "ConsoleApp"
-- 	language "C++"
-- 	staticruntime "Off"

-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir ("int/" .. outputdir .. "/%{prj.name}")

-- 	files
-- 	{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}
	
-- 	includedirs
-- 	{
-- 		"Timefall/vendor/spdlog/include",
-- 		"Timefall/src"
-- 	}

-- 	links
-- 	{
-- 		"Timefall"
-- 	}
	
-- 	filter "system:windows"
-- 		cppdialect "C++17"
-- 		systemversion "latest"

-- 		defines
-- 		{
-- 			"TF_PLATFORM_WINDOWS"			
-- 		}

-- 	filter "configurations:Debug"
-- 		defines "TF_DEBUG"
-- 		runtime "Debug"
-- 		symbols "On"

-- 	filter "configurations:Release"
-- 		defines "TF_RELEASE"
-- 		runtime "Release"
-- 		optimize "On"

-- 	filter "configurations:Dist"
-- 		defines "TF_DIST"
-- 		runtime "Release"
-- 		optimize "On"