#pragma once
#include <mutex>
#include <queue>
#include <memory>

template <typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mtx;
	std::queue<T> _data_queue;

public:
	threadsafe_queue() :_data_queue()
	{}
	threadsafe_queue(const threadsafe_queue& other)
	{
		std::lock_guard<std::mutex> lck(other.mtx);
		_data_queue = other._data_queue;
	}

	void push(const T& object)
	{
		std::lock_guard<std::mutex> lck(mtx);
		_data_queue.push(object);
	}
	void push(T&& object)
	{
		std::lock_guard<std::mutex> lck(mtx);
		_data_queue.push(std::move(object));
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lck(mtx);
		if (_data_queue.empty())
			return nullptr;
		auto res = std::make_shared<T>(_data_queue.front());
		_data_queue.pop();
		return res;
	}

	int size() const
	{
		std::lock_guard<std::mutex> lck(mtx);
		return _data_queue.size();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lck(mtx);
		return _data_queue.empty();
	}

};

