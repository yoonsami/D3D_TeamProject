#pragma once
#include "Component.h"
#include "Frustum.h"

enum class ProjectionType
{
    Perspective,
    Orthographic,
};

struct CameraDesc
{
	wstring strName = L"";
	_float fFOV = XM_PI / 4.f;
	_float fSizeX = 1920.f;
	_float fSizeY = 1080.f;
	_float fNear = 0.1f;
	_float fFar = 100.f;
    ProjectionType eType = ProjectionType::Perspective;
};

class Camera :
    public Component, public enable_shared_from_this<Camera>
{
    
public:
    Camera(CameraDesc desc = CameraDesc());
    virtual ~Camera();

public:
    virtual HRESULT Init() override;
    virtual void Late_Tick() override;
    virtual void Final_Tick() override;
    const wstring& Get_CameraName() { return m_Desc.strName; }

    void Set_ProjType(ProjectionType eType) { m_eType = eType; }
    ProjectionType Get_ProjType() { return m_eType; }

    const  _float4x4& Get_ViewMat() { return m_matView; }
    const  _float4x4& Get_ProjMat() { return m_matProj; }
    CameraDesc& Get_CameraDesc() { return m_Desc; }
    void Update_Matrix();

	static const _float4x4& Get_View() { return S_View; }
	static const _float4x4& Get_Proj() { return S_Proj; }

public:
    void Sort_GameObject(shared_ptr<Scene> scene);
    void Sort_ShadowObject(shared_ptr<Scene> scene);
    void Render_DistrotionEffects();
    void Render_BlurForward();
    void Render_SkyBox();
    void Render_Forward();
    void Render_Deferred();
    void Render_Shadow();

    void Render_MotionBlur();
    void Render_AfterUI();
    void Set_CullingMaskLayerOnOff(_uint layer, _bool on)
    {
        if (on)
            m_iCullingMask |= (1 << layer);
        else
            m_iCullingMask &= ~(1 << layer);
    }
    void Set_CullingMaskAll() { Set_CullingMask(UINT32_MAX); }
    void Set_CullingMask(_uint mask) { m_iCullingMask = mask; }
    void Set_EffectToolOn(_bool bEffectToolOn) { m_bEffectToolMode_On = bEffectToolOn; }
    _bool IsCulled(_uchar layerIndex) { return (m_iCullingMask & (1 << layerIndex)) != 0; }

    Frustum& Get_Frustum() { return frustum; }
    template<typename T>
    shared_ptr<T> Get_MainScript();

private:
    vector<shared_ptr<MonoBehaviour>>& Get_Scripts();
    ProjectionType m_eType = ProjectionType::Perspective;
    _float4x4 m_matView = _float4x4::Identity;
    _float4x4 m_matProj = _float4x4::Identity;

    CameraDesc m_Desc{};

    Frustum frustum;

private:
    _uint m_iCullingMask = 0;
    vector<shared_ptr<GameObject>> m_Sky;
    vector<shared_ptr<GameObject>> m_Water;
    vector<shared_ptr<GameObject>> m_Forward;
    vector<shared_ptr<GameObject>> m_VelocityMapObj;
    vector<shared_ptr<GameObject>> m_Deferred;
    vector<shared_ptr<GameObject>> m_Effect;
    vector<shared_ptr<GameObject>> m_Particle;
    vector<shared_ptr<GameObject>> m_Shadow;
    vector<shared_ptr<GameObject>> m_DistortionEffects;
    vector<shared_ptr<GameObject>> m_Trails;
    vector<shared_ptr<GameObject>> m_AfterUI;
    vector<shared_ptr<GameObject>> m_Decal;
    shared_ptr<GameObject> m_Terrain;
private:
    _bool                           m_bEffectToolMode_On = { false };

public:
	static _float4x4 S_View;
	static _float4x4 S_Proj;
};

template<typename T>
inline shared_ptr<T> Camera::Get_MainScript()
{
    for (auto& script : Get_Scripts())
    {
        if (typeid(*script.get()) == typeid(T))
            return static_pointer_cast<T>(script);
    }
    return nullptr;
}
