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
#include "Entity.h"
#include "SimAdmin.h"

using namespace std;

namespace EnergySim {
	
	class Step;
	class IRoute
	{
		public:
			list<Step*>::iterator  getIterator(){ return itsSteps.begin(); }
		private:
			list<Step*> itsSteps;
	};
	class Resource;

	class IRouteFollower : public IEntity, public IJobFinishedListener, public IRouteSlots
	{
		public:
			void nextStep();
			//IRouteFollower(string route);
			long orderID() { return itsOrderID; }
			Resource* primaryRes(long theProcessID);
			void addClaimedByProcessID(long theProcessID, Resource* theClaimed);
			void releaseByStepID(long theStepID);
			void releaseByRes(Resource* theRes);
			void releaseAll();
		protected:
			list < pair<long, list<Resource*>*>*> itsClaimedByProcessID;
			SimContext* itsSimContext;
			list<Resource*> itsRoutes;
			IRoute* itsRoute;
			Step* itsCurrentStep;
			list<Step>::const_iterator itsStepI;
			CombinedJobController* itsCJC;
			long itsOrderID;
	};

	class OperationDoer : public IRouteFollower
	{
		public: 
			OperationDoer(string route, list<long> preReqs, long id);
			long itsID;
			list<long> itsNeededPreReqs;
	};

	class WaitForPreReqDoneJob;
	class PreReqs
	{
	public:
		static void wait(WaitForPreReqDoneJob* theJob) { itsWaiters.push_back(theJob);  while (startWaiters()); }
		static void done(long theDone) { itsDonePreReqs.push_back(theDone); while (startWaiters()); };
	private:
		static bool canIStart(WaitForPreReqDoneJob* theJob);
		static bool startWaiters();
		static list<WaitForPreReqDoneJob*> itsWaiters;
		static list<long> itsDonePreReqs;
	};

}