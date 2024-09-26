#include "Render.fx"
#include "Light.fx"

int g_SeedCount = 7;
int g_CirclesCount = 20;
float g_fCircleSize = 1.f / 20.f;
float3 col1 = float3(0.2f, 0.1f, 0.f);
float3 col2 = float3(0.9f,1.f,1.f);
float fadeRadius = 0.7f;
float overeallOpacity = 5.f;
int bokehIterations = 5;
float bokehRadius = 0.4;

const static float2 offset[25] =
{
    float2(-2, 2), float2(-1, 2), float2(0, 2), float2(1, 2), float2(2, 2),
    float2(-2, 1), float2(-1, 1), float2(0, 1), float2(1, 1), float2(2, 1),
    float2(-2, 0), float2(-1, 0), float2(0, 0), float2(1, 0), float2(2, 0),
    float2(-2, 1), float2(-1, 1), float2(0, 1), float2(1, 1), float2(2, 1),
    float2(-2, 2), float2(-1, 2), float2(0, 2), float2(1, 2), float2(2, 2),

};

float rnd(float2 p)
{
    float f = frac(sin(dot(p, float2(12.1234, 72.8392)) * 45123.2));
    return f;
}

float rnd(float w)
{
    float f = frac(sin(w) * 1000.);
    return f;
}

float regShape(float2 p, int N)
{
    float f;
    
    
    float a = atan2(p.x, p.y) + .2;
    float b = 6.28319 / float(N);
    f = smoothstep(.5, .51, cos(floor(.5 + a / b) * b - a) * length(p.xy));
    
    
    return f;
}

float3 circle(float2 p, float size, float decay, float3 color, float3 color2, float dist, float2 mouse)
{
 	 
    
    //l is used for making rings.I get the length and pass it through a sinwave
    //but I also use a pow function. pow function + sin function , from 0 and up, = a pulse, at least
    //if you return the max of that and 0.0.
    
    float l = length(p + mouse * (dist * 4.)) + size / 2.;
    
    //l2 is used in the rings as well...somehow...
    float l2 = length(p + mouse * (dist * 4.)) + size / 3.;
    
    ///these are circles, big, rings, and  tiny respectively
    float c = max(0.01 - pow(length(p + mouse * dist), size * 1.4), 0.0) * 50.;
    float c1 = max(0.001 - pow(l - 0.3, 1. / 40.) + sin(l * 30.), 0.0) * 3.;
    float c2 = max(0.04 / pow(length(p - mouse * dist / 2. + 0.09) * 1., 1.), 0.0) / 20.;
    float s = max(0.01 - pow(regShape(p * 5. + mouse * dist * 5. + 0.9, 6), 1.), 0.0) * 5.;
    
    color = 0.5 + 0.5 * sin(color);
    color = cos(float3(0.44, .24, .2) * 8. + dist * 4.) * 0.5 + .5;
    float3 f = c * color;
    f += c1 * color;
    
    f += c2 * color;
    f += s * color;
    return f - 0.01;
}

float sun(float2 p, float2 mouse)
{
    float f;
    
    float2 sunp = p + mouse;
    float sun = 1.0 - length(sunp) * 8.;
    return f;
}


float easeOutQuad(float x)
{
    return x * x;
}

float random(float seed)
{
    return frac(sin(seed) * 100000.0);
}

float getPointDist(float2 point1, float2 point2)
{
    float x = point2.x - point1.x;
    float y = point2.y - point1.y;
    return length(float2(x, y));
}

float3 hsv2rgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float3 drawRainbowCircle(float2 circlePos, float2 pixelPos, float radius)
{
    float dist = getPointDist(circlePos, pixelPos) / radius;
    if (dist > 1.)
        return float3(0, 0, 0);
    
    float colorMul = (dist - 0.4) / (dist + 1.);
    float3 col = hsv2rgb(float3(colorMul * 8., 0.5, 1));
    col *= colorMul;
    col = max(col, 0.);
    if (dist > 0.5)
        col *= 1. - dist;
    
    return col;
}

float3 drawCircle(float2 circlePos, float2 pixelPos, float radius, float falloff, float3 startColor, float3 endColor)
{
    float dist = getPointDist(circlePos, pixelPos) / radius;
    if (dist > 1.)
        return float3(0, 0, 0);
    
    float colorMul = (dist - falloff) / (dist + 1.);
    float3 col = lerp(startColor, endColor, colorMul);
    col *= colorMul;
    col = max(col, 0.);
    
    return col;
}

float2 getFlarePoint(float2 centerPos, float2 centerfloat, float position)
{
    float2 delta = centerfloat * position;
    delta += centerPos;
    return delta;
}

float angleBetween(float2 v1, float2 v2)
{
    return acos(dot(v1, v2) / (length(v1) * length(v2)));
}

#define GOLDEN_ANGLE 2.3999632
#define BLUR 1.

float2x2 rot = float2x2(cos(GOLDEN_ANGLE), sin(GOLDEN_ANGLE), -sin(GOLDEN_ANGLE), cos(GOLDEN_ANGLE));

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}
float2 g_LightPos;
float2 g_TestPos1;
float2 g_TestPos2;
float4 PS_Final(VS_OUT input) : SV_Target0
{
  
    float4 output = (float4) 0.f;

    float3 col = (float3) 0.f;
    
    uint width, height, numMips;
    SubMap1.GetDimensions(0, width, height, numMips);
    
    float2 vLightPos = g_LightPos;
    
    float2 vLightUV;
    vLightUV.x = (vLightPos.x / (float)width);
    vLightUV.y = (vLightPos.y / (float) height);
    
    float fLightDepth = SubMap1.Sample(PointSampler, vLightUV).r;
    float fDepth = SubMap1.Sample(PointSampler, input.uv).r;
    
    float2 centerPos = 2.f * g_LightPos - float2(960.f, 540.f);

    
    float2 centerVec = (vLightPos - centerPos);
    float centerVecLength = length(centerVec);
    
    float centerProximity = centerVecLength / 150.;
    if (centerProximity < 1.)
        centerProximity = 1.;
    
    float opacityMul = centerVecLength / 1920.f / fadeRadius;
    if (opacityMul > 1.)
        opacityMul = 1.;
    opacityMul = 1. - easeOutQuad(1. - opacityMul);
    
    float circleOpacityMul = centerVecLength / 1920.f / 0.2;
    if (circleOpacityMul > 1.)
        circleOpacityMul = 1.;
    
    float3 acc = float3(0,0,0);
    float3 div = acc;
    float r = 1.;
    float2 vangle = float2(0.0, bokehRadius * .01 / sqrt(float(bokehIterations)));
    
    for (int j = 0; j < bokehIterations; j++)
    {
        vangle = mul(rot, vangle);
        float2 pixelPos = (input.uv + (BLUR) * vangle) * float2(1920.f, 1080.f);
        
        float baseRadius = (1. - circleOpacityMul) / 4. + 3. / 4.;
        float circleBoost = 1. + (1. - circleOpacityMul) * 0.1;
        if (fDepth >= 1.f)
        col = drawCircle(getFlarePoint(vLightPos, centerVec, 0.), pixelPos, 1920.f / 1.5 * baseRadius, circleBoost, float3(0, 0, 0), float3(1, 1, 1));
    
        for (int i = 0; i < g_CirclesCount; i++)
        {
            float newSeed = float(g_SeedCount) / (float(i) + 10.);
            float pos = random(newSeed);
            float dist = 1. - pos;
            float radius = 1920.f * g_fCircleSize * random(newSeed * 2.) * dist;
            float falloff = 0.35 + random(newSeed * 3.) * 0.5;
            float opacity = (0.8 + random(newSeed * 4.) * 0.2) * opacityMul * overeallOpacity;
            opacity *= dist;
            
            float2 flarePoint = getFlarePoint(vLightPos, centerVec, pos);
            float2 pointToFlare = flarePoint - pixelPos;
            float angle = angleBetween(pointToFlare, centerVec) / PI;
            angle = easeOutQuad(angle);
            angle = clamp(angle, 0., 1.);
            opacity *= angle;
            opacity /= centerProximity;
            
            if (vLightPos.x >= 1920.f)
            {
                opacity *= saturate(1.f - (vLightPos.x - 1920.f) / 120.f);

            }
            else if (vLightPos.x <= 0.f)
            {
                opacity *= saturate(1 + vLightPos.x / 120.f);
            }
            if (vLightPos.y <= 0.f)
            {
                opacity *= saturate(1 + vLightPos.y / 120.f);
            }
            if (fLightDepth >= 0.999f)
            col += drawCircle(flarePoint, pixelPos, radius, falloff, col1, col2) * opacity;
        }
        
        float2 flarePoint = getFlarePoint(vLightPos, centerVec, 0.25);
        float2 pointToFlare = flarePoint - pixelPos;
        float angle = angleBetween(pointToFlare, centerVec) / PI;
        angle = easeOutQuad(easeOutQuad(angle));
        angle = clamp(angle, 0., 1.);
        angle = lerp(1., angle, opacityMul);
        float opacity = angle * 1.5;
        opacity /= centerProximity / 1.5;
        
        if (vLightPos.x >= 1920.f)
        {
            opacity *= saturate(1.f - (vLightPos.x - 1920.f) / 120.f);

        }
        else if (vLightPos.x <= 0.f)
        {
            opacity *= saturate(1 + vLightPos.x / 120.f);
        }
        if(vLightPos.y <= 0.f)
        {
            opacity *= saturate(1 + vLightPos.y / 120.f);
        }
        
        if (fLightDepth >= 1.f)
        col += drawRainbowCircle(flarePoint, pixelPos, 1920.f / 5.) * opacity;
        
        float2 distToCenter = pixelPos - vLightPos;
        float res = 1920.f / 80.;
        opacity = (1. - opacityMul) * 0.9 + 0.1;
        
        float dist = 1. - (abs(distToCenter.x / 16.) + abs(distToCenter.y * 4.)) / res;
        if (fDepth >= 1.f)
        col += max(dist, 0.) * 0.3 * opacity;
       
        dist = 1. - (abs(distToCenter.x * 4.) + abs(distToCenter.y / 16.)) / res;
        if (fDepth >= 1.f)
        col += max(dist, 0.) * 0.3 * opacity;
        
        res = 1920.f / 80.;
        
        dist = 1. - (abs(distToCenter.x / 9.) + abs(distToCenter.y)) / res;
        if (fDepth >= 1.f)
        col += max(dist, 0.) * 0.3 * opacity;
       
        dist = 1. - (abs(distToCenter.x) + abs(distToCenter.y / 9.)) / res;
        if (fDepth >= 1.f)
        col += max(dist, 0.) * 0.3 * opacity;
        
        
        float3 bokeh = pow(col, float3(4,4,4));
        acc += col * bokeh;
        div += bokeh;
    }
    float power = 1.f;
    if (vLightPos.x >= 1920.f )
    {
        power = saturate(1.f - (vLightPos.x - 1920.f) / 1920.f);
        
    }
    else if(vLightPos.x <= 0.f)
    {
        power = saturate(1 + vLightPos.x / 1920.f);
    }
    else if (vLightPos.y <= 0.f)
    {
        power *= saturate(1 + vLightPos.y / 120.f);
    }
        
    if (length(div) != 0.f)
        output += float4(acc / div, 0.f) * power;

  
    return output;
}

float2 g_LightUV;

float4 PS_Final2(VS_OUT input) : SV_Target
{
    uint width, height, numMips;
    SubMap1.GetDimensions(0, width, height, numMips);
    float2 vLightUV = g_LightUV;
    float fLightDepth;
    float fDepth;
    float4 vLightPos = SubMap1.Sample(PointSampler, vLightUV);
    float4 vViewPos = SubMap1.Sample(PointSampler, input.uv);
    
    if (vLightPos.w == 0.f)
        fLightDepth = 1.f;
    else
    {
        vLightPos = mul(vLightPos, P);
        fLightDepth = vLightPos.z / vLightPos.w;
    }
    
    if(vViewPos.w == 0.f)
        fDepth = 1.f;
    else
    {
        vViewPos = mul(vViewPos, P);
        fDepth = vViewPos.z / vViewPos.w;
    }
    
        
   
   
    
    
    float dU = 1.f / (float) width;
    float dV = 1.f / (float) height;
    
    float fLightPower = 0;
    for (int j = 0; j < 25;++j)
    {
        float4 vPos = SubMap1.Sample(PointSampler, vLightUV + offset[j] * float2(dU, dV) * 2);
        
        if(vPos.w == 0.f )
            fLightPower += 1;
    }
    fLightPower /= 25;
    
    float2 uv = input.uv;
    uv = uv * 2.0 - float2(1.0, 1.0);

    uv.x *= (float) width / (float) height;
    
    float2 mm = g_LightPos;
    
    float3 circColor = float3(0.9, 0.2, 0.1);
    float3 circColor2 = float3(0.3, 0.1, 0.9);
    
    //now to make the sky not black
    float3 color = 0.f;
    
    //this calls the function which adds three circle types every time through the loop based on parameters I
    //got by trying things out. rnd i*2000. and rnd i*20 are just to help randomize things more
    for (float i = 0.; i < 10.; i++)
    {
        //©Ь
        color += circle(uv, pow(rnd(i * 2000.) * 1.8, 2.) + 1.41, 0.0, circColor + i, circColor2 + i, rnd(i * 20.) * 3. + 0.2 - .5, mm) * fLightPower;
    }
    //get angle and length of the sun (uv - mouse)
    float a = atan2(uv.y - mm.y, uv.x - mm.x);
    float l = max(1.0 - length(uv - mm) - 0.84, 0.0);
    
    float bright = 0.1; //+0.1/abs(sin(iTime/3.))/3.;//add brightness based on how the sun moves so that it is brightest
    //when it is lined up with the center
    
    //add the sun with the frill things
    // еб╬Г╨Ш
    if (fDepth >= 1.f)
    color += max(0.1 / pow(length(uv - mm) * 5., 5.), 0.0) * abs(sin(a * 5. + cos(a * 9.))) / 20.;
    
    
    // ╨Ш line
    color += max(0.1 / pow(length(uv - mm) * 10., 1. / 20.), .0) + abs(sin(a * 3. + cos(a * 9.))) / 8. * (abs(sin(a * 9.))) / 1. * fLightPower;
    //add another sun in the middle (to make it brighter)  with the20color I want, and bright as the numerator.
    color += (max(bright / pow(length(uv - mm) * 4., 1. / 2.), 0.0) * 4.) * float3(0.2, 0.21, 0.3) * 4. * fLightPower;
       // * (0.5+.5*sin(float3(0.4, 0.2, 0.1) + float3(a*2., 00., a*3.)+1.3));
    	
    //multiply by the exponetial e^x ? of 1.0-length which kind of masks the brightness more so that
    //there is a sharper roll of of the light decay from the sun. 
    color *= exp(1.0 - length(uv - mm)) / 5.;
    return float4(color, 1.0);

}
float4 PS_Bloom(VS_OUT input) : SV_Target
{
    float4 output = (float4) 0.f;
    float4 vHDRColor = SubMap0.Sample(LinearSampler, input.uv);
    float4 vBloomOrigin = SubMap2.Sample(LinearSampler, input.uv);
    
    float4 vBloom =vBloomOrigin;
    
    output = vHDRColor;
    
    output = pow(abs(output), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);
    
    output += vBloom;
    
    return pow(abs(output), 1 / 2.2f);
}

technique11 T0
{
    Pass brightcontrast
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Final2()));
    }
    Pass apply
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Bloom()));
    }
};
