#pragma once
#include "../Singleton/Singleton.h"

struct ImageInfo
{
	ImageInfo(wstring path, D2D1_RECT_F& pos, int width, int height, int totalX, int totalY, int frameX, int frameY) 
		: m_FilePath(path), m_Bitmap(nullptr), m_Pos(pos), m_WidthPixel(width), m_HeightPixel(height), m_TotalFrameX(totalX), m_TotalFrameY(totalY), m_FrameXNum(frameX), m_FrameYNum(frameY) {}

	wstring m_FilePath;
	ID2D1Bitmap* m_Bitmap;
	D2D1_RECT_F m_Pos;

	int m_WidthPixel = 0;
	int m_HeightPixel = 0;

	// 총 가로 몇프레임
	int m_TotalFrameX = 0;
	// 총 세로 몇프레임
	int m_TotalFrameY = 0;

	// 현재 프레임
	int m_FrameXNum = 0;
	int m_FrameYNum = 0;
};

class CDirect2D : public Singleton<CDirect2D>
{
public:
	void CreateBitmapImage(ImageInfo info, string key);
	void Release();
	void Render();
	void Render(string key);
	void Render(string key, D2D1_RECT_F pos, int fx, int fy);

	ImageInfo& GetImageInfo(string key);

private:
	unordered_map<string, ImageInfo> m_ImageInfoMap;
};