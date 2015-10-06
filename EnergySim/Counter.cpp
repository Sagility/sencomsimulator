#include "Stdafx.h"
#pragma once
#include "Counter.h"
#include "SimEngine.h"
using namespace std;
namespace EnergySim
{
	void writeMyLogger(string s)
	{
		//return;
		ofstream myfile;
		myfile.open("mytestlogCounter.txt", std::ios::app);
		myfile << s;
		myfile << "\n";
		myfile.close();
		return;
	}
	class CounterUserInfo
	{
	public:
		CounterUser* itsUser;
		Counter* itsCounter;
		int itsBucket;
		double itsTimeIntoBucket;
	};
	CounterUser::CounterUser(std::function<void()> theStartFP, std::function<void()> theEndFP, int theID)
	{
		itsStartFP = theStartFP;
		itsEndFP = theEndFP;
		itsID = theID;
	}
	void CounterUser::reachedEnd()
	{
		itsEndFP();
	}
	void CounterUser::started()
	{
		itsStartFP();
	}
	Counter::Counter(ISimEngine* theEngine, SimContext* thectx)
	{
		itsEngine = theEngine;
		_ctx = thectx;
		aCJC = new CombinedJobController(_ctx, "delayAndDoJobFunctionDoer");
		aDJob = new DelayJob(_ctx, updateTime);
		aCJC->AddJob(aDJob);
		itsFP = std::bind(&Counter::tick, this);
		aCJC->AddJob(new FunctionDoJob(_ctx, itsFP)); // Memoryleak
		itsEngine->ScheduleJobNow(aCJC);
		for (int i = 9999; i > -1; i--)
			slots[i] = NULL;
	}
	void Counter::tick()
	{
		bool atEnd = false;
		if (slots[9999] == NULL)
			atEnd = true;
		for (int i = 9999; i > 0; i--)
		{
			if (slots[i] == NULL)
			{
				slots[i] = slots[i - 1];
				slots[i - 1] = NULL;
			}
		}
		if (slots[0] == NULL)
		{
			if (slotsInTraining.size() > 0)
			{
				slots[0] = slotsInTraining.front();
				slotsInTraining.pop_front();
				slots[0]->started();
			}
		}
		if (atEnd)
			if (slots[9999] != NULL)
				slots[9999]->reachedEnd();
		if (!atEnd)
			slots[9999] = NULL;

		//writeMyLogger(to_string(itsEngine->simulated_time()));
		bool done = true;
		for each (CounterUser* var in slots)
		{
			if (var!=NULL)
			{
				done = false;
				break;
			}
		}
		if (done)
		{
			writeMyLogger(to_string(itsEngine->simulated_time()));
			return;
		}
		if (itsEngine->simulated_time() > 100000)
			return;
		aDJob = new DelayJob(_ctx, updateTime);// Memoryleak
		aCJC->AddJob(aDJob);
		aCJC->AddJob(new FunctionDoJob(_ctx, itsFP)); // Memoryleak
		itsEngine->ScheduleJobNow(aCJC);
		//writeMyLogger("leaving tick");
	}
	Counter::~Counter()
	{

	}
	void Counter::insert(CounterUser* theUser)
	{
		slotsInTraining.push_back(theUser);
		return;


		waiting.push_back(make_pair(theUser,itsEngine->simulated_time()));
		update();
	}
	list<pair<int, double>>* Counter::getPos()
	{
		list<pair<int, double>>* aList = new list<pair<int, double>>();


		return aList;
	}
	void Counter::update()
	{
		double nextTime = 100000;
		bool needsFutureUpdates = false;
		double time = itsEngine->simulated_time();
		double delta = time - lastUpdateTime;
		if (inSystem.size() > 0)
		{
			CounterUserInfo* aCUI = inSystem.front();
			if (aCUI->itsBucket)
				int bucketIndex = aCUI->itsBucket;

		}
		lastUpdateTime = time;

		if (inSystem.size() > 0)
		{





			for each (CounterUserInfo* var in inSystem)
			{
				double left = var->itsCounter->buckets[var->itsBucket].first;
				left = -var->itsTimeIntoBucket;

				double tmp = delta;
				int bucketIndex = var->itsBucket;
				double bucketTime = var->itsCounter->buckets[bucketIndex].first - var->itsTimeIntoBucket;
				while (tmp > 0)
				{


				}
			}
		}
		if (waiting.size() == 0)
		{

			return;
		}

	}
}