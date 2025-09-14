#pragma once

#include <random>

class Random
{
public:
	// 생성 및 소멸을 막아 인스턴스화 할 수 없는 순수 정적 클래스로 만듭니다.
	Random() = delete;
	~Random() = delete;

	// [min, max] 범위의 정수를 반환
	static int GetInt(int min, int max)
	{
		// GetGenerator()를 통해 유일한 생성기 인스턴스를 사용합니다.
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(GetGenerator());
	}

	// [min, max] 범위의 실수를 반환
	static float GetFloat(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(GetGenerator());
	}

private:
	static std::mt19937& GetGenerator()
	{
		static std::mt19937 generator{ std::random_device{}() };
		return generator;
	}
};