#pragma once

#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <vector>         // std::vector
#include <deque>          // std::deque
#include "EnergySim.h"
#include "SimEngine.h"
#include "Job.h"
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <type_traits>
#include <functional>
#include "Parsing.h"
#include "Attributes.h"
using namespace std;

namespace EnergySim
{
	class Schedule;
	class WaitForScheduleJob;
	class SimModel;
	class ConcreteProcess;
	struct SchedElement
	{
	public:
		SchedElement(long a, long b, long c, long d, list<long> e);
		long lotID;
		long processID;
		long time;
		long processModeID;
		list<long> resID;
	};

	class Schedule
	{
	public: list<SchedElement*>* itsInSchedule = new list<SchedElement*>();
			static void printOutSchedule(string s1, string s2);
	private: vector<pair<double, pair<long, long>>> *itsOutSchedule = new vector<pair<double, pair<long, long>>>();
	public: list<WaitForScheduleJob*> *itsWaiters = new list<WaitForScheduleJob*>();
	private: string itsName = "";
	private:  SimContext itsContext;
			  SimModel* model;
	public: 
		Schedule(string theName, SimContext& theContext)
		{
			itsName = theName;
			itsContext = theContext;
			model = itsContext.model();
		}
		void finish()
		{
			string line = "";
		//	using (System.IO.StreamWriter file = new System.IO.StreamWriter(itsName + ".outschedul"))
			{

				for(pair<double, pair<long, long>> p : *itsOutSchedule)
				{
				//	line = p.first().ToString() + " " + p.Value.Value.ToString() + " " + p.Key.ToString();
					//file.WriteLine(line);
				}
			}
		}
		void addJob(long lotID, long stepID, long time, long concreteProcessID, list<long> resIDs)
		{
			itsInSchedule->push_back(new SchedElement(lotID, stepID, time, concreteProcessID, resIDs));
		}
		void sort()
		{
			itsInSchedule->sort([](const SchedElement *a, const SchedElement *b) {return a->time < b->time; });
		}
		SchedElement* getSE(long lotID, long stepID)
		{
			for (SchedElement *aSE : *itsInSchedule)
				if (lotID == aSE->lotID)
					if (stepID == aSE->processID)
						return aSE;
			return NULL;
		}
		void filterOutConcreteProcess();
		void markJobAsStarted(long lotID, long stepID);
		bool canIStart(long lotID, long stepID)
		{
			// Does the number exist in the schedule. if not it can start
			// If it exists in the schedule, is it the first one, then it can start
			// If there are other numbers before it, but none of them uses a resource that this uses, then it can start
			// Otherwise it can not start
			pair<long, long> *aJob = new pair<long, long>(lotID, stepID);
			SchedElement *aSE = NULL;
			bool found = false;
			for (SchedElement *dSE : *itsInSchedule)
			{
				if (aJob->first == dSE->lotID)
					if (aJob->second == dSE->processID)
					{
						found = true;
						aSE = dSE;
						break;
					}
			}
			if (!found)
				return true;

			if (itsInSchedule->empty())
				return true;

			if (aSE->lotID == 26)
				found = found;

			if (aSE->lotID == itsInSchedule->front()->lotID)
				if (aSE->processID == itsInSchedule->front()->processID)
					return true;

			for(SchedElement *bSE : *itsInSchedule)
			{
				if (aSE->lotID == bSE->lotID)
					if (aSE->processID == bSE->processID)
						return true;
				for (long aL :aSE->resID)			
					for (long bL : bSE->resID)
						if (aL==bL)
							return false;
			}
			return true;
		}
	};
}