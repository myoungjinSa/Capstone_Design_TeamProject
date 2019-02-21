Texture2D gInput            : register(t0);
RWTexture2D<float4> gOutput : register(u0);

// RGB값에서 휘도(밝기)를 근사한다. 아래 코드에 쓰인 가중치들은
// 여러 파장의 빛에 대한 눈의 민감도에 기초한 실험으로 얻은 것이다.
float CalcLuminance(float3 color)
{
	return dot(color, float3(0.299f, 0.587f, 0.114f));
	
}

[numthreads(16,16,1)]
void SobelCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	//Sample the pixels in the neighborhood of this pixels.

	float4 c[3][3];

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int2 xy = dispatchThreadID.xy + int2(-1 + j, -1 + i);
			c[i][j] = gInput[xy];
		}
	}

	// For each color channel, estimate partial x derivative using Sobel scheme.
	// 각 색상 채널에 대해, 소벨 핵을 이용해서 X의 편미분계수를 추정한다.
	float4 Gx = -1.0f*c[0][0] - 2.0f*c[1][0] - 1.0f*c[2][0] + 1.0f*c[0][2] + 2.0f*c[1][2] + 1.0f*c[2][2];

	// For each color channel, estimate partial y derivative using Sobel scheme.
	float4 Gy = -1.0f*c[2][0] - 2.0f*c[2][1] - 1.0f*c[2][1] + 1.0f*c[0][0] + 2.0f*c[0][1] + 1.0f*c[0][2];

	// Gradient is (Gx, Gy).  For each color channel, compute magnitude to get maximum rate of change.
	// 이제 (Gx,Gy)가 이 픽셀의 기울기이다. 각 색상 채널에 대해, 변화율이 
	// 최대가 되는 크기를 구한다.
	float4 mag = sqrt(Gx*Gx + Gy * Gy);

	// Make edges black, and nonedges white.
// 윤곽선 픽셀은 검게, 윤곽선이 아닌 픽셀은 희게 만든다.
	mag = 1.0f - saturate(CalcLuminance(mag.rgb));

	gOutput[dispatchThreadID.xy] = mag;
}