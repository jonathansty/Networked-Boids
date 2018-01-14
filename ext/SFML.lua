local compiler = "nan"

SFML_MODULES = {
"Audio",
"Window",
"Graphics",
"Network",
"System"
}

for i,module_name in ipairs( SFML_MODULES ) do
project ("sfml-" .. module_name:lower() )

	kind "StaticLib"
	objdir "build/obj"
	location "build"
	targetdir "lib"

	language "C++"
	defines{
		"SFML_".. module_name:upper() .. "_EXPORTS",
		"SFML_STATIC"
	}
	files{
		"SFML/src/SFML/" .. module_name .."/*.cpp",
		"SFML/src/SFML/" .. module_name .."/*.hpp",
		"SFML/include/SFML/".. module_name .. "/*.hpp"
	}

	links{
		-- "flac",
		"freetype",
		"ogg",
		"openal32",
		"vorbis",
		-- "vorbisenc",
		-- "vorbisfile"
	}
	sysincludedirs{
		"SFML/include",
		"SFML/src",
		"SFML/extlibs/headers",
		"SFML/extlibs/headers/AL",
		"SFML/extlibs/headers/freetype2",
		"SFML/extlibs/headers/ogg",
		"SFML/extlibs/headers/stb_image",
		"SFML/extlibs/headers/vorbis"
	}

	output_name = "sfml-" .. module_name:lower() .."-s"
	filter "configurations:Debug"
		output_name = output_name .. "-d"
		optimize "Off"
		symbols "On"

	filter "toolset:msc"
		compiler = "msvc"
		libdirs{
			"SFML/extlibs/libs-msvc/x64"
		}
		translated_platform = "Win32"

	filter "toolset:gcc"
		compiler = "msvc"
		libdirs{
			"SFML/extlibs/libs-msvc/x64"
		}

	filter "toolset:clang"
		compiler = "msvc"

	filter {}
		print( "Using \"" .. output_name .."\" as our file name!" )
		targetname( output_name )

		print( "Using \"" .. translated_platform .."\" as our platform name!" )
		files{
			"SFML/src/SFML/"..  module_name .. "/" .. translated_platform .. "/**.*pp"
		}

		print("Using external libs for the \"".. compiler .. "\" toolset!")
		libdirs{
			"SFML/extlibs/libs-" .. compiler .. "/x64"
		}

		removefiles{
			"SFML/src/SFML/Window/EGLCheck.cpp",
			"SFML/src/SFML/Window/EGLCheck.hpp",
			"SFML/src/SFML/Window/EglContext.cpp",
			"SFML/src/SFML/Window/EglContext.hpp",
		}

end
