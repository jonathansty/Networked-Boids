local prj = path.getabsolute('.')
local source_directory = path.join(path.getabsolute("."),"src")
print("Source: " .. source_directory)
local client_parameters = "-c 127.0.0.1:5555"
local server_parameters = "-s servers"

require("vscode")
workspace "Network-Research"
	filename(_ACTION .. "_AppBoids" )
	configurations{ "Debug", "Release" }

project "BoidsSimulation"
	location("build")
	kind "ConsoleApp"
	filename(_ACTION .. "_networked")
	language "C++"
	targetdir (path.join(path.getabsolute('.'),"bin/" .. _TARGET_OS))
	debugdir(path.join(prj,"src/")) 

	files{ 
		 path.join(source_directory,"**.cpp"),
		 path.join(source_directory,"**.h")
	}
	includedirs{
		source_directory
	}
	links{
		"sfml-system",
		"sfml-graphics",
		"sfml-window",
		"sfml-network"
	}

  -- Due to weirdness in precompiled headers these have to be specific to compiler
	filter "action:not vs*"
		pchheader "stdafx.h"

	filter "action:vs*"  
		pchheader "stdafx.h"
		pchsource(path.join(source_directory , "/stdafx.cpp"))

	filter "*"
	buildoptions{
		"-std=c++17"
	}


filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "On"

filter "configurations:Release"
	defines{"NDEBUG"}
	optimize "On"	

filter "system:windows"
	postbuildcommands{
		"xcopy \"../" .. source_directory .. "/Resources\" \"../bin/%{cfg.buildcfg}/Resources\" /s /y /i"
	}
	defines{"WINDOWS"}

filter "system:linux"
	defines{"LINUX"}

filter {}
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
