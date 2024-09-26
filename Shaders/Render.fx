#ifndef _RENDER_FX_
#define _RENDER_FX_

#include "Global.fx"

#define MAX_MODEL_TRANSFORM 300
#define MAX_MODEL_INSTANCE 400
#define MAX_SRV_MAP_COUNT 16
// Mesh_Renderer

struct VTXMeshInstancing
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint instanceID : SV_INSTANCEID;
    row_major float4x4 world : INST;
};

// ModelRenderer, Animator

struct VTXModelInstancing
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHT;
    int filtered : TEXCOORD2;
    uint instanceID : SV_INSTANCEID;
    row_major float4x4 world : INST;
};

struct RenderParam
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
};

struct KeyFrameDesc
{
    int animIndex;
    uint currFrame;
    uint nextFrame;
    float ratio;
    float sumTime;
    float3 padding;
};

struct TweenFrameDesc
{
    float tweenDuration;
    float tweenRatio;
    float tweenSumTime;
    float padding;
    KeyFrameDesc curr;
    KeyFrameDesc next;
};

float g_brightness;
float g_contrast;
float g_saturation;

float g_max_white;

cbuffer RenderParamBuffer
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    float4 g_vec4_0;
    float4 g_vec4_1; 
    float4 g_vec4_2;
    float4 g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
};

cbuffer KeyFrameBuffer
{
    KeyFrameDesc keyframes;
};

cbuffer TweenBuffer
{
    TweenFrameDesc TweenFrames;
    TweenFrameDesc preTweenFrames;
};

bool g_SSAO_On;

cbuffer InstanceTweenBuffer
{
    TweenFrameDesc InstanceTweenFrames[MAX_MODEL_INSTANCE];
};

cbuffer InstanceRenderParamBuffer
{
    float4 InstanceRenderParams[MAX_MODEL_INSTANCE];
};

cbuffer BoneBuffer
{
    row_major float4x4 BoneTransform[MAX_MODEL_TRANSFORM];
};

cbuffer AnimAddonBuffer
{
    row_major float4x4 OffsetMatrix[MAX_MODEL_TRANSFORM];
    row_major float4x4 pivot;
};

cbuffer EffectBuffer
{
    float2 UVSliding;
    int maxGradationIndex;
    int gradationMapIndex;
    
    int maxSubGradationIndex;
    int SubGradationMapIndex;
    float2 uvSlidingAcc;
};

cbuffer ParticleBuffer
{
    int maxCount;
    int addCount;
    int frameNumber;
    int padding0;
    
    float2 DeltaAccTime;
    float2 MinMaxLifeTime;
    
    float2 MinMaxSpeed;
    float2 padding1;
};

cbuffer CreateParticleBuffer
{
    int g_iEndScaleOption;
    int g_iEndSpeedOption;

    float2 g_vMinMaxLifeTime;
	
    float2 g_vMinMaxRotationSpeed_X;
    float2 g_vMinMaxRotationSpeed_Y;
    float2 g_vMinMaxRotationSpeed_Z;

    float2 g_vMinMaxRotationAngle_X;
    float2 g_vMinMaxRotationAngle_Y;
    float2 g_vMinMaxRotationAngle_Z;

    float4 g_vStartColor;
    float4 g_vEndColor;
           
    float2 g_vMinMaxStartScale;
    float2 g_vMinMaxEndScale;
           
    float2 g_vMinMaxStartSpeed;
    float2 g_vMinMaxEndSpeed;
           
    float4 g_vCreateRange;
    float4 g_vCreateOffsets;

    int g_iNewlyAddCnt;
    int g_iRandomRotationOn;
    float2 padding_CreateParticleBuffer;
};

cbuffer TextureMapBuffer
{
    int bHasDiffuseMap;
    int bHasNormalMap;
    int bHasSpecularMap;
    int bHasOpacityMap;
    int bHasEmissiveMap;
    int bHasDissolveMap;
    int bHasDistortionMap;
    int bHasTexturemap7;
    int bHasTexturemap8;
    int bHasTexturemap9;
    int bHasTexturemap10;
    int bHasTexturemap11;
    int bHasTexturemap12;
    int bHasTexturemap13;
    int bHasTexturemap14;
    int bHasTexturemap15;
};

cbuffer SubMapBuffer
{
    int bHasSubmap0;
    int bHasSubmap1;
    int bHasSubmap2;
    int bHasSubmap3;
    int bHasSubmap4;
    int bHasSubmap5;
    int bHasSubmap6;
    int bHasSubmap7;
    int bHasSubmap8;
    int bHasSubmap9;
    int bHasSubmap10;
    int bHasSubmap11;
    int bHasSubmap12;
    int bHasSubmap13;
    int bHasSubmap14;
    int bHasSubmap15;
};

Texture2DArray TransformMap;

int parentIndex[MAX_MODEL_TRANSFORM];


uint BoneIndex;

float2 g_UVSliding;

row_major float4x4 GetAnimationMatrix(VTXModel input)
{
    float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
    float weights[4] = { input.blendWeight.x, input.blendWeight.y, input.blendWeight.z, input.blendWeight.w };
    if (input.blendIndices.x + input.blendIndices.y + input.blendIndices.z + input.blendIndices.w == 0.f)
    {
        indices[0] = BoneIndex;
        weights[0] = 1.f;
        
        for (int i = 1; i < 4; ++i)
        {
            indices[i] = 0;
            weights[i] = 0;

        }

    }
    {
        int animIndex[2] = { TweenFrames.curr.animIndex, TweenFrames.next.animIndex };
        int currFrame[2] = { TweenFrames.curr.currFrame, TweenFrames.next.currFrame };
        int nextFrame[2] = { TweenFrames.curr.nextFrame, TweenFrames.next.nextFrame };
        float ratio[2] = { TweenFrames.curr.ratio, TweenFrames.next.ratio };
    
        float4 c0, c1, c2, c3;
        float4 n0, n1, n2, n3;
        row_major float4x4 curr = 0;
        row_major float4x4 next = 0;
        row_major float4x4 transform = 0;
    
        for (int i = 0; i < 4; ++i)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
            curr = 
            float4x4(c0, c1, c2, c3);
    
            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
            next = 
             float4x4(n0, n1, n2, n3);
    
            row_major float4x4 result = slerpMat(curr, next, ratio[0]);
            //matrix result = curr;
            if (animIndex[1] >= 0)
            {
                c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
                c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
                c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
                c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
                curr = 
                float4x4(c0, c1, c2, c3);
    
                n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
                n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
                n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
                n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
                next = 
                float4x4(n0, n1, n2, n3);
    
                row_major float4x4 nextResult = slerpMat(curr, next, ratio[1]);
      
                result = slerpMat(result, nextResult, TweenFrames.tweenRatio);
              
            }
            result = mul(OffsetMatrix[indices[i]], result);
            result = mul(result, pivot);
                      
            transform += mul(weights[i], result);
    
        }
        return transform;
    }
}

row_major float4x4 GetPreAnimationMatrix(VTXModel input)
{
    float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
    float weights[4] = { input.blendWeight.x, input.blendWeight.y, input.blendWeight.z, input.blendWeight.w };
    if (input.blendIndices.x + input.blendIndices.y + input.blendIndices.z + input.blendIndices.w == 0.f)
    {
        indices[0] = BoneIndex;
        weights[0] = 1.f;
        
        for (int i = 1; i < 4; ++i)
        {
            indices[i] = 0;
            weights[i] = 0;

        }

    }
    {
        int animIndex[2] = { preTweenFrames.curr.animIndex, preTweenFrames.next.animIndex };
        int currFrame[2] = { preTweenFrames.curr.currFrame, preTweenFrames.next.currFrame };
        int nextFrame[2] = { preTweenFrames.curr.nextFrame, preTweenFrames.next.nextFrame };
        float ratio[2] = { preTweenFrames.curr.ratio, preTweenFrames.next.ratio };
    
        float4 c0, c1, c2, c3;
        float4 n0, n1, n2, n3;
        row_major float4x4 curr = 0;
        row_major float4x4 next = 0;
        row_major float4x4 transform = 0;
    
        for (int i = 0; i < 4; ++i)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
            curr = matrix(c0, c1, c2, c3);
    
            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
            next = matrix(n0, n1, n2, n3);
    
            row_major float4x4 result = slerpMat(curr, next, ratio[0]);
            //matrix result = curr;
            if (animIndex[1] >= 0)
            {
                c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
                c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
                c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
                c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
                curr = matrix(c0, c1, c2, c3);
    
                n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
                n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
                n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
                n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
                next = matrix(n0, n1, n2, n3);
    
                row_major float4x4 nextResult = slerpMat(curr, next, ratio[1]);
      
                result = slerpMat(result, nextResult, preTweenFrames.tweenRatio);
              
            }
            result = mul(OffsetMatrix[indices[i]], result);
            result = mul(result, pivot);
                      
            transform += mul(weights[i], result);
    
        }
        return transform;
    }
}


row_major float4x4 GetAnimationMatrix_Instance(VTXModelInstancing input)
{
    float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
    float weights[4] = { input.blendWeight.x, input.blendWeight.y, input.blendWeight.z, input.blendWeight.w };
    if (input.blendIndices.x + input.blendIndices.y + input.blendIndices.z + input.blendIndices.w == 0.f)
    {
        indices[0] = BoneIndex;
        weights[0] = 1.f;
        
        for (int i = 1; i < 4; ++i)
        {
            indices[i] = 0;
            weights[i] = 0;

        }

    }
    int animIndex[2] = { InstanceTweenFrames[input.instanceID].curr.animIndex, InstanceTweenFrames[input.instanceID].next.animIndex };
    int currFrame[2] = { InstanceTweenFrames[input.instanceID].curr.currFrame, InstanceTweenFrames[input.instanceID].next.currFrame };
    int nextFrame[2] = { InstanceTweenFrames[input.instanceID].curr.nextFrame, InstanceTweenFrames[input.instanceID].next.nextFrame };
    float ratio[2] = { InstanceTweenFrames[input.instanceID].curr.ratio, InstanceTweenFrames[input.instanceID].next.ratio };
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    row_major float4x4 curr = 0;
    row_major float4x4 next = 0;
    row_major float4x4 transform = 0;
    
    for (int i = 0; i < 4; ++i)
    {
        c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
        c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
        c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
        c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
        curr = matrix(c0, c1, c2, c3);
      
        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
        next = matrix(n0, n1, n2, n3);
      
        row_major float4x4 result = slerpMat(curr, next, ratio[0]);
      
        if (animIndex[1] >= 0)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
            curr = matrix(c0, c1, c2, c3);
      
            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            next = matrix(n0, n1, n2, n3);
      
            row_major float4x4 nextResult = slerpMat(curr, next, ratio[1]);
      

            result = slerpMat(result, nextResult, InstanceTweenFrames[input.instanceID].tweenRatio);

        }
        result = mul(OffsetMatrix[indices[i]], result);
        result = mul(result, pivot);
        transform += mul(weights[i], result);
    }
    
    return transform;
}

//row_major float4x4 GetPreAnimationMatrix_Instance(VTXModelInstancing input)
//{
//    float indices[4] = { input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w };
//    float weights[4] = { input.blendWeight.x, input.blendWeight.y, input.blendWeight.z, input.blendWeight.w };
//    if (input.blendIndices.x + input.blendIndices.y + input.blendIndices.z + input.blendIndices.w == 0.f)
//    {
//        indices[0] = BoneIndex;
//        weights[0] = 1.f;
        
//        for (int i = 1; i < 4; ++i)
//        {
//            indices[i] = 0;
//            weights[i] = 0;

//        }

//    }
//    int animIndex[2] = { InstancePreTweenFrames[input.instanceID].curr.animIndex, InstancePreTweenFrames[input.instanceID].next.animIndex };
//    int currFrame[2] = { InstancePreTweenFrames[input.instanceID].curr.currFrame, InstancePreTweenFrames[input.instanceID].next.currFrame };
//    int nextFrame[2] = { InstancePreTweenFrames[input.instanceID].curr.nextFrame, InstancePreTweenFrames[input.instanceID].next.nextFrame };
//    float ratio[2] = { InstancePreTweenFrames[input.instanceID].curr.ratio, InstancePreTweenFrames[input.instanceID].next.ratio };
    
//    float4 c0, c1, c2, c3;
//    float4 n0, n1, n2, n3;
//    row_major float4x4 curr = 0;
//    row_major float4x4 next = 0;
//    row_major float4x4 transform = 0;
    
//    for (int i = 0; i < 4; ++i)
//    {
//        c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
//        c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
//        c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
//        c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
//        curr = matrix(c0, c1, c2, c3);
      
//        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
//        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
//        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
//        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
//        next = matrix(n0, n1, n2, n3);
      
//        row_major float4x4 result = slerpMat(curr, next, ratio[0]);
      
//        if (animIndex[1] >= 0)
//        {
//            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
//            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
//            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
//            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
//            curr = matrix(c0, c1, c2, c3);
      
//            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
//            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
//            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
//            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
//            next = matrix(n0, n1, n2, n3);
      
//            row_major float4x4 nextResult = slerpMat(curr, next, ratio[1]);
      

//            result = slerpMat(result, nextResult, InstancePreTweenFrames[input.instanceID].tweenRatio);

//        }
//        result = mul(OffsetMatrix[indices[i]], result);
//        result = mul(result, pivot);
//        transform += mul(weights[i], result);
//    }
    
//    return transform;
//}

float luminance(float3 v)
{
    return dot(v, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 change_luminance(float3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
}

float3 reinhard_extended_luminance(float3 v, float max_white_l)
{
    float l_old = luminance(v);
    float numerator = l_old * (1.f + (l_old / (max_white_l * max_white_l)));
    float l_new = numerator / (1.f + l_old);
    return change_luminance(v, l_new);
}

float3 uncharted2_tonemap_partial(float3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float3 uncharted2_filmic(float3 v)
{
    float exposure_bias = 2.0f;
    float3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    float3 W = float3(11.2f, 11.2f, 11.2f);
    float3 white_scale = float3(1.0f, 1.0f, 1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

static const float3 aces_input_matrix[3] =
{
    float3(0.59719f, 0.35458f, 0.04823f),
    float3(0.07600f, 0.90834f, 0.01566f),
    float3(0.02840f, 0.13383f, 0.83777f)
};

static const
float3 aces_output_matrix[3] =
{
    float3(1.60475f, -0.53108f, -0.07367f),
    float3(-0.10208f, 1.10813f, -0.00605f),
    float3(-0.00327f, -0.07276f, 1.07602f)
};

float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 aces_fitted(float3 v)
{
    float x, y, z;
    x = dot(aces_input_matrix[0], v);
    y = dot(aces_input_matrix[1], v);
    z = dot(aces_input_matrix[2], v);
    v = float3(x, y, z);
    v = rtt_and_odt_fit(v);
    
    x = dot(aces_output_matrix[0], v);
    y = dot(aces_output_matrix[1], v);
    z = dot(aces_output_matrix[2], v);
    
    return float3(x,y,z);
}

static const row_major float4x4 T =
{
    0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
};


#endif