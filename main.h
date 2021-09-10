#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#ifdef _DEBUG
#pragma comment(lib, "libs\\debug\\DirectxTex.lib")
#else
#pragma comment(lib, "libs\\release\\DirectXTex.lib")
#endif

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3dx12.h"
#include "DirectXTex.h"
#include <directxmath.h>
#include <directxcolors.h>
#include <string>
#include <chrono>
#include <algorithm>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef CreateWindow
#undef CreateWindow
#endif

using namespace DirectX;
using namespace Microsoft::WRL;

#include "error.h"
#include "time.h"
#include "window.h"
#include "directx.h"
#include "texture.h"
#include "camera.h"

// Tests
#include "tests\cube.h"

#include "entry.h"

// TODO: I think we need to implement resizing but to resize need to store some global state

// Compilation: 
// DEBUG: cl /Zi /MTd /D _DEBUG main.cpp
// RELEASE: cl main.cpp

// Shader compilation:
// PIXEL SHADER: fxc /T ps_5_1 /Fo ps.cso ps.hlsl
// VERTEX SHADER: fxc /T vs_5_1 /Fo vs.cso vs.hlsl

// 1. Create Box (V)
// 2. Create Texture (V)
// 3. Create Camera (V)
// 4. Create constant buffer for a cube (...)
// 5. Create descriptor allocator (...)
// N. Create thousands moving colored cubes like in Sebastian video (...)