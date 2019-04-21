#pragma once

//추후에 SoundSystem을 싱글톤으로 변경해서 
//객체 하나만 생성하게 수정할 예정 - 명진

template<class T>
class Singleton
{
private:
	static T* _instance;

public:
	static T* GetInstance()
	{
		if (_instance == nullptr)
			_instance = new T();
		return _instance;
	}

	static void DeleteInstance()
	{
		if (_instance) 
			delete _instance;		
		
	}
};


