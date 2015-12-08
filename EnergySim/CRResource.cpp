#include "Stdafx.h"
#include "ClaimRelease.h"
#include "CRResource.h"
#include "SimModel.h"
#include <ppl.h>

using namespace concurrency;
using namespace std;

namespace EnergySim
{
	long ConcreteProcess::startID = 0;

	void FinishProcessJob::Execute()
	{
		NotifyJobStarted();
		model->itsResHandler->done(itsFollowerID, itsProcessID);
		NotifyJobFinished();
	}

	FinishProcessJob::FinishProcessJob(long ProcessID, long RouteFollowerID, SimModel* theModel)
	{
		this->set_context(theModel->context());
		this->itsFollowerID = RouteFollowerID;
		this->itsProcessID = ProcessID;
		this->model = theModel;
	}

	void ResourceHandler::done(long theRouteFollowerID, long theProcessID)
	{
		for (pair<pair<long, long>, ConcreteProcess*> p1 : onGoing)
			if ((p1.first.first == theRouteFollowerID) & (p1.first.second == theProcessID))
			{
				for (pair<long, Resource*> p2 : *(p1.second->need))
				{
					p2.second->free(p2.first);
					IEvent::publishEvent(ET_RESOURCE_RELEASE, vector<string>(2) = { (p2.second->name()), "LOT_" + p1.first.first });
					IEvent::publishEvent(ET_PROCESS_END, vector<string>(3) = { (p2.second->name()), "LOT" + std::to_string(theRouteFollowerID), std::to_string(-p2.first) });
					if (p2.second->name() == "0")
					{
						double d = model->itsParser.itsValue->getAttributeHandler()->getAttribute("res");
						d -= p2.first;
						model->itsParser.itsValue->getAttributeHandler()->replace("res", new ConstantValue(d));
					}
				}
				onGoing.remove(p1);
				while (assignJob());
				return;
			}
	}
	ResourceHandler* ResourceHandler::itsResourceHandler = new ResourceHandler(NULL);
	ResourceHandler::ResourceHandler(SimModel* theModel)
	{
		model = theModel;
	}
	ResourceHandler::ResourceHandler(SimModel* theModel, IJobAssigner* assigner)
	{
		model = theModel;
		itsAssigner = assigner;
	}
	WaitForResourcesJob::WaitForResourcesJob(Process* theProcess, SimModel* theModel, long theRouteFollowerID, CombinedJobController* theController)
	{
		itsController = theController;
		itsResReq = theProcess->copy();
		model = theModel;
		this->set_context(theModel->context());
		itsRouteFollowerID = theRouteFollowerID;
		//return;  // TESTING Remove later
		SchedElement* aSE = model->inSchedule.itsValue->getSE(itsRouteFollowerID, itsResReq->processID);
		if (aSE == NULL)
			return;
		/* Remove the alternates that are not compatibel with the schedule */	
		if (itsResReq->itsAlternates.size() > 1)
		{
			bool found = false;
			if (itsResReq->processID == aSE->processID)
				found = true;
			if (found)
				itsResReq->itsAlternates.remove_if([aSE](ConcreteProcess* a) { return !(a->id == aSE->processModeID); });
		}
	}
	void WaitForResourcesJob::claimResources()
	{
		model->itsResHandler->wait(this);
	}
	void ResourceHandler::wait(WaitForResourcesJob* aJ)
	{
		SchedElement* aSE = model->inSchedule.itsValue->getSE(aJ->itsRouteFollowerID, aJ->itsResReq->processID);
		if (aSE != NULL)
			waiting.push_front(aJ);
		else
			waiting.push_back(aJ);
		while (assignJob())	;
	}

	long IEntity::itsUniqueID = 1000;

	bool ResourceHandler::assignJob()
	{
		int t = 56;
		if (model->context()->engine()->simulated_time() > 400)
			t = 78;
		

		for (WaitForResourcesJob* aJ : waiting)
		{
			Process* aP = aJ->itsResReq;
			for (ConcreteProcess* aCP : aP->itsAlternates)
			{
				bool doCP = true;
				for (pair<long, Resource*> p : *(aCP->need))
				{
					if (p.second->free() <p.first)
					{
						doCP = false;
						break;
					}
				}
				if (!doCP)
					continue;
				// This CP works!
				string s2 = " " + std::to_string(aCP->id);

				if (!aCP->need->empty())
					s2 += " " + aCP->need->front().second->name();
				for(pair<long, Resource*> p : *(aCP->need))
				{
					if (p.second->free() < p.first)
					{}
					else
					{
						p.second->getCapacity(p.first);
						onGoing.push_back(pair<pair<long, long>, ConcreteProcess*>(pair<long, long>(aJ->itsRouteFollowerID,aP->processID),aCP));
						aJ->itsController->addSlot("PROCESS", aP->processID);
						aJ->itsController->addSlot("CONCRETE_PROCESS", aCP->id);
						IEvent::publishEvent(ET_RESOURCE_CLAIM, vector<string>(2) = { (p.second->name()), "LOT_" + std::to_string(aJ->itsRouteFollowerID) });
						IEvent::publishEvent(ET_PROCESS_START, vector<string>(3) = { (p.second->name()), "LOT" + std::to_string(aJ->itsRouteFollowerID), std::to_string(p.first) });
						if (p.second->name() == "0")
						{
							double d = model->itsParser.itsValue->getAttributeHandler()->getAttribute("res");
							d += p.first;
							model->itsParser.itsValue->getAttributeHandler()->replace("res", new ConstantValue(d));
						}
						continue;
					}
				}
				// Find stepID
				string s1 = std::to_string(aJ->itsRouteFollowerID) + " " +std::to_string(aP->processID) + " ";
				s1 += std::to_string(aJ->context()->engine()->simulated_time());
				Schedule::printOutSchedule(s1, s2);

				waiting.remove(aJ);
				aJ->finish();
				vector<long> aV;
				aV.push_back(aCP->id);
				IEvent::publishEvent(ET_PROCESS_START, aJ->itsRouteFollowerID, aV);
				return true;
			}
		}
		return false;
	}

	void IJobAssigner::assignAJob(WaitForResourcesJob* aWFR, list<Resource*>* aRL)
	{

	}
	
	bool isEmpty(Resource* r) { if (r->free() < 1) return true; return false; }

	bool byClaimer(pair<WaitForResourcesJob*, Process*> a, pair<WaitForResourcesJob*, Process*> b)
	{
		//if (a.first->claimer()->ID() > b.first->claimer()->ID())
		//	return true;
		return false;
	}

	bool shouldWork(Resource* r1, Resource* r2) 
	{
		return true;
	}

	bool TestJobAssigner::assignJob()
	{
		// Assign according to prio of claimers
		// Assign according to largest job first
		// Assign according to prio of resource
		// Assign according to schedule

		list<Resource*> aRL = resources();
		aRL.remove_if(isEmpty);
		aRL.sort(shouldWork);
	//	list<list<Resource*>>
		list<pair<WaitForResourcesJob*, Process*>> aResReg = resReg();
		aResReg.sort(byClaimer);

		WaitForResourcesJob* aWFR = NULL;
		list<Resource*>* aCRL =  new list<Resource*>();

		if (aWFR == NULL)
			return false;
		assignAJob(aWFR, aCRL);
		return true;
	};

	IJob* ResourceHandler::getResources(long ProcessID, long RouteFollowerID)
	{
		return NULL;
	}
}