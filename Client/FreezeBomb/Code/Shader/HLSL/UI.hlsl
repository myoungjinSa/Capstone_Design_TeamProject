#include "Shaders.hlsl"

Texture2D UITexture			: register(t16);

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

VS_UI_OUTPUT VSOneSecUI(uint nVertexID : SV_VertexID)
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

VS_UI_OUTPUT VSTenSecUI(uint nVertexID : SV_VertexID)
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

VS_UI_OUTPUT VSOneMinUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(-0.5f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(-0.5f, 0.75f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(-0.25f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(-0.25f, 0.75f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(-0.25f, 1.f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(-0.5f, 1.f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

VS_UI_OUTPUT VSColonUI(uint nVertexID : SV_VertexID)
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

VS_UI_OUTPUT VSItemBoxUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(0.2f, -0.45f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(0.2f, -1.f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(1.f, -1.f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(1.f, -1.f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(1.f, -0.45f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(0.2f, -0.45f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

VS_UI_OUTPUT VSNormalItemUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(0.298f, -0.556f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(0.298f, -0.909f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(0.572f, -0.909f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(0.572f, -0.909f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(0.572f, -0.556f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(0.298f, -0.556f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

VS_UI_OUTPUT VSSpecialItemUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(0.635f, -0.552f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(0.635f, -0.907f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(0.907f, -0.907f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(0.907f, -0.907f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(0.907f, -0.552f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(0.635f, -0.552f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}

	return(output);
}

cbuffer cbProgressBar	: register(b8)
{
	float2 g_Position : packoffset(c0);
	//float g_X : packoffset(c0);
	//float g_Y : packoffset(c1);
};

VS_UI_OUTPUT VSProgressBarUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(-0.25f, 0.25f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(-0.25f, g_Position.y, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(g_Position.x, g_Position.y, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);

	}
	else if (nVertexID == 3)
	{
		output.position = float4(g_Position.x, g_Position.y, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(g_Position.x, 0.25f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(-0.25f, 0.25f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	return(output);
}

float4 PSUI(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = UITexture.Sample(gssWrap, input.uv);
	return(cColor);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Texture2D gtxtLoadingTexture : register(t20);
SamplerState gssLoading			: register(s2);

VS_UI_OUTPUT VSLoadingScene(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 0.0f);
	}
	if (nVertexID == 1)
	{
		output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f);
		output.uv = float2(1.0f, 0.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
		output.uv = float2(1.0f, 1.0f);	
	}
	if (nVertexID == 3)
	{
		output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 0.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
		output.uv = float2(1.0f, 1.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 1.0f);
	}
	return (output);
}

float4 PSLoadingScene(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtLoadingTexture.Sample(gssLoading, input.uv);
	return(cColor);
}

