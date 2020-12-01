#pragma once
#pragma once
#include <chrono>
#include <iostream>
#include <string>
class Timer
{
private:

	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<double> dt;
	double total_time = 0;
	std::string benchmark_location;
public:

	Timer()
	{
		start = std::chrono::high_resolution_clock::now();

	}

	Timer(const std::string& benchmark_location)
		:benchmark_location(benchmark_location)
	{}

	//custom start point
	void start_timer() {
		start = std::chrono::high_resolution_clock::now();
	}

	auto get_dt()
	{
		dt = end - start;
		return dt;
	}

	void print_dt()
	{
		if (!benchmark_location.empty()) {
			std::cout << "delta time: " << get_dt().count() << "from location: " << benchmark_location << std::endl;
		}
		else
		{
			std::cout << "delta time: " << get_dt().count() << std::endl;
		}
	}

	void end_timer()
	{
		end = std::chrono::high_resolution_clock::now();
	}

	void accumulate()
	{
		total_time += get_dt().count();
	}

	double get_total_time()
	{
		return total_time;
	}

	void print_total_time()
	{
		if (!benchmark_location.empty()) {
			std::cout << "total time: " << get_dt().count() << "from location: " << benchmark_location << std::endl;
		}
		else
		{
			std::cout << "total time: " << get_dt().count() << std::endl;
		}
	}

	~Timer()
	{
		end = std::chrono::high_resolution_clock::now();
	}




};