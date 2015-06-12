#pragma once
#include "EnergySim.h"
#include <vector>
#include <list>
#include <algorithm>
#include "Job.h"
#include "ClaimRelease.h"
#include <memory>
#include "SimEngine.h"

using namespace std;

namespace EnergySim
{
	class IClock
	{
		public:
			virtual double getTime(){ return 0; }
	};
	class Clock : public IClock
	{
		public:
			Clock(ISimEngine* theEngine = NULL)
			{
				itsEngine = theEngine;
				time = 0;
			}
			virtual double getTime() 
			{
				if (itsEngine!=NULL)
					return itsEngine->simulated_time();
				return time;
			}
			double time;
		private:
			ISimEngine* itsEngine;			
	};
	template<typename T>
	class TimeValue
	{
		public:
			TimeValue(T the, IClock* theClock)
			{
				it = the;
				itsClock = theClock;
				startTime = itsClock->getTime();
				min = it;
				max = it;
				store();
			}
			TimeValue operator* (T x)
			{
				it = it *x;
				store();
				return *this;
			}
			TimeValue operator+ (T x)
			{
				it = it + x;
				store();
				return *this;
			}
			TimeValue operator- (T x)
			{
				it = it - x;
				store();
				return *this;
			}
			TimeValue operator/ (T x)
			{
				it = it / x;
				store();
				return *this;
			}
			T getValue(){ return it; }
			double getMin() { return min; }
			double getMax() { return max; }
			double getAvg() 
			{
				if (!avgUpdated)
					avg = calculateAverage();
				return avg; 
			}
			double getValue(double theTime)
			{
				for (pair<double, T> aP : itsTimeValues)
				{
					if (aP.first > theTime)
						return aP.second;
				}
				return it;
			}
			bool avgUpdated = true;
	private:
			IClock* itsClock;
			void store()
			{
				if (it < min)
					min = it;
				if (it > max)
					max = it;
				itsTimeValues.push_back(pair<double, T>(itsClock->getTime(), it));
				avgUpdated = false; 
			}
			double calculateAverage()
			{
				if (itsTimeValues.empty())
					return 0;
				double aTotal = 0;
				double lastTime = startTime;
				double lastValue = 0;
				currentTime = itsClock->getTime();
				if ((currentTime - startTime) < 0.000001)
					return 0;
				for (pair<double, T> aP : itsTimeValues)
				{
					aTotal += (aP.first - lastTime) * lastValue;
					lastValue = aP.second;
					lastTime = aP.first;
				}
				aTotal += (currentTime - lastTime) * lastValue;
				avgUpdated = true;
				return aTotal / (currentTime-startTime);
			}
			std::list<pair<double,T>> itsTimeValues;
			T it,min,max,avg;
			double startTime, currentTime;
	};
}