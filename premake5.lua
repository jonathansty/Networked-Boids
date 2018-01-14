require "raw"

local PROJECT_PATH = path.getabsolute('.')
local SOURCE_DIR = path.join(PROJECT_PATH,"src")
print("Source: " .. SOURCE_DIR)
local client_parameters = "-c 127.0.0.1:5555"
local server_parameters = "-s servers.txt"

INTERMEDIATE_DIR = path.join(PROJECT_PATH, "intermediate")
BUILD_DIR = path.join(INTERMEDIATE_DIR, "build")
PROJECT_DIR  = path.join(INTERMEDIATE_DIR, "projectfiles")
BINARY_DIR = path.join(PROJECT_PATH, "bin/" .. _TARGET_OS)
workspace "workspace"
	configurations{ "Debug", "Release" }
	architecture("x64")

	filter "platforms:Win64"
		system "Windows"

	filter {}

group("Applications")
local prj = project "boid-simulation"
	location(PROJECT_DIR)
	kind "ConsoleApp"
	language "C++"
	objdir(BUILD_DIR .. "/".. prj.name)
	targetdir(BINARY_DIR)

	debugdir(path.join(PROJECT_PATH,"src/")) 

	files{ 
		 path.join(SOURCE_DIR,"**.cpp"),
		 path.join(SOURCE_DIR,"**.h")
	}
	includedirs{
		SOURCE_DIR
	}

	includedirs{
		path.join(PROJECT_PATH,"ext/SFML/include")
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
		pchsource(path.join(SOURCE_DIR , "stdafx.cpp"))

	filter{}

filter "configurations:Debug"
	symbols "On"
	defines { "DEBUG" }
	target_name = "boid-simulation-debug"

filter "configurations:Release"
	defines{"NDEBUG"}
	target_name = "boid-simulation-release"
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
	targetname(target_name)
	
group("External")
dofile("ext/SFML.lua")

group("Launchers")
project "client"
	location(PROJECT_DIR)
	kind "ConsoleApp"
	debugdir(SOURCE_DIR)
	debugcommand(BINARY_DIR .. "/" .. target_name .. "%{cfg.buildtarget.extension}")
	debugargs(client_parameters)
	files{}
	dependson("boid-simulation")


project "server"
	location(PROJECT_DIR)
	kind "ConsoleApp"
	debugdir(SOURCE_DIR)
	debugcommand(BINARY_DIR .. "/" .. target_name .. "%{cfg.buildtarget.extension}")
	debugargs(server_parameters)
	files{}
	dependson("boid-simulation")
