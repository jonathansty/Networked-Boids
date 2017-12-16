
/* Including c++ specifics */
#include <list>
#include <vector>
#include <cassert>
#include <memory>
#include <map>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>

#define SFML_STATIC
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>


// Prerequisites for linking statically
#if defined(SFML_STATIC)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"freetype.lib")
#pragma comment(lib,"jpeg.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"gdi32.lib")


#if defined(DEBUG) | defined(_DEBUG)
// Link debug
	#pragma comment(lib,"sfml-main-d.lib")
	#pragma comment(lib,"sfml-window-s-d.lib")
	#pragma comment(lib,"sfml-graphics-s-d.lib")
	#pragma comment(lib,"sfml-network-s-d.lib")
	#pragma comment(lib,"sfml-audio-s-d.lib")
	#pragma comment(lib,"sfml-system-s-d.lib")
#else
	#pragma comment(lib,"sfml-main.lib")
	#pragma comment(lib,"sfml-window-s.lib")
	#pragma comment(lib,"sfml-graphics-s.lib")
	#pragma comment(lib,"sfml-network-s.lib")
	#pragma comment(lib,"sfml-audio-s.lib")
	#pragma comment(lib,"sfml-system-s.lib")
#endif
#else
#if defined(DEBUG) | defined(_DEBUG)
// Link debug
	#pragma comment(lib,"sfml-main-d.lib")
	#pragma comment(lib,"sfml-window-d.lib")
	#pragma comment(lib,"sfml-graphics-d.lib")
	#pragma comment(lib,"sfml-network-d.lib")
	#pragma comment(lib,"sfml-audio-d.lib")
	#pragma comment(lib,"sfml-system-d.lib")
#else
	#pragma comment(lib,"sfml-main.lib");
	#pragma comment(lib,"sfml-window.lib")
	#pragma comment(lib,"sfml-graphics.lib")
	#pragma comment(lib,"sfml-network.lib")
	#pragma comment(lib,"sfml-audio.lib")
	#pragma comment(lib,"sfml-system.lib")
#endif
#endif
// Link statically

#include "Utility/StaticSettings.h"
#include "Utility/MathHelpers.h"
#include "Utility/GamePacket.h"


/* Pointer declarations to make our life easier */
template <typename T>
using TUnique = std::unique_ptr<T>;

template<typename T>
using TShared = std::shared_ptr<T>;

template<typename T>
using TWeak = std::weak_ptr<T>;


#ifdef WINDOWS
#define PLATFORM_WINDOWS 1
#define PLATFORM_LINUX 0
#endif
#ifdef LINUX
#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX 1
#endif