struct MATERIAL
{
	float4			m_cAmbient;
	float4			m_cDiffuse;		
	float4			m_cSpecular;	//a = power
	float4			m_cEmissive;

};

cbuffer cbCameraInfo						: register(b1)
{
	matrix			gmtxView					: packoffset(c0);
	matrix			gmtxProjection			: packoffset(c4);
	matrix			gmtxShadow			: packoffset(c8);
	float3			gvCameraPosition	: packoffset(c12);
};

cbuffer cbWorld								: register(b2)
{
	matrix			gmtxWorld				: packoffset(c0);
};

cbuffer cbMaterialInfo						: register(b3)
{
	MATERIAL	gMaterial					: packoffset(c0);
	uint				gnTexturesMask		: packoffset(c4);
};

#define LINEAR_FOG 1.0f
#define EXP_FOG 2.0f
#define EXP2_FOG 3.0f


cbuffer cbFog : register(b8)
{
	float4 gcFogColor;
	float4 gvFogParameter; //(mode,start,End,Density)
}

float4 Fog(float4 cColor, float3 vPosition)
{
	float3 vCameraPosition = gvCameraPosition.xyz;
	float3 vPositionToCamera = vCameraPosition - vPosition;
	float fDistanceToCamera = length(vPositionToCamera);
	float fFogFactor = 0.0f;
	if (gvFogParameter.x == LINEAR_FOG)
	{
		float fFogRange = gvFogParameter.z - gvFogParameter.y;
		fFogFactor = saturate((gvFogParameter.z - fDistanceToCamera) / fFogRange);

	}

	float4 cColorByFog = lerp(cColor, gcFogColor, 1 - fFogFactor);
	return(cColorByFog);
}



//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f*normalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}



#include "Light.hlsl"



SamplerState gssWrap		: register(s0);
SamplerState gssClamp	: register(s1);

TextureCube gtxtSkyCubeTexture : register(t3);

struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION0;
	float3  positionW : POSITION1;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.positionL = input.position;
	output.positionW = mul(input.position, (float3x3)gmtxWorld);
	return(output);
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);


	float4 cFog = Fog(cColor, input.positionW);
	
	return (lerp(cFog,cColor,0.3f));
	//return (cColor);
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture2D gtxtTerrainBaseTexture : register(t4);
Texture2D gtxtTerrainDetailTexture : register(t5);



struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	
};

struct VS_TERRAIN_OUTPUT
{
	float3 positionW : POSITION;
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.positionW = mul(input.position, (float3x3)gmtxWorld);
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;
	
	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gssWrap, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gssWrap, input.uv1);
	float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
	
	float4 cFog = Fog(cBaseTexColor, input.positionW);
	return (lerp(cFog,cBaseTexColor,0.7f));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define _WITH_VERTEX_LIGHTING
#define MATERIAL_ALBEDO_MAP				0x01
#define MATERIAL_SPECULAR_MAP				0x02
#define MATERIAL_NORMAL_MAP				0x04
#define MATERIAL_METALLIC_MAP				0x08
#define MATERIAL_EMISSION_MAP				0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D gtxtAlbedoTexture			: register(t6);
Texture2D gtxtSpecularTexture			: register(t7);
Texture2D gtxtNormalTexture			: register(t8);
Texture2D gtxtMetallicTexture			: register(t9);
Texture2D gtxtEmissionTexture			: register(t10);
Texture2D gtxtDetailAlbedoTexture	: register(t11);
Texture2D gtxtDetailNormalTexture	: register(t12);

struct VS_STANDARD_INPUT
{
	float3 position			: POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 bitangent		: BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position			: SV_POSITION;
	float3 positionW		: POSITION;
	float3 normalW		: NORMAL;
	float3 tangentW		: TANGENT;
	float3 bitangentW	: BITANGENT;
	float2 uv					: TEXCOORD;
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.tangentW = mul(input.tangent, (float3x3)gmtxWorld);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);

	if (cAlbedoColor.a < 0.3f) discard;

	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);

	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);

	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);

	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

	float3 normalW;
	float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
	if (gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
		normalW = normalize(mul(vNormal, TBN));
	}
	else
	{
		normalW = normalize(input.normalW);
	}
	float4 cIllumination = Lighting(input.positionW, normalW);

	float4 cFog = Fog(cColor, input.positionW);

	return(lerp(cFog,cIllumination,0.35f));
	//return(lerp(cColor, cIllumination, 0.5f));

	//cColor = float4(1, 1, 1, 1);
	//return cColor;
}

struct VS_SHADOW_INPUT
{
	float3 position			: POSITION;
};

struct VS_SHADOW_OUTPUT
{
	float4 position			: SV_POSITION;
	float3 positionW		: POSITION;
};

VS_SHADOW_OUTPUT VSShadow(VS_SHADOW_INPUT input)
{
	VS_SHADOW_OUTPUT output;

	// 모델좌표계의 점을 월드좌표계의 점으로 변환
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld);
	// 월드좌표계의 점을 그림자 행렬로 변환
	output.position = mul(mul(mul(float4(output.positionW, 1.0f), gmtxShadow), gmtxView), gmtxProjection);

	return output;
}

float4 PSShadow(VS_SHADOW_OUTPUT input) : SV_TARGET
{
	float4 cFog = Fog(float4(0.85f,0.85f,0.85f,1.0f),input.positionW);
	return(lerp(cFog,1.0f,0.1f));
	//return (float4(0.85f,0.85f,0.85f,1.0f));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct InstanceData
{
	matrix m_InstanceWorld;
};

//StructuredBuffer<InstanceData> g_InstanceFoliageData : register(t17);
//VS_STANDARD_OUTPUT VSFoliage(VS_STANDARD_INPUT input,uint nInstanceID: SV_InstanceID)
//{
//	VS_STANDARD_OUTPUT output;
//
//	//output.position = mul(mul(mul(float4(input.position, 1.0f), g_InstanceData[nInstanceID].m_InstanceWorld), gmtxView), gmtxProjection);
//	
//	output.positionW = mul(float4(input.position, 1.0f), g_InstanceFoliageData[nInstanceID].m_InstanceWorld).xyz;
//	output.normalW = mul(input.normal, (float3x3)g_InstanceFoliageData[nInstanceID].m_InstanceWorld);
//	output.tangentW = mul(input.tangent, (float3x3)g_InstanceFoliageData[nInstanceID].m_InstanceWorld);
//	output.bitangentW = mul(input.bitangent, (float3x3)g_InstanceFoliageData[nInstanceID].m_InstanceWorld);
//	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
//	output.uv = input.uv;
//
//	return(output);
//}
//

float4 PSFoliage(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

	float3 normalW;

	cSpecularColor = cSpecularColor + float4(0.1f, 0.3f, 0.1f, 1.0f);
	cEmissionColor = cEmissionColor + float4(0.05f, 0.6f, 0.1f, 1.0f);
	float4 cColor = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
	if (gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] → [-1, 1]
		normalW = normalize(mul(vNormal, TBN));
	}
	else
	{
		normalW = normalize(input.normalW);
	}
	float4 cIllumination = Lighting(input.positionW, normalW);
	return(lerp(cColor, cIllumination, 0.5f));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_VERTEX_INFLUENCES			4
//#define SKINNED_ANIMATION_BONES	128
#define SKINNED_ANIMATION_BONES	72

cbuffer cbBoneOffsets : register(b5)
{
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b6)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	uint4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = float3(0.0f, 0.0f, 0.0f);
	output.normalW = float3(0.0f, 0.0f, 0.0f);
	output.tangentW = float3(0.0f, 0.0f, 0.0f);
	output.bitangentW = float3(0.0f, 0.0f, 0.0f);
	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		output.positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
		output.normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld);
		output.tangentW += input.weights[i] * mul(input.tangent, (float3x3)mtxVertexToBoneWorld);
		output.bitangentW += input.weights[i] * mul(input.bitangent, (float3x3)mtxVertexToBoneWorld);
	}

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

struct VS_ANIMATION_SHADOW_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	uint4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_SHADOW_OUTPUT VSAnimationShadow(VS_ANIMATION_SHADOW_INPUT input)
{
	VS_SHADOW_OUTPUT output;

	output.positionW = float3(0.0f, 0.0f, 0.0f);

	matrix mtxVertexToBoneWorld;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
		output.positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	}

	// 월드좌표계의 점을 그림자 행렬로 변환
	output.position = mul(mul(mul(float4(output.positionW, 1.0f), gmtxShadow), gmtxView), gmtxProjection);

	return output;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
cbuffer cbAnimationClip	: register(b7)
{
	uint	gAnimationClip	: packoffset(c0);
};

Texture2D FireTexture : register (t13);

struct VS_LAMPPARTICLE_INPUT
{
	float3 position	: POSITION;
	float2 uv			: TEXCOORD;
};

struct VS_LAMPPARTICLE_OUTPUT
{
	float4 position	: SV_POSITION;
	float2 uv			: TEXCOORD;
};

VS_LAMPPARTICLE_OUTPUT VSLampParticle(VS_LAMPPARTICLE_INPUT input)
{
	VS_LAMPPARTICLE_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return (output);
}

float4 PSLampParticle(VS_LAMPPARTICLE_OUTPUT input) : SV_TARGET
{
	float2 texcoord = input.uv;
	texcoord.x = texcoord.x / 6.0f + (1.0f / 6.0f) * gAnimationClip;
	float4 cColor = FireTexture.Sample(gssWrap, texcoord);

	return (cColor);
}

struct VS_BOMBPARTICLE_INPUT
{
	float3 position	: POSITION;
	float2 uv			: TEXCOORD;
};

struct VS_BOMBPARTICLE_OUTPUT
{
	float4 position	: SV_POSITION;
	float2 uv			: TEXCOORD;
};

VS_BOMBPARTICLE_OUTPUT VSBombParticle(VS_BOMBPARTICLE_INPUT input)
{
	VS_LAMPPARTICLE_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return (output);
}

float4 PSBombParticle(VS_BOMBPARTICLE_OUTPUT input) : SV_TARGET
{
	float2 texcoord = input.uv;
	texcoord.x = texcoord.x / 6.0f + (1.0f / 6.0f) * gAnimationClip;
	float4 cColor = FireTexture.Sample(gssWrap, texcoord);

	return (cColor);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StructuredBuffer<InstanceData> g_InstanceCubeData : register(t1);
Texture2D IceTexture : register(t14);

struct VS_CUBEPARTICLE_INPUT
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;

};

struct VS_CUBEPARTICLE_OUTPUT
{
	float4 position :SV_POSITION;
	float2 uv		:TEXCOORD;
};

VS_CUBEPARTICLE_OUTPUT VSCubeParticle(VS_CUBEPARTICLE_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_CUBEPARTICLE_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), g_InstanceCubeData[nInstanceID].m_InstanceWorld), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSCubeParticle(VS_CUBEPARTICLE_OUTPUT input) : SV_Target
{
	float4 cColor = IceTexture.Sample(gssWrap, input.uv);

	return(cColor);
}

VS_CUBEPARTICLE_OUTPUT VSCubeParticleShadow(VS_CUBEPARTICLE_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_CUBEPARTICLE_OUTPUT output;

	float4 position = mul(float4(input.position, 1.0f), g_InstanceCubeData[nInstanceID].m_InstanceWorld);

	output.position = mul(mul(mul(position, gmtxShadow), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSCubeParticleShadow(VS_CUBEPARTICLE_OUTPUT input) : SV_Target
{
	return(float4(0.7f, 0.7f, 0.7f, 1.f));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StructuredBuffer<InstanceData> g_InstanceData : register(t2);
Texture2D SnowTexture : register (t15);

struct VS_SNOW_INPUT
{
	float3 position	: POSITION;
	float2 uv			: TEXCOORD;
};

struct VS_SNOW_OUTPUT
{
	float4 position	: SV_POSITION;
	float2 uv			: TEXCOORD;
};

VS_SNOW_OUTPUT VSSnow(VS_SNOW_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_SNOW_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), g_InstanceData[nInstanceID].m_InstanceWorld), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSSnow(VS_SNOW_OUTPUT input) : SV_TARGET
{
	float4 cColor = SnowTexture.Sample(gssWrap, input.uv);

	return(cColor);
}