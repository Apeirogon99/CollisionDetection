#pragma once
#pragma warning(disable: 4275)

//Library
#ifdef _DEBUG
#pragma comment(lib, "sfml-main-s-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-audio-d.lib")
#pragma comment(lib, "sfml-network-d.lib")
#else
#pragma comment(lib, "sfml-main-s.lib")
#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")
#pragma comment(lib, "sfml-audio.lib")
#pragma comment(lib, "sfml-network.lib")
#endif

#define USE_SAT 0
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define FRAME 60

#pragma region DEFAULT
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <thread>
#include <vector>
#include <deque>
#include <chrono>
#include <mutex>
#include <random>
#include <unordered_set>
#pragma endregion

#pragma region TYPE
using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
#pragma endregion

#pragma region SFML
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#pragma endregion

#pragma region CONTENT
#include "TaskTimer.h"

#include "Actor.h"
#include "Attack.h"

#include "SAT.h"
#include "Detection.h"

#include "ICollisionSystem.h"
#include "KDTree.h"
#include "QuadTree.h"
#include "Array.h"

#include "Widget.h"
#include "CollisionConfig.h"
#pragma endregion

using namespace std;