#pragma once
#include "Component.h"

class Model;



class ModelAnimator :
    public Component
{
public:
	enum PASS_INFO
	{
		PS_ANIM = 2,
		PS_ANIMINSTANCING = 3,

        PS_ANIM_SHADOW = 8,
        PS_ANIM_SHADOW_INSTANCING =9,

        PS_FORWARD = 19,

	};
public:
    ModelAnimator(shared_ptr<Shader> shader);
    ~ModelAnimator();

public:
    virtual void Tick() override;
    _bool Is_Finished() { return m_bFinished; }
    void Set_NextAnim(_int index);
    void Set_Model(shared_ptr<Model> model);
    void Set_Pass(_uint pass) { m_iPass = pass; }
    shared_ptr<Model> Get_Model() { return m_pModel; }
    shared_ptr<Shader> Get_Shader() { return m_pShader; }


    void Render();
	void Render_Instancing(shared_ptr<class InstancingBuffer>& buffer, shared_ptr<InstanceTweenDesc> desc, shared_ptr<InstanceRenderParamDesc> renderParamDesc);
   
    void Render_Shadow();
	void Render_Shadow_Instancing(shared_ptr<class InstancingBuffer>& buffer, shared_ptr<InstanceTweenDesc> tweenDesc, shared_ptr<InstanceRenderParamDesc> renderParamDesc );

	void Render_MotionBlur();

    void Render_Forward();

    InstanceID Get_InstanceID();
    TweenDesc& Get_TweenDesc() { return m_TweenDesc; }
    TweenDesc& Get_preTweenDesc() { return m_preTweenDesc; }

    void Set_CurrentAnim(_int index) {
        m_TweenDesc.curr = KeyFrameDesc();
        m_preTweenDesc.curr = KeyFrameDesc();
        m_TweenDesc.curr.animIndex = index;
        m_preTweenDesc.curr.animIndex = index;
        m_bFinished = false;
        m_TweenDesc.ClearNextAnim();
        m_preTweenDesc.ClearNextAnim();
    }

    _float4x4 Get_CurAnimTransform(_int boneIndex);

    void Set_NextTweenAnim(const wstring& animName, _float tweenDuration, _bool loop, _float animSpeed);
    void Set_CurrentAnim(_int animIndex, _bool loop, _float animSpeed);
    void Set_CurrentAnim(const wstring& animName, _bool loop, _float animSpeed);

    void Set_AnimationSpeed(_float animSpeed);

    _uint Get_FinalFrame(_int animIndex);
    void Reset_Frame() { m_TweenDesc.curr.currentFrame = 0; }

    void Set_AnimState(_bool bStop) { m_bAnimStop = bStop; }
    _bool Is_AnimStop() { return m_bAnimStop; }

    void Set_RenderState(_bool bRenderOn) { m_bRenderOn = bRenderOn; }
    _bool Is_RenderOn() { return m_bRenderOn; }
private:
    TweenDesc   m_TweenDesc;
    TweenDesc   m_preTweenDesc;
    _int m_iNextAnimation =-1;

private:
    shared_ptr<Shader> m_pShader;
    _uchar              m_iPass =0;
    shared_ptr<Model> m_pModel;

    _bool               m_bFinished = false;    

public:
	auto& Get_RenderParamDesc() { return m_RenderParams; }
	void SetInt(_uchar index, _int value) { m_RenderParams.SetInt(index, value); }
	void SetFloat(_uchar index, _float value) { m_RenderParams.SetFloat(index, value); }
	void SetVec2(_uchar index, _float2 value) { m_RenderParams.SetVec2(index, value); }
	void SetVec4(_uchar index, _float4 value) { m_RenderParams.SetVec4(index, value); }
	void SetMatrix(_uchar index, _float4x4& value) { m_RenderParams.SetMatrix(index, value); }

private:
	RenderParams m_RenderParams{};

    _bool m_bRenderOn = true;
    _bool m_bAnimStop = false;
};


