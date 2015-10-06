#include "Stdafx.h"
#pragma once
#include "CRResource.h"
using namespace std;
namespace EnergySim
{
	class CounterUserInfo;
	class ISimEngine;
	class ENERGYSIM_DLL_PUBLIC CounterUser
	{
	public:
		CounterUser(std::function<void()> theStartFP, std::function<void()> theEndFP, int theID);
		void started();
		void reachedEnd();
		int itsID;
	private:
		std::function<void()> itsStartFP;
		std::function<void()> itsEndFP;
	};
	class ENERGYSIM_DLL_PUBLIC Counter
	{
	public:
		Counter(ISimEngine* theEngine, SimContext* thectx);
		ISimEngine* itsEngine;
		void tick();
		void insert(CounterUser* theUser);
		list<pair<int, double>>* getPos();
		~Counter();
	private:
		std::function<void()> itsFP;
		DelayJob* aDJob;
		CombinedJobController* aCJC;
		SimContext* _ctx;
		vector<CounterUser*> slots = vector<CounterUser*>(10000);
		list<CounterUser*> slotsInTraining = list<CounterUser*>();
		double updateTime = 2;

		vector<pair<double, double>> buckets = vector<pair<double, double>>(); // Time bucket, time remaining
		double bucketLength;
		list<pair<CounterUser*, double>> waiting = list<pair<CounterUser*, double>>();
		list<CounterUserInfo*> inSystem = list<CounterUserInfo*>();
		void update();
		double lastUpdateTime;
	};
}