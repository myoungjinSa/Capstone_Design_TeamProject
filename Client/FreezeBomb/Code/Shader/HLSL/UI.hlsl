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

VS_UI_OUTPUT VSItemBoxUI(VS_UI_INPUT input,uint nVertexID : SV_VertexID)
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
		output.position = float4(0.298f, -0.556f, 0.f, 1.f); //float4(0.635f, -0.552f, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(0.298f, -0.909f, 0.f, 1.f);//float4(0.635f, -0.907f, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(0.572f, -0.909f, 0.f, 1.f);//float4(0.907f, -0.907f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 3)
	{
		output.position = float4(0.572f, -0.909f, 0.f, 1.f);//float4(0.907f, -0.907f, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(0.572f, -0.556f, 0.f, 1.f);//float4(0.907f, -0.552f, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(0.298f, -0.556f, 0.f, 1.f);//float4(0.635f, -0.552f, 0.f, 1.f);
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
		output.position =float4(0.907f, -0.907f, 0.f, 1.f);
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

//VS_UI_OUTPUT VSSubSpecialItemUI(uint nVertexID : SV_VertexID)
//{
//	VS_UI_OUTPUT output;
//
//	if (nVertexID == 0)
//	{
//		output.position = float4(0.222f, -0.550f, 0.f, 1.f);// float4(0.298f, -0.556f, 0.f, 1.f);
//		output.uv = float2(0.f, 0.f);
//	}
//	else if (nVertexID == 1)
//	{
//		output.position = float4(0.222f, -0.720f, 0.f, 1.f);//float4(0.298f, -0.909f, 0.f, 1.f);
//		output.uv = float2(0.f, 1.f);
//	}
//	else if (nVertexID == 2)
//	{
//		output.position = float4(0.339f, -0.720f, 0.f, 1.f);// float4(0.572f, -0.909f, 0.f, 1.f);
//		output.uv = float2(1.f, 1.f);
//	}
//	else if (nVertexID == 3)
//	{
//		output.position = float4(0.339f, -0.720f, 0.f, 1.f);//float4(0.572f, -0.909f, 0.f, 1.f);
//		output.uv = float2(1.f, 1.f);
//	}
//	else if (nVertexID == 4)
//	{
//		output.position = float4(0.339f, -0.550f, 0.f, 1.f);//float4(0.572f, -0.556f, 0.f, 1.f);
//		output.uv = float2(1.f, 0.f);
//	}
//	else if (nVertexID == 5)
//	{
//		output.position = float4(0.222f, -0.550f, 0.f, 1.f);//float4(0.298f, -0.556f, 0.f, 1.f);
//		output.uv = float2(0.f, 0.f);
//	}
//
//	return(output);
//}

cbuffer cbMenuUV					: register(b9)
{
	float4 m_MenuBoard_MinMaxPos				: packoffset(c0);
	float4 m_MenuICON_MinMaxPos					: packoffset(c1);
	float4 m_MenuBoard_MenuICON_UV			: packoffset(c2);

	float4 m_MenuOption_MinMaxPos				: packoffset(c3);
	float4 m_MenuGameOver_MinMaxPos			: packoffset(c4);
	float4 m_MenuOption_MenuGameOver_UV	: packoffset(c5);

	float4 m_MenuSound_MinMaxPos				: packoffset(c6);
	float4 m_MenuCartoon_MinMaxPos				: packoffset(c7);
	float4 m_MenuSound_MenuCartoon_UV		: packoffset(c8);
};

VS_UI_OUTPUT VSMenuBoardUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_MenuBoard_MinMaxPos.x, m_MenuBoard_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.x, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_MenuBoard_MinMaxPos.x, m_MenuBoard_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.x, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_MenuBoard_MinMaxPos.z, m_MenuBoard_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.y, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_MenuBoard_MinMaxPos.z, m_MenuBoard_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.y, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_MenuBoard_MinMaxPos.z, m_MenuBoard_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.y, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_MenuBoard_MinMaxPos.x, m_MenuBoard_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.x, 0.0f);
	}
	return (output);
}

VS_UI_OUTPUT VSMenuICONUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_MenuICON_MinMaxPos.x, m_MenuICON_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.z, 0);
	}
	if (nVertexID == 1)
	{
		output.position = float4(m_MenuICON_MinMaxPos.x, m_MenuICON_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.z, 1);

	}
	if (nVertexID == 2)
	{
		output.position = float4(m_MenuICON_MinMaxPos.z, m_MenuICON_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.w, 1);

	}
	if (nVertexID == 3)
	{
		output.position = float4(m_MenuICON_MinMaxPos.z, m_MenuICON_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.w, 1);

	}
	if (nVertexID == 4)
	{
		output.position = float4(m_MenuICON_MinMaxPos.z, m_MenuICON_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.w, 0);

	}
	if (nVertexID == 5)
	{
		output.position = float4(m_MenuICON_MinMaxPos.x, m_MenuICON_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuBoard_MenuICON_UV.z, 0.f);

	}
	return (output);
}

VS_UI_OUTPUT VSMenuOptionUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_MenuOption_MinMaxPos.x, m_MenuOption_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.x, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_MenuOption_MinMaxPos.x, m_MenuOption_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.x, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_MenuOption_MinMaxPos.z, m_MenuOption_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.y, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_MenuOption_MinMaxPos.z, m_MenuOption_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.y, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_MenuOption_MinMaxPos.z, m_MenuOption_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.y, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_MenuOption_MinMaxPos.x, m_MenuOption_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.x, 0.0f);
	}
	return (output);
}

VS_UI_OUTPUT VSMenuGameOverUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_MenuGameOver_MinMaxPos.x, m_MenuGameOver_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.z, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_MenuGameOver_MinMaxPos.x, m_MenuGameOver_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.z, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_MenuGameOver_MinMaxPos.z, m_MenuGameOver_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.w, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_MenuGameOver_MinMaxPos.z, m_MenuGameOver_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.w, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_MenuGameOver_MinMaxPos.z, m_MenuGameOver_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.w, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_MenuGameOver_MinMaxPos.x, m_MenuGameOver_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuOption_MenuGameOver_UV.z, 0.0f);
	}
	return (output);
}

VS_UI_OUTPUT VSMenuSoundUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_MenuSound_MinMaxPos.x, m_MenuSound_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.x, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_MenuSound_MinMaxPos.x, m_MenuSound_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.x, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_MenuSound_MinMaxPos.z, m_MenuSound_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.y, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_MenuSound_MinMaxPos.z, m_MenuSound_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.y, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_MenuSound_MinMaxPos.z, m_MenuSound_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.y, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_MenuSound_MinMaxPos.x, m_MenuSound_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.x, 0.0f);
	}
	return (output);
}

VS_UI_OUTPUT VSMenuCartoonUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_MenuCartoon_MinMaxPos.x, m_MenuCartoon_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.z, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_MenuCartoon_MinMaxPos.x, m_MenuCartoon_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.z, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_MenuCartoon_MinMaxPos.z, m_MenuCartoon_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.w, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_MenuCartoon_MinMaxPos.z, m_MenuCartoon_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.w, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_MenuCartoon_MinMaxPos.z, m_MenuCartoon_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.w, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_MenuCartoon_MinMaxPos.x, m_MenuCartoon_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_MenuSound_MenuCartoon_UV.z, 0.0f);
	}
	return (output);
}

VS_UI_OUTPUT VS_OutCome(uint nVertexID: SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(-1.f, 1.f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 0.0f);
	}
	if (nVertexID == 1)
	{
		output.position = float4(-1.f, -1.f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(1.f, -1.f, 0.0f, 1.0f);
		output.uv = float2(1.0f, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(1.f, -1.f, 0.0f, 1.0f);
		output.uv = float2(1.0f, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(1.f, 1.f, 0.0f, 1.0f);
		output.uv = float2(1.0f, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(-1.f, 1.f, 0.0f, 1.0f);
		output.uv = float2(0.0f, 0.0f);
	}
	return (output);

}

float4 PSUI(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 Color = UITexture.Sample(gssWrap, input.uv);
	float4 temp = float4(1, 1, 0, 1);
	//Color = Color * temp;

	return Color;
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
///////////////////////////////////////////////////////////////////////////////////////////////////
cbuffer cbProgressBar	: register(b8)
{
	float4 g_Position : packoffset(c0);
};

VS_UI_OUTPUT VSProgressBarUI(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(g_Position.x, g_Position.y, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	else if (nVertexID == 1)
	{
		output.position = float4(g_Position.x, g_Position.w, 0.f, 1.f);
		output.uv = float2(0.f, 1.f);
	}
	else if (nVertexID == 2)
	{
		output.position = float4(g_Position.z, g_Position.w, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);

	}
	else if (nVertexID == 3)
	{
		output.position = float4(g_Position.z, g_Position.w, 0.f, 1.f);
		output.uv = float2(1.f, 1.f);
	}
	else if (nVertexID == 4)
	{
		output.position = float4(g_Position.z, g_Position.y, 0.f, 1.f);
		output.uv = float2(1.f, 0.f);
	}
	else if (nVertexID == 5)
	{
		output.position = float4(g_Position.x, g_Position.y, 0.f, 1.f);
		output.uv = float2(0.f, 0.f);
	}
	return(output);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//LOGIN
Texture2D gtxtLoginTexture : register(t21);
SamplerState gssLogin			: register(s3);

VS_UI_OUTPUT VSIPScene(uint nVertexID : SV_VertexID)
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

float4 PSIPScene(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtLoginTexture.Sample(gssLogin, input.uv);
	return(cColor);
}

VS_UI_OUTPUT VSIDScene(uint nVertexID : SV_VertexID)
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

float4 PSIDScene(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtLoginTexture.Sample(gssLogin, input.uv);
	return(cColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////

cbuffer cbLobbyInfo							: register(b10)
{
	float4 m_Ready_MinMaxPos			: packoffset(c0);
	float4 m_Quit_MinMaxPos			: packoffset(c1);
	float4 m_ReadyAndQuitUV			: packoffset(c2);

	//float4 m_MenuOption_MinMaxPos				: packoffset(c3);
	//float4 m_MenuGameOver_MinMaxPos			: packoffset(c4);
	//float4 m_MenuOption_MenuGameOver_UV	: packoffset(c5);

	//float4 m_MenuSound_MinMaxPos				: packoffset(c6);
	//float4 m_MenuCartoon_MinMaxPos				: packoffset(c7);
	//float4 m_MenuSound_MenuCartoon_UV		: packoffset(c8);
};

//CharacterSelect
Texture2D gtxtSelectTexture : register(t21);
VS_UI_OUTPUT VSLobbyBase(uint nVertexID : SV_VertexID)
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

VS_UI_OUTPUT VSLobbyReady(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_Ready_MinMaxPos.x, m_Ready_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.x, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_Ready_MinMaxPos.x, m_Ready_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.x, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_Ready_MinMaxPos.z, m_Ready_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.y, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_Ready_MinMaxPos.z, m_Ready_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.y, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_Ready_MinMaxPos.z, m_Ready_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.y, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_Ready_MinMaxPos.x, m_Ready_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.x, 0.0f);
	}

	return output;
}

VS_UI_OUTPUT VSLobbyQuit(uint nVertexID : SV_VertexID)
{
	VS_UI_OUTPUT output;

	if (nVertexID == 0)
	{
		output.position = float4(m_Quit_MinMaxPos.x, m_Quit_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.z, 0.0f);

	}
	if (nVertexID == 1)
	{
		output.position = float4(m_Quit_MinMaxPos.x, m_Quit_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.z, 1.0f);
	}
	if (nVertexID == 2)
	{
		output.position = float4(m_Quit_MinMaxPos.z, m_Quit_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.w, 1.0f);
	}
	if (nVertexID == 3)
	{
		output.position = float4(m_Quit_MinMaxPos.z, m_Quit_MinMaxPos.y, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.w, 1.0f);
	}
	if (nVertexID == 4)
	{
		output.position = float4(m_Quit_MinMaxPos.z, m_Quit_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.w, 0.0f);
	}
	if (nVertexID == 5)
	{
		output.position = float4(m_Quit_MinMaxPos.x, m_Quit_MinMaxPos.w, 0.0f, 1.0f);
		output.uv = float2(m_ReadyAndQuitUV.z, 0.0f);
	}
	return (output);
}

float4 PSCharacterSelect(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSelectTexture.Sample(gssLoading, input.uv);
	return(cColor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Lobby
VS_UI_OUTPUT VSLobby(uint nVertexID : SV_VertexID)
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


float4 PSLobby(VS_UI_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSelectTexture.Sample(gssLoading, input.uv);
	return(cColor);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

