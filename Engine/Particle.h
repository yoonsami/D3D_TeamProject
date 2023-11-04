#pragma once
#include "Component.h"

struct ParticleInfo_UAV // ����: Shader_Particle2�� ParticleInfo_UAV�� �����ؾ���.
{
    /* �� �����Ӹ��� ���� �� �ִ� ���� */
    _float3  vWorldPos;      // ������ ���� World pos 
    _float   fCurrTime;      // ���ڰ� �����ǰ������� �󸶳� �ð��� �귶����
    _float3  vWorldDir;      // ������ ���� ���� 
    _int     iAlive;         // ���ڰ� ���� ����ִ°�. 
    _float4  vColor;         // ������ color 
    _float   fSpeed;         // ������ speed
    _float   fSpeeeExponent; // ������ speed Exponent

    /* ���ڰ� ó�� ������ �� �����Ǵ� ���� */
    _float2  vSize;          // ������ size 
    _float   fLifeTime;      // ������ LifeTime 

    /* �����Ӱ� ����� ������ ���� */
    _float3  vOffsets;       // 16 byte���߱� ���� ����. ���������� �˾Ƽ� ���缭 �����. 
};  

struct ComputeShared_UAV // ����: Shader_Particle2�� ComputeShared_UAV�� �����ؾ���.
{
    int     addCount;
    _float3 padding;
};

class StructuredBuffer;
class Particle : public Component
{
public:
    enum TYPE   {TYPE_Mesh, TYPE_Model, TYPE_END};
    enum TECHNIQUE {TECHNIQUE_Compute, TECHNIQUE_MeshRender, TECHNIQUE_ModelRender};   // Shader_Particle2.fx�� technique
    enum COMPUTE_PASS { CP_Default, CP_END };
    enum RENDERMESH_PASS { RP_Default, RP_Full_Billbord, RP_Horizontal_Billbord, RP_Vertical_Billbord, RP_END };

    typedef struct tagParticle
    {
        // Textures
        string  strSelected_Texture_Shape = { "None" };
        string  m_strSelected_Texture_Dissolve = { "None" };
        string  m_strSelected_Texture_Distortion = { "None" };
        string  strSelected_Texture_Option1 = { "None" };
        string  strSelected_Texture_Option2 = { "None" };

        // Diffuse Color 
        _int    iSelected_ColorOption;
        Color   vConstantColor;
        Color   vStartColor;
        Color   vEndColor;

        // Coloring Option 
        _bool	bGradationByAlpha_Brighter = { false };
        _bool	bGradationByAlpha_Darker = { false };
        _bool   bDissolveOn = { false };
        _float  fDissolveSpeed = { 1.f };
        _bool   bDistortionOn = { false };
        _float  fDistortionOffset[2] = { 0.5f, 0.5f };

        // Duration ( �� Particle GameObject�� lifetime )
        _float  fDuration;

        // Create Interval
        _float  fCreateInterval;

        // Particle Count
        _int    iMinCnt = { 1 };
        _int    iMaxCnt = { 10 };
        _int    iMaxParticleNum = { 10 };

        // LifeTime ( Particle �ϳ��� lifetime )
        _int    iLifeTimeOption;
        _float2 vLifeTime;

        // Speed
        _int    iSpeedOption;
        _float2 vSpeed;

        // Billbord
        _int    iBillbordOption;

        // Movement 
        _int    iMovementOption;
        _float4 vMovementOffsets;

        // IsLoop
        _bool   bIsLoop;

        // Create Particle Position 
        _float	fCenterPosition[3] = { 0.f, 0.f, 0.f };
        _float	fCreateRange[3] = { 1.f, 1.f, 1.f };

        // Scale
        _float2 vScale = { 1.f, 5.f };

        // Rotation 
        _float3 fRotationSpeed = { 0.f, 0.f, 0.f };
        _float3 fRotationDir = { 1.f, 1.f, 1.f };
    }DESC;

public:
    Particle(shared_ptr<Shader> shader);
    virtual ~Particle();

public:
    virtual HRESULT Init(void* tDesc);      //�� �ε��ÿ� �߰��� ������Ʈ���� init�� �� init���� �ڵ����� �Ҹ���, �߰��� �߰��� �͵��� ���� ȣ���ؾߵ�. 
    virtual void    Tick() override;
    virtual void    Final_Tick() override;  // CS
    void            Render();               // VS, PS

    /* Setter */
    

    /* Getter */
    auto&           Get_ComputeParamDesc() { return m_ComputeParams; }
    auto&           Get_RenderParamDesc() { return m_RenderParams; }

private:
    void            Bind_BasicData_ToShader();
    void            Bind_ComputeShaderData_ToShader();
    void            Bind_RenderShaderData_ToShader();

private:
    DESC                    m_tDesc;
    
    TYPE                    m_eType = {TYPE_END};           // �� Particle�� standard mesh����, ������
    RENDERMESH_PASS         m_eRenderPass = { RP_Default }; // �� Particl�� ������ �� ����� pass (VS, PS)
    COMPUTE_PASS            m_eComputePass = { CP_Default };// �� Particl�� ���̴����� ������ �� ����� pass (CS)

    shared_ptr<Shader>      m_pShader = { nullptr };     // �� Particle�� Render���� ����� shader
    shared_ptr<Mesh>        m_pMesh = { nullptr };       // �� Particle�� Render���� �׸� Mesh
    shared_ptr<Model>       m_pModel = { nullptr };      // �� Particle�� Render���� �׸� Model
    shared_ptr<Material>    m_pMaterial = { nullptr };   // �� Particle�� ����ϴ� Textures, Colors�� Tick���� shader�� ���ε���. 

    RenderParams            m_ComputeParams{};      // Computer shader���� ����ϴ� �������� ���� ����ü (Render()���� ���̴��� �ѱ��)
    RenderParams            m_RenderParams{};       // VS, PS���� ����ϴ� �������� ���� ����ü (Render()���� ���̴��� �ѱ��)

    shared_ptr<StructuredBuffer> m_pParticleInfo_UAVBuffer = nullptr;   // ���̴��� �����͸� �ְ�޴� UAV (���� �ϳ��ϳ��� ���� ����)
    shared_ptr<StructuredBuffer> m_pComputeShared_UAVBuffer = nullptr;  // ���̴��� �����͸� �ְ�޴� UAV

    /* Time Acc */
    _float                  m_fCurrLifeTime = { 0.f };   // �� Particle�� ������ �� �帥 �ð� 
    _float                  m_fTimeAcc_CreatCoolTime = { 0.f };
};

