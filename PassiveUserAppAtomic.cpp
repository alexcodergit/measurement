#include<iostream>
#include<chrono>
#include<vector>
#include<thread>
#include <mutex>
#include <utility>
#include <atomic>
using namespace std;

const unsigned MEASURE_CYCLES = 10000;
const unsigned INNER_LOOPS = 10;
unsigned long long USER_APP_CYCLES = 0;
unsigned long long TOTAL_SUM = 0;
unsigned long long MEASURE_SUM = 0;
const unsigned VARIABLES = 10;
vector<uint32_t>measured(VARIABLES, 0);
vector<std::atomic<uint32_t*>> addresses(VARIABLES);
const uint32_t MAX_VALUE = numeric_limits<uint32_t>::max();
vector<uint32_t> dummies(VARIABLES, MAX_VALUE);
volatile bool runActivity = true;

uint32_t getFib(uint32_t x)
{
	if (x < 2)
		return x;
	uint32_t fib1 = 0;
	uint32_t fib2 = 1;
	uint32_t fib3 = 1;
	uint32_t count = 2;
	while (count < x)
	{
		fib1 = fib2;
		fib2 = fib3;
		fib3 = fib1 + fib2;
		count++;
	}
	return fib3;
}


void loopSimulator()
{
	uint32_t x0 = 0;
	addresses[0].store(&x0);

	uint32_t x1 = 0;
	addresses[1].store(&x1);

	uint32_t x2 = 0;
	addresses[2].store(&x2);

	uint32_t x3 = 0;
	addresses[3].store(&x3);

	uint32_t x4 = 0;
	addresses[4].store(&x4);

	for (unsigned i = 0; i < INNER_LOOPS; i++)
	{
		x0++;
		x1 = x1 + x0;
		x2 = x2 + x1;
		x3 = x3 + x2;
		x4 = x4 + x3;
	}
	TOTAL_SUM = TOTAL_SUM + x0 + x1 + x2 + x3 + x4;

	addresses[0].store(&dummies[0]);
	addresses[1].store(&dummies[1]);
	addresses[2].store(&dummies[2]);
	addresses[3].store(&dummies[3]);
	addresses[4].store(&dummies[4]);
}


void checkValues()
{
	uint32_t start = 20;
	{
		uint32_t x5 = getFib(start);
		addresses[5].store(&x5);

		TOTAL_SUM += x5;

		addresses[5].store(&dummies[5]);
	}
	start++;
	{
		uint32_t x6 = getFib(start);
		addresses[6].store(&x6);

		TOTAL_SUM += x6;

		addresses[6].store(&dummies[6]);
	}
	start++;
	{
		uint32_t x7 = getFib(start);
		addresses[7].store(&x7);

		TOTAL_SUM += x7;

		addresses[7].store(&dummies[7]);
	}
	start++;
	{
		uint32_t x8 = getFib(start);
		addresses[8].store(&x8);

		TOTAL_SUM += x8;

		addresses[8].store(&dummies[8]);
	}
	start++;
	{
		uint32_t x9 = getFib(start);
		addresses[9].store(&x9);

		TOTAL_SUM += x9;

		addresses[9].store(&dummies[9]);
	}
}

void UserApp()
{
	while (runActivity)
	{
		USER_APP_CYCLES++;
		loopSimulator();
		checkValues();
	}
}


void Measure()
{
	const std::chrono::duration<double, std::ratio<1, 1000000>> span = std::chrono::milliseconds(1);
	std::chrono::high_resolution_clock::time_point lastSample = std::chrono::high_resolution_clock::now();
	unsigned loopCount = 0;
	while (loopCount < MEASURE_CYCLES)
	{
		std::chrono::high_resolution_clock::time_point nextSample = std::chrono::high_resolution_clock::now();
		if (nextSample - lastSample > span)
		{
			for (unsigned i = 0; i < VARIABLES; i++)
			{
				if (*addresses[i] != MAX_VALUE)
				{
					measured[i] = *(addresses[i]);
					MEASURE_SUM += measured[i];
				}
			}

			lastSample = nextSample;
			loopCount++;
			if (loopCount % 1000 == 0)
				cout << loopCount << " measurements\n";
		}
	}
	runActivity = false;
}


int main()
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::thread AppThread(UserApp);
	std::thread MeasureThread(Measure);
	AppThread.join();
	MeasureThread.join();
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto time_duration = std::chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
	cout << "Passive User App Atomic: " << endl;
	cout << MEASURE_CYCLES << " measurements in " << time_duration << " milliseconds" << endl;
	cout << "UserApp run " << USER_APP_CYCLES / 1000000 << " million cycles" << endl;
	cout << "TOTAL_SUM: " << TOTAL_SUM << endl;
	cout << "MEASURE_SUM: " << MEASURE_SUM << endl;

	__debugbreak();
}