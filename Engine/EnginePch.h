#pragma once

//Engine Headers
#include "Types.h"
#include "Define.h"
#include "Struct.h"
#include "Enum.h"

// STL
#include <memory>
#include <io.h>
#include <iostream>
#include <random>
#include <fstream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <string>
#include <algorithm>
using namespace std;

// WIN
#include <windows.h>
#include <optional>
#include <chrono>

using namespace chrono;

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include <FX11/d3dx11effect.h>
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
using namespace DirectX;
using namespace Microsoft::WRL;

//FMOD
#include "FMOD/fmod.hpp"

// Physx
#include "Physx/PxPhysicsAPI.h"
using namespace physx;

// Libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib , "FMOD/fmod_vc.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "FX11/Effects11d.lib")
#pragma comment(lib, "DirectXTK/Debug/DirectXTK.lib")

#pragma comment(lib, "Physx/debug/PhysX_64.lib")
#pragma comment(lib, "Physx/debug/PhysXCommon_64.lib")
#pragma comment(lib, "Physx/debug/PhysXCooking_64.lib")
#pragma comment(lib, "Physx/debug/PhysXFoundation_64.lib")
#pragma comment(lib, "Physx/debug/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "Physx/debug/PhysXExtensions_static_64.lib")
#pragma comment(lib, "Physx/debug/PVDRuntime_64.lib")
#pragma comment(lib, "Physx/debug/PhysXCharacterKinematic_static_64.lib")


#else
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "FX11/Effects11.lib")
#pragma comment(lib, "DirectXTK/Release/DirectXTK.lib")

#pragma comment(lib, "Physx/Release/PhysX_64.lib")
#pragma comment(lib, "Physx/Release/PhysXCommon_64.lib")
#pragma comment(lib, "Physx/Release/PhysXCooking_64.lib")
#pragma comment(lib, "Physx/Release/PhysXFoundation_64.lib")
#pragma comment(lib, "Physx/Release/PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "Physx/Release/PhysXExtensions_static_64.lib")
#pragma comment(lib, "Physx/Release/PVDRuntime_64.lib")
#pragma comment(lib, "Physx/Release/PhysXCharacterKinematic_static_64.lib")

#endif



// Managers
#include "GameInstance.h"
#include "Graphics.h"
#include "InputMgr.h"
#include "SoundMgr.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "InstancingMgr.h"
#include "CollisionMgr.h"
#include "PhysxMgr.h"
#include "EventMgr.h"

// Engine
#include "VertexData.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Shader.h"

// Often Used
#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"