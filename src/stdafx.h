
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

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>


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