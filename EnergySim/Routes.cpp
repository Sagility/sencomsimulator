#include "stdafx.h"
#include "Routes.h"

using namespace std;

namespace EnergySim
{
	list<WaitForPreReqDoneJob*> PreReqs::itsWaiters = list<WaitForPreReqDoneJob*>();
	list<long> PreReqs::itsDonePreReqs = list<long>();
	bool PreReqs::canIStart(WaitForPreReqDoneJob* theJob)
	{
		for (long aID : theJob->itsPreReqs)
		{
			bool aFound = false;
			for (long bID : itsDonePreReqs)
			{
				if (aID == bID)
				{
					aFound = true;
					break;
				}
			}
			if (!aFound)
				return false;
		}
		return true;
	}
	bool PreReqs::startWaiters()
	{
		for (WaitForPreReqDoneJob* aJob : itsWaiters)
		{
			if (canIStart(aJob))
			{
				itsWaiters.remove(aJob);
				aJob->Finish();
				return true;
			}
		}
		return false;
	}
	void IRouteFollower::addClaimedByProcessID(long theProcessID, Resource* theClaimed)
	{
		for (pair<long, list<Resource*>*>* aP : itsClaimedByProcessID)
		{
			if (aP->first == theProcessID)
			{
				aP->second->push_back(theClaimed);
				return;
			}
		}
		list<Resource*>* aList = new list<Resource*>();
		aList->push_back(theClaimed);
		itsClaimedByProcessID.push_back(new pair<long, list<Resource*>*>(theProcessID, aList));
	}
	void IRouteFollower::releaseByRes(Resource* theRes)
	{
		for (pair<long, list<Resource*>*>* aP : itsClaimedByProcessID)
		{
			for (Resource* aRes : *(aP->second))
			{
			}

			if (aP->first == 0)
			{
				aP->second->push_back(theRes);
				return;
			}
		}
	}
	Resource* IRouteFollower::primaryRes(long theProcessID)
	{
		return NULL;
	}
}