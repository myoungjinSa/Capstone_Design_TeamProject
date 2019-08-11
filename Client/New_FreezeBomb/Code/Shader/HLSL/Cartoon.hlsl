Texture2D<float4> gBaseMap : register(t0);
Texture2D<float4> gEdgeMap : register(t1);
SamplerState gsSamplerToonState : register(s0);


struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD;
};

VS_TEXTURED_OUTPUT VSToonShader(uint nVertexID :SV_VertexID)
{
	VS_TEXTURED_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(-1.0f, 1.0f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(1.0f, 1.0f, 0.0f, 1.0f);
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

float4 PSToonShader(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cBaseTexture = gBaseMap.SampleLevel(gsSamplerToonState, input.uv, 0.0f);
	float4 cEdgeTexture = gEdgeMap.SampleLevel(gsSamplerToonState, input.uv, 0.0f);

	return cBaseTexture * cEdgeTexture;
}
