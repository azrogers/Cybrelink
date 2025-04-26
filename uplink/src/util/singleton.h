#pragma once

template <class T> class Singleton {
public:
	static T& instance()
	{
		static T _instance;

		return _instance;
	}
};