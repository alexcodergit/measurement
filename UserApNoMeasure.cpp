#include<iostream>
#include<chrono>
#include<vector>
#include<thread>
using namespace std;

const unsigned MEASURE_CYCLES = 10000;
const unsigned INNER_LOOPS = 10;
unsigned long long USER_APP_CYCLES = 0;
unsigned long long TOTAL_SUM = 0;
unsigned long long MEASURE_SUM = 0;
const unsigned VARIABLES = 10;
const uint32_t MAX_VALUE = numeric_limits<uint32_t>::max();
vector<uint32_t>measured(VARIABLES, 0);
vector<uint32_t>distab(VARIABLES, 0);
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
	uint32_t x1 = 0;
	uint32_t x2 = 0;
	uint32_t x3 = 0;
	uint32_t x4 = 0;

	for (unsigned i = 0; i < INNER_LOOPS; i++)
	{
		x0++;
		x1 = x1 + x0;
		x2 = x2 + x1;
		x3 = x3 + x2;
		x4 = x4 + x3;
	}
	TOTAL_SUM = TOTAL_SUM + x0 + x1 + x2 + x3 + x4;
}


void checkValues()
{
	uint32_t start = 20;
	{
		uint32_t x5 = getFib(start);
		TOTAL_SUM += x5;
	}
	start++;
	{
		uint32_t x6 = getFib(start);
		TOTAL_SUM += x6;
	}
	start++;
	{
		uint32_t x7 = getFib(start);
		TOTAL_SUM += x7;
	}
	start++;
	{
		uint32_t x8 = getFib(start);
		TOTAL_SUM += x8;
	}
	start++;
	{
		uint32_t x9 = getFib(start);
		TOTAL_SUM += x9;
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
				if (distab[i] != MAX_VALUE)
				{
					measured[i] = distab[i];
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
	cout << "User App No Measurements:" << endl;
	cout << MEASURE_CYCLES << " measurements in " << time_duration << " milliseconds" << endl;
	cout << "UserApp run " << USER_APP_CYCLES / 1000000 << " million cycles" << endl;
	cout << "TOTAL_SUM: " << TOTAL_SUM << endl;
	cout << "MEASURE_SUM: " << MEASURE_SUM << endl;

	__debugbreak();
}