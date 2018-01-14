local prj = path.getabsolute('.')
local source_directory = path.join(path.getabsolute("."),"src")
print("Source: " .. source_directory)
local client_parameters = "-c 127.0.0.1:5555"
local server_parameters = "-s servers"

workspace "Network-Research"
	filename(_ACTION .. "_AppBoids" )
	configurations{ "Debug", "Release" }
	architecture("x64")

	filter "platforms:Win64"
		system "Windows"

	filter {}

group("Applications")
project "boid-simulation"
	location("intermediate/build")
	kind "ConsoleApp"
	language "C++"
	objdir(path.join(prj,"intermediate/obj"))
	targetdir (path.join(path.getabsolute('.'),"bin/" .. _TARGET_OS))
	debugdir(path.join(prj,"src/")) 

	files{ 
		 path.join(source_directory,"**.cpp"),
		 path.join(source_directory,"**.h")
	}
	includedirs{
		source_directory
	}

	includedirs{
		path.join(prj,"ext/SFML/include")
	}

	links{ 
	 "sfml-audio",
	 "sfml-graphics",
	 "sfml-network",
	 "sfml-system",
	 "sfml-window"
	  }


  -- Due to weirdness in precompiled headers these have to be specific to compiler
	filter "action:not vs*"
		pchheader "stdafx.h"
		buildoptions{
			"-std=c++17"
		}

	filter "action:vs*"  
		pchheader "stdafx.h"
		pchsource(path.join(source_directory , "stdafx.cpp"))

	filter{}

filter "configurations:Debug"
	symbols "On"
	defines { "DEBUG" }

filter "configurations:Release"
	defines{"NDEBUG"}
	optimize "On"	

filter {"system:windows"}
	defines{"WINDOWS"}
	defines{"SFML_STATIC"}
	links{
		"opengl32",
		"winmm",
		"gdi32",
		"ws2_32"
	}

-- filter {"system:windows", "configurations:Debug"}
-- 	links{
-- 		"sfml-system-s-d",
-- 		"sfml-graphics-s-d",
-- 		"sfml-window-s-d",
-- 		"sfml-main-d",
-- 		"sfml-network-s-d"
-- 	}

-- filter {"system:windows", "configurations:Release"}
-- 	links{
-- 		"sfml-system-s",
-- 		"sfml-graphics-s",
-- 		"sfml-window-s",
-- 		"sfml-main",
-- 		"sfml-network-s"
-- 	}

filter "system:linux"
	defines{"LINUX"}

filter {}

group("External")
dofile("ext/SFML.lua")

group("Launchers")
-- project "AppBoids_Client"
-- 	filename(_ACTION .. "_AppBoids_Client")
-- 	kind "ConsoleApp"
-- 	language "C++"
-- 	debugdir "bin/%{cfg.buildcfg}"
-- 	debugcommand("bin/%{cfg.buildcfg}/AppBoids.exe ")
-- 	debugargs(client_parameters)
-- 	files{}
-- 	dependson("AppBoids")


-- project "AppBoids_Server"
-- 	filename(_ACTION .. "_AppBoids_Server")
-- 	kind "ConsoleApp"
-- 	language "C++"
-- 	debugdir "bin/%{cfg.buildcfg}"
-- 	debugcommand("bin/%{cfg.buildcfg}/AppBoids.exe")
-- 	debugargs(server_parameters)
-- 	files{}
-- 	dependson("AppBoids")
