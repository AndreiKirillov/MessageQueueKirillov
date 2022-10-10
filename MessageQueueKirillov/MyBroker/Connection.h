#pragma once
#include "framework.h"
#include <thread>

class Connection
{
private:
	// поток в котором выполняется обработка подсоединения
	std::thread _implementation;

	// имя подключенного клиента
	std::string _client_id;
public:
	template <typename Func, typename ...Args>
	Connection(Func&& init_function, Args&&... args):_client_id()
	{
		_implementation = std::thread(std::forward<Func>(init_function), std::forward<Args>(args)...);
	}

	~Connection()
	{
		if (_implementation.joinable())
			_implementation.detach();
	}

	void setId(const std::string& id);
};

