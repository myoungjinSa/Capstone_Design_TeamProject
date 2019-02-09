#include "Shaders.hlsl"

struct VS_UI_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_UI_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D gtxtUITexture			: register(t17);

VS_UI_OUTPUT VSNumberUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(0.25f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(0.25f, 0.75f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(0.5f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(0.5f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(0.5f, 1.f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(0.25f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

VS_UI_OUTPUT VSTenNumberUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(0.0f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(0.0f, 0.75f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(0.25f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(0.25f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(0.25f, 1.f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(0.0f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

VS_UI_OUTPUT VSHundredNumberUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(-0.25f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(-0.25f, 0.75f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(0.f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(0.f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(0.f, 1.f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(-0.25f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

float4 PSNumberUI(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtUITexture.Sample(gssWrap, input.uv);
	return(cColor);
}