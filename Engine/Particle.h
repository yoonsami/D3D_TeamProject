#pragma once
#include "Component.h"

struct ParticleInfo_UAV // ����: Shader_Particle2�� ParticleInfo_UAV�� �����ؾ���.
{
    /* �� �����Ӹ��� ���� �� �ִ� ���� */
    _float   fCurrCurrTime;      // ���ڰ� ���� �����ǰ������� �󸶳� �ð��� �귶����
    _float3  vCurrWorldPos;      // ������ ���� World pos 
    _float   fCurrSpeed;         // ������ ���� speed
    _float3  vCurrRotationSpeed; // ������ ���� ȸ���ӵ� 
    _float   fCurrSpeedExponent; // ������ ���� speed Exponent
    _float3  vCurrWorldDir;      // ������ ���� ���� ���� 
    _float4  vCurrColor;         // ������ ���� color 
    _float2  vCurrSize;          // ������ ���� size 
    _int     iCurrAlive;         // ���ڰ� ���� ����ִ°�. 

    /* ���ڰ� ó�� ������ �� �����Ǵ� ���� */
    _float   fLifeTime;          // ������ LifeTime 
    _float3  vRotationAngle;     // ������ ȸ������  

    /* �����Ӱ� ����� ������ ���� */
    _float   vOffsets;           // 16 byte���߱� ���� ����. ���������� �˾Ƽ� ���缭 �����. 
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
        string  strSelected_Texture_Dissolve = { "None" };
        string  strSelected_Texture_Option1 = { "None" };
        string  strSelected_Texture_Option2 = { "None" };

        // Diffuse Color 
        Color   vStartColor;
        Color   vEndColor;
        Color   vDestColor;         

        // Alpha gradation
        _float	fGradationByAlpha_Brighter = { 0.f };
        _float	fGradationByAlpha_Darker = { 0.f };
        _bool   bIsAlphaFollowDuration = { false };
        
        // Dissolve
        _int    iDissolveOption = { 0 };            // non, constant, curve
        _float2 fDissolveCurveSpeed = { 0.f, 0.f }; // (if option is curve) speed base, speed exponent

        // Duration ( Particle GameObject�� lifetime )
        _float  fDuration;

        // Create Interval
        _float  fCreateInterval;

        // Particle Count
        _int    iMinCnt = { 1 };
        _int    iMaxCnt = { 10 };
        _int    iMaxParticleNum = { 10 };
        
        // LifeTime ( Particle �ϳ��� lifetime )
        _int    iLifeTimeOption;        // rand, curve
        _float2 vLifeTime;              // (if option is rand) min, max / (if option is curve) base, exponent  

        // Speed
        _int    iSpeedOption;           // rand, curve
        _float2 vSpeed;                 // (if option is rand) min, max / (if option is curve)  base, exponent

        // Billbord
        _int    iBillbordOption;

        // Movement 
        _int    iMovementOption;
        _float4 vMovementOffsets;

        // IsLoop
        _bool   bIsLoop;

        // Create Particle Position 
        _float3	vCenterPosition = { 0.f, 0.f, 0.f };
        _float3	vCreateRange = { 1.f, 1.f, 1.f };
        _float4 vCreateOffsets = { 0.f, 0.f, 0.f, 0.f };   // ���� �� ���� �� ���� ������� ���� ��ġ�� ������ ��� ����� ����. (ex.������, ���� ��)

        // Scale
        _float2 vStartScale = { 1.f, 5.f };     // min, max
        _int    iScaleOption;                   // constant, curve
        _float2 vScaleSpeed;                    // (if option is constant) speed, no use / (if option is curve)  base, exponent

        // Rotation Speed, Angle 
        _float3 vRotationSpeed;         // x,y,z  
        _int    iRotationAngleOption;   // const, rand
        _float3 vRotationAngle;         // (if option is const) x,y,z/ (if option is rand) min, max  
    
    }DESC;

public:
    Particle(shared_ptr<Shader> shader);
    virtual ~Particle();

public:
    virtual HRESULT Init(void* tDesc);      
    virtual void    Tick() override;
    virtual void    Final_Tick() override;  // CS
    void            Render();               // VS, PS

    /* Setter */
    void            Set_Mesh(shared_ptr<Mesh> mesh) { m_pMesh = mesh; }
    void            Set_Material(shared_ptr<Material> material) { m_pMaterial = material; }

private:
    void            Init_ComputeParams();
    void            Init_RenderParams();
    void            Init_CreateParticleParams();

    void            Bind_BasicData_ToShader();
    void            Bind_ComputeParams_ToShader();
    void            Bind_RenderParams_ToShader();
    void            Bind_CreateParticleParams_ToShader();

private:
    DESC                    m_tDesc;
    
    TYPE                    m_eType = {TYPE_END};           // �� Particle�� standard mesh����, ������
    RENDERMESH_PASS         m_eRenderPass = { RP_Default }; // �� Particl�� ������ �� ����� pass (VS, PS)
    COMPUTE_PASS            m_eComputePass = { CP_Default };// �� Particl�� ���̴����� ������ �� ����� pass (CS)

    shared_ptr<Shader>      m_pShader = { nullptr };     // �� Particle�� Render���� ����� shader
    shared_ptr<Mesh>        m_pMesh = { nullptr };       // �� Particle�� Render���� �׸� Mesh
    shared_ptr<Model>       m_pModel = { nullptr };      // �� Particle�� Render���� �׸� Model
    
    shared_ptr<Material>    m_pMaterial = { nullptr };   // �� Particle�� ����ϴ� Textures, Colors�� Tick���� shader�� ���ε���. 

    RenderParams            m_ComputeParams{};           // Computer shader���� ����ϴ� �������� ���� ����ü (Bind_ComputeShaderData_ToShader()���� ���̴��� �ѱ��)
    RenderParams            m_RenderParams{};            // GS, VS, PS���� ����ϴ� �������� ���� ����ü (Bind_RenderShaderData_ToShader()���� ���̴��� �ѱ��)
    CreateParticleDesc      m_CreateParticleParams;     

    shared_ptr<StructuredBuffer> m_pParticleInfo_UAVBuffer = nullptr;   // ���̴��� �����͸� �ְ�޴� UAV (���� �ϳ��ϳ��� ���� ����)
    shared_ptr<StructuredBuffer> m_pComputeShared_UAVBuffer = nullptr;  // ���̴��� �����͸� �ְ�޴� UAV

    /* Time Acc */
    _float                  m_fCurrLifeTime = { 0.f };   // �� Particle�� ������ �� �帥 �ð� 
    _float                  m_fTimeAcc_CreatCoolTime = { 0.f };
};

