#pragma once
#include "MonoBehaviour.h"
#include "ModelRenderer.h"
#include "BaseCollider.h"
// 생성후 패스, float3 인자던지기 필수
// _renderer->Set_PassType(_PassType);
// _renderer->SetFloat(3, m_MapObjDesc.fUVWeight);
class MapObjectScript :
    public MonoBehaviour
{
public:
    typedef struct MapObjectDesc
    {
        string strName = "";
        _float fUVWeight = { 1.f }; // UV비율 : 기본1배
        _bool bShadow = false;
        _bool bBlur = false;
    // Component
        // Transform
        _bool bTransform = true;
        _float4x4 WorldMatrix = XMMatrixIdentity();
        // Collider
        _bool bCollider = false;
        //ColliderType ColliderType = ColliderType::OBB;
        // 0:Sphere 1:AABB 2:OBB 3:Mesh
        _int ColliderType = static_cast<_int>(ColliderType::OBB);
            // ColliderDesc
        _float3 ColliderOffset = _float3{ 0.f, 0.f, 0.f };
        _float ColRadius = { 0.f };
        _float3 ColBoundingSize = _float3{ 0.f, 0.f, 0.f };
        string ColModelName = "";
    // Culling
        _float3 CullPos = _float3{ 0.f, 0.f, 0.f };
        _float CullRadius = { 0.f };
    // CullDirection
        _char bCullNone = 0;
    // DummyData
        _float4x4 matDummyData = _float4x4::Identity;
    }MAPOBJDESC;

public:
    MapObjectScript(MAPOBJDESC _MapObjDesc);
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<MapObjectScript>(m_MapObjDesc); }

public:
    _float Get_UVWeight() { return m_MapObjDesc.fUVWeight; }
    MAPOBJDESC& Get_DESC() { return m_MapObjDesc; }

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;

private:
    MAPOBJDESC m_MapObjDesc;
};

