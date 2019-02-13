#include <iostream>
#include <fstream>
#include <string>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

#define SIZE 20
struct Bound
{
	string m_Name;
	XMFLOAT3 m_xmf3Center = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_xmf3Extent = XMFLOAT3(0.f, 0.f, 0.f);
};

void WriteToText(const string&, Bound*);
void ReadToText(const string&);
void WriteToBinary(const string&, Bound*);
void ReadToBinary(const string&);
int main()
{
	const string filename = "ModelBounds";

	Bound bounds[SIZE] =
	{
		// EvilBear
		"<EvilBear>", XMFLOAT3(-0.1304445, 0.003544204, - 7.450581E-09), XMFLOAT3(0.2756854, 0.1529771, 0.2030513),

		// DeadTree
		"<DeadTree01>", XMFLOAT3(0, 0.135704, 0), XMFLOAT3(0.4260397, 0.1520242, 0.5093679),
		"<DeadTree02>", XMFLOAT3(0, 0.1853436, 0), XMFLOAT3(0.3149319, 0.204001, 0.2920995),
		"<DeadTree03>", XMFLOAT3(0, 0.2153993, 0), XMFLOAT3(0.4829912, 0.2368244, 0.4896541),
		"<DeadTree04>", XMFLOAT3(0, 0.7272767, 0), XMFLOAT3(1.17994, 0.7715069, 1.204947),
		"<DeadTree05>", XMFLOAT3(0.5152383, 0.7651024, 0.1873603), XMFLOAT3(3.147523, 0.7651024, 0.7412322),
		// PineTree
		"<PineTree01>", XMFLOAT3(0.0353142, 3.199464, 0.03110158), XMFLOAT3(1.502276, 3.209207, 1.659834),
		"<PineTree02>", XMFLOAT3(-0.1440293, 3.268975, 0.2914116), XMFLOAT3(1.731676, 3.282075, 1.586664),
		"<PineTree03>", XMFLOAT3(-0.01337361, 3.161052, 0.05751443), XMFLOAT3(1.659016, 3.157176, 1.576497),
		"<PineTree04>", XMFLOAT3(-0.08885717, 2.359786, 0), XMFLOAT3(1.222467, 2.36444, 1.299297),
		"<PineTree05>", XMFLOAT3(0.006743312, 3.185632, 0), XMFLOAT3(1.192782, 3.181756, 1.318227),
		"<PineTree06>", XMFLOAT3(-0.1351846, 3.163321, 0), XMFLOAT3(1.731676, 3.192807, 1.586664),
		"<PineTree07>", XMFLOAT3(0.0759207, 2.3369, 1.192093E-07), XMFLOAT3(1.155055, 2.379681, 1.045298),
		"<PineTree08>", XMFLOAT3(-0.01955438, 2.838236, 0.2241595), XMFLOAT3(1.659016, 2.83436, 1.576497),
		// Rock
		"<Rock01>", XMFLOAT3(2.980232E-07, 1.216973, 0), XMFLOAT3(1.457033, 1.345459, 1.575962),
		"<Rock02>", XMFLOAT3(0, 1.307119, 0), XMFLOAT3(1.817355, 1.481039, 1.982507),
		"<Rock03>", XMFLOAT3(0, 1.780387, -1.788139E-07), XMFLOAT3(1.365763, 1.908676, 1.603257),
		"<Rock04>", XMFLOAT3(0, 1.442339, 1.788139E-07), XMFLOAT3(1.135531, 1.568365, 1.968186),
		"<Rock05>", XMFLOAT3(-4.172325E-07, 0.6365597, 0), XMFLOAT3(1.111851, 0.6991383, 0.9731598),
		// Deer
		"<Deer01>", XMFLOAT3(0.01083577, 1.601403, 0.5396753), XMFLOAT3(1.542158, 1.601403, 1.463584)
	};

	WriteToText(filename, bounds);
	ReadToText(filename);
	WriteToBinary(filename, bounds);
	ReadToBinary(filename);
}

void WriteToText(const string& filename, Bound* pBounds)
{
	ofstream out(filename + ".txt");
	for (int i = 0; i < SIZE; ++i)
	{
		size_t FrameNameSize = pBounds[i].m_Name.size();

		string BoundName = "<Bound>:";
		size_t BoundNameSize = BoundName.size();

		out << FrameNameSize << "\t" << pBounds[i].m_Name << endl;

		out << "\t" << BoundNameSize << "\t" << BoundName << "\t" 
			<< pBounds[i].m_xmf3Center.x << "\t" << pBounds[i].m_xmf3Center.y << "\t" << pBounds[i].m_xmf3Center.z << "\t" 
			<< pBounds[i].m_xmf3Extent.x << "\t" << pBounds[i].m_xmf3Extent.y << "\t" << pBounds[i].m_xmf3Extent.z << endl;
	}

	out.close();
}

void ReadToText(const string& filename)
{
	ifstream in(filename + ".txt");
	if (!in)
	{
		cout << filename + ".txt" << "- 텍스트 파일 없음" << endl;
		return;
	}
	Bound bound[SIZE];

	for (int i = 0; i < SIZE; ++i)
	{
		size_t FrameNameSize = 0;
		in >> FrameNameSize;

		char* p = new char[FrameNameSize + 1];
		in >> p;
		p[FrameNameSize] = '\0';
		bound[i].m_Name = p;
		delete[] p;

		size_t BoundNameSize = 0;
		in >> BoundNameSize;
		p = new char[BoundNameSize + 1];
		in >> p;
		p[BoundNameSize] = '\0';
		string BoundName = p;
		delete[] p;

		in >> bound[i].m_xmf3Center.x;
		in >> bound[i].m_xmf3Center.y;
		in >> bound[i].m_xmf3Center.z;
		in >> bound[i].m_xmf3Extent.x;
		in >> bound[i].m_xmf3Extent.y;
		in >> bound[i].m_xmf3Extent.z;

		cout << "텍스트 파일 : " 
			<< FrameNameSize << ", " << bound[i].m_Name << ", " 
			<< BoundNameSize << ", " << BoundName << ", " 
			<< bound[i].m_xmf3Center.x << ", " << bound[i].m_xmf3Center.y << ", " << bound[i].m_xmf3Center.z << ", " 
			<< bound[i].m_xmf3Extent.x << ", " << bound[i].m_xmf3Extent.y << ", " << bound[i].m_xmf3Extent.z << endl;
	}

	in.close();
}

void WriteToBinary(const string& filename, Bound* pBound)
{
	ofstream out(filename + ".bin", ios::binary);

	for (int i = 0; i < SIZE; ++i)
	{;
		size_t FrameNameSize = pBound[i].m_Name.size();
		out.write(reinterpret_cast<const char*>(&FrameNameSize), sizeof(size_t));
		out.write(pBound[i].m_Name.c_str(), sizeof(char) * FrameNameSize);

		string BoundName = "<Bound>:";
		size_t BoundNameSize = BoundName.size();
		out.write(reinterpret_cast<const char*>(&BoundNameSize), sizeof(size_t));
		out.write(BoundName.c_str(), sizeof(char) * BoundNameSize);

		out.write(reinterpret_cast<const char*>(&pBound[i].m_xmf3Center.x), sizeof(float));
		out.write(reinterpret_cast<const char*>(&pBound[i].m_xmf3Center.y), sizeof(float));
		out.write(reinterpret_cast<const char*>(&pBound[i].m_xmf3Center.z), sizeof(float));
		out.write(reinterpret_cast<const char*>(&pBound[i].m_xmf3Extent.x), sizeof(float));
		out.write(reinterpret_cast<const char*>(&pBound[i].m_xmf3Extent.y), sizeof(float));
		out.write(reinterpret_cast<const char*>(&pBound[i].m_xmf3Extent.z), sizeof(float));
	}
	out.close();
}

void ReadToBinary(const string& filename)
{
	ifstream in(filename + ".bin", ios::binary);
	if (!in)
	{
		cout << filename + ".bin" << " - 바이너리 파일 없음" << endl;
		return;
	}

	Bound bound[SIZE];

	for (int i = 0; i < SIZE; ++i)
	{
		size_t FrameNameSize = 0;
		in.read(reinterpret_cast<char*>(&FrameNameSize), sizeof(size_t));

		char* p = new char[FrameNameSize + 1];
		in.read(p, sizeof(char) * FrameNameSize);
		p[FrameNameSize] = '\0';
		bound[i].m_Name = p;
		delete[] p;

		size_t BoundNameSize = 0;
		in.read(reinterpret_cast<char*>(&BoundNameSize), sizeof(size_t));

		p = new char[BoundNameSize + 1];
		in.read(p, sizeof(char) * BoundNameSize);
		p[BoundNameSize] = '\0';
		string BoundName = p;
		delete[] p;

		in.read(reinterpret_cast<char*>(&bound[i].m_xmf3Center.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&bound[i].m_xmf3Center.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&bound[i].m_xmf3Center.z), sizeof(float));
		in.read(reinterpret_cast<char*>(&bound[i].m_xmf3Extent.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&bound[i].m_xmf3Extent.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&bound[i].m_xmf3Extent.z), sizeof(float));

		cout << "바이너리 파일 : " 
			<< FrameNameSize << ", " << bound[i].m_Name << ", " 
			<< BoundNameSize << ", " << BoundName << ", " 
			<< bound[i].m_xmf3Center.x << ", " << bound[i].m_xmf3Center.y << ", " << bound[i].m_xmf3Center.z << ", "
			<< bound[i].m_xmf3Extent.x << ", " << bound[i].m_xmf3Extent.y << ", " << bound[i].m_xmf3Extent.z << endl;
	}
	in.close();
}