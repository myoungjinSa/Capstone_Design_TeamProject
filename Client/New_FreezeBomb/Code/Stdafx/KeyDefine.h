#pragma once

#define KEY_DOWN(VK_CODE) ((GetAsyncKeyState(VK_CODE) & 0x8000 ? 1 : 0))
#define KEY_UP(VK_CODE) ((GetAsyncKeyState(VK_CODE) & 0x8000 ? 0 : 1))

// 키입력 정의
enum KEY_TYPE
{
	NONE				= 0x0000,
	UP					= 0x0001,
	DOWN				= 0x0002,
	RIGHT				= 0x0004,
	LEFT					= 0x0008,

	UP_RIGHT			= 0x0005,
	UP_LEFT			= 0x0009,
	DOWN_RIGHT	= 0x0006,
	DOWN_LEFT		= 0x000A
};

constexpr char VK_A = 0x41;
constexpr char VK_B = 0x42;
constexpr char VK_C = 0x43;
constexpr char VK_D = 0x44;
constexpr char VK_E = 0x45;
constexpr char VK_F = 0x46;
constexpr char VK_G = 0x47;
constexpr char VK_H = 0x48;
constexpr char VK_I = 0x49;
constexpr char VK_J = 0x4A;
constexpr char VK_K = 0x4B;
constexpr char VK_L = 0x4C;
constexpr char VK_M = 0x4D;
constexpr char VK_N = 0x4E;
constexpr char VK_O = 0x4F;
constexpr char VK_P = 0x50;
constexpr char VK_Q = 0x51;
constexpr char VK_R = 0x52;
constexpr char VK_S = 0x53;
constexpr char VK_T = 0x54;
constexpr char VK_U = 0x55;
constexpr char VK_V = 0x56;
constexpr char VK_W = 0x57;
constexpr char VK_X = 0x58;
constexpr char VK_Y = 0x59;
constexpr char VK_Z = 0x5A;

constexpr short DIGIT_0 = 0x30;
constexpr short DIGIT_1 = 0x31;
constexpr short DIGIT_2 = 0x32;
constexpr short DIGIT_3 = 0x33;
constexpr short DIGIT_4 = 0x34;
constexpr short DIGIT_5 = 0x35;
constexpr short DIGIT_6 = 0x36;
constexpr short DIGIT_7 = 0x37;
constexpr short DIGIT_8 = 0x38;
constexpr short DIGIT_9 = 0x39;

