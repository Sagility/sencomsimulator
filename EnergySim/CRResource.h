#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include "config.h"
#include "Job.h"
#include "SimEngineTimer.h"
#include "TimeValues.h"
#include <list>
#include "ClaimRelease.h"
#include "Entity.h"
#include "Jobs.h"

#include "Admin.h"

using namespace std;

namespace EnergySim 
{
	class Resource;
	class Process;
	class ConcreteProcess;
	class IJobAssigner;
	class ResourceHandler
	{
	public:
		ResourceHandler(SimModel* theModel);
		ResourceHandler(SimModel* theModel,IJobAssigner* assigner);
		static ResourceHandler* gResourceHandler(){ return itsResourceHandler; };
		IJob* getResources(long ProcessID, long RouteFollowerID);
		bool assignJob();
		friend class IJobAssigner;
		Resource* getResource(string name) { return itsResources.front(); };
		void wait(WaitForResourcesJob* aJob);
		void done(long theRouteFollowerID, long theProcessID);
	protected:
		static ResourceHandler* itsResourceHandler;
		list<Resource*> itsResources;
		list<pair<WaitForResourcesJob*, Process*>> itsResReq;
		
		list<WaitForResourcesJob*> waiting;
		list<pair<pair<long,long>,ConcreteProcess*>> onGoing; // LotID, ProcessID, Reslist
		SimModel* model;
		IJobAssigner* itsAssigner;
	};
	class IJobAssigner
	{
	public:
		IJobAssigner(ResourceHandler* theRH){ itsRH = theRH; }
		virtual bool assignJob()= 0;
	protected:
		list<Resource*> resources() { return itsRH->itsResources; }
		ResourceHandler* itsRH;
		list<pair<WaitForResourcesJob*, Process*>> resReg() { return itsRH->itsResReq; }
		void assignAJob(WaitForResourcesJob* aWFR,	list<Resource*>* aRL );
	};
	class TestJobAssigner : public IJobAssigner
	{
	public:
		TestJobAssigner(ResourceHandler* theRH) : IJobAssigner(theRH){}
		virtual bool assignJob();
		bool IsOdd(Resource* r) { return true; }
	};
	class IEntity;

	using namespace std::placeholders;
	class  Resource : public IEntity, protected ClaimReleaseResourceHandler<long>
	{
	public:
		Resource(IClock* theClock, string theName) : itsCapacity(0, theClock), itsUsedCapacity(0, theClock)
		{
			itsName = theName;
			MyRealSetter* aRS = new MyRealSetter(std::bind(&Resource::setDoubleCap, this, _1));
			SetterHandler::getSetterHandler()->store(itsName, aRS);
			IEvent::publishEvent(ET_RESOURCE_USES, vector<string>(2) = { (name()), to_string(0) });
		}
		int capacity() { return itsCapacity.getValue(); }
		void setDoubleCap(double d){ setCapacity(d); }
		void setCapacity(int cap) 
		{
			itsCapacity = itsCapacity + cap - itsCapacity.getValue(); 
			IEvent::publishEvent(ET_RESOURCE_CAPACITY, vector<string>(2) = { (name()), to_string(cap) });
		}
		int free(){ return itsCapacity.getValue() - itsUsedCapacity.getValue(); }

		void free(long value)
		{
			vector<long> v1;
			v1.push_back(value);
			IEvent::publishEvent(ET_RESOURCE_RELEASE, eID(), v1);
			itsUsedCapacity = itsUsedCapacity - value; 
			vector<long> v2;
			v2.push_back( itsUsedCapacity.getValue() );
			IEvent::publishEvent(ET_RESOURCE_USES, eID(), v2);
			IEvent::publishEvent(ET_RESOURCE_USES, vector<string>(2) = { (name()), std::to_string(itsUsedCapacity.getValue()) });
		}

		void get(IClaimer<long>* theClaimer, long neededCapacity)
		{
			vector<long> v1;
			v1.push_back(neededCapacity);
			IEvent::publishEvent(ET_RESOURCE_CLAIM, eID(), v1);
			itsUsedCapacity = itsUsedCapacity + neededCapacity;
			itsClaimers.push_back(pair<IClaimer<long>*, int>(theClaimer, neededCapacity));
		}
		void getCapacity(long neededCapacity)
		{
			vector<long> v1;
			v1.push_back(neededCapacity);
			IEvent::publishEvent(ET_RESOURCE_CLAIM, eID(), v1);
			itsUsedCapacity = itsUsedCapacity + neededCapacity;
			vector<long> v2;
			v2.push_back(itsUsedCapacity.getValue());
			IEvent::publishEvent(ET_RESOURCE_USES, eID(), v2);
			IEvent::publishEvent(ET_RESOURCE_USES, vector<string>(2) = { (name()), std::to_string(itsUsedCapacity.getValue()) });
		}
		string name(){ return itsName; };
		void free(IClaimer<long>* theClaimer)
		{
			for (pair<IClaimer<long>*, int> aP : itsClaimers)
			{
				if (aP.first == theClaimer)
				{
					itsUsedCapacity = itsUsedCapacity - aP.second;
					IEvent::publishEvent(ET_RESOURCE_USES, vector<string>(2) = { (name()), std::to_string(itsUsedCapacity.getValue()) });
					itsClaimers.remove(aP);
					break;
				}
			}
		}
		double getAvgUse() { return itsUsedCapacity.getAvg(); }
		double getAvg() { return itsCapacity.getAvg(); }
		long setup() { return itsSetup; }
		long setup(int theSetup){ return itsSetup = theSetup; }
		double setupTime(long theSetup)
		{
			if (theSetup == itsSetup)
				return 0;
			return 10;
		}
		ResourceState state() { return itsState; }
		ResourceState state(ResourceState theState) { return itsState = theState; }
	private:
		string itsName;
		ResourceState itsState;
		long itsSetup;
		TimeValue<long> itsCapacity, itsUsedCapacity;
		list<pair<IClaimer<long>*, int>> itsClaimers;
	};
	class ConcreteProcess 
	{
	public:
		long prio;
		long id;
		list<pair<long, Resource*>>* need = new list<pair<long, Resource*>>();
		ConcreteProcess(list<pair<long, Resource*>>* theNeed, long theID){ need = theNeed; id = theID; }
	private:
		static long startID;
	};
	class Process : public IEntity
	{
	public:
		Process* copy()
		{
			Process* aP = new Process();
			aP->processID = processID;
			for each (auto var in itsAlternates)
			{
				aP->itsAlternates.push_back(var);
			}
			return aP;
		}
		long processID;
		list<ConcreteProcess*> itsAlternates = list<ConcreteProcess*>();
	};
}