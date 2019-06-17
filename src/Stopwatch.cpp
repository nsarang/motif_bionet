//============================================================================
// Name        : Stopwatch.cpp
// Author      : Nima Sarang
// Version     :
// Copyright   :
// Description :
//============================================================================


#ifndef __STOPWATCH
#define __STOPWATCH

#include <chrono>

class Stopwatch
{
public:
	Stopwatch()
		: tStart( tdClock::now() )
	{
	}
	void resetTime()
	{
		tStart = tdClock::now();
	}
	double elapsedTime() const 
	{
		return std::chrono::duration_cast<tdSecond> (tdClock::now() - tStart).count();
	}

private:
	typedef std::chrono::high_resolution_clock tdClock;
	typedef std::chrono::duration<double, std::ratio<1> > tdSecond;
	std::chrono::time_point<tdClock> tStart;
};


#endif