#pragma once
#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <vector>         // std::vector
#include <deque>          // std::deque
#include "EnergySim.h"
#include "SimEngine.h"
#include "Job.h"
#include <set>
#include "CombinedJobController.h"
#include "ClaimRelease.h"
#include "Attributes.h"
#include "Parsing.h"
#include "Jobs.h"
#include "Schedule.h"
#include "Routes.h"
using namespace std;


namespace EnergySim_NameConstants
{
	const std::string DELAYSTEP = "DELAY";						// Time
	const std::string WAIT_ATTRIBUTE_STEP = "WAIT_ATTRIBUTE";			// ID, värde
	const std::string PUBLISH_ATTRIBUTE_STEP = "SET_ATTRIBUTE";		//
	const std::string PUBLISH_ROUTEFOLLOWER_TO_ATTRIBUTE_STEP = "SET_ATTRIBUTE_FROM_LOT";
	const std::string WAIT_SCHEDULE_STEP = "WAIT_SCHEDULE";
	const std::string PUBLISH_SCHEDULE_STEP = "PUBLISH_SCHEDULE";
	const std::string WAIT_PREREQ_STEP = "WAIT_PREREQ";
	const std::string PUBLISH_PREREQ_STEP = "PUBLISH_PREREQ";
	const std::string GET_STEP = "GET";
	const std::string FREE_STEP = "FREE";
	const std::string STATE_SET = "SET_STATE";
	const std::string WAIT_SETUP_STEP = "SETUP";
	const std::string LOGSTEP = "LOG";
	const std::string GOTOIFSTEP = "GOTOIF";
	const std::string RUNCONTROL = "RUNCONTROL";
	const std::string IDSTEP = "ID";
	const std::string EVENTSTEP = "EVENT";
}

namespace EnergySim
{
	class Route;
	class Process;
	class Order;
	class Schedule;
	class WaitForScheduleJob;
	class SimContext;
	class TokenListner
	{
		public:
			virtual void gotInt(long aL);
			virtual void gotDouble();
			virtual void gotBool();
			virtual void gotString();
			virtual void gotDateTime();
			virtual void getEnd();
	};
	class SimAdmin
	{
	public:
		 list<Route*> *itsRoutes = new list<Route*>();
		 list<Order*> *itsOrders = new list<Order*>();
		 map<long, long> *itsRes = new map<long, long>();
		 list<Process*> *itsProcess = new list<Process*>();
		 AttributeHandler *itsHandler;
		 SimContext *Context;
		 Schedule *aScheduler;
		 bool createModel(string basename)
		{
			itsHandler = new AttributeHandler();
			Context = new SimContext();
			aScheduler = new Schedule(basename, *Context);
			readModel(basename);
			return true;
		}

		long runModel();
		void readOrders(string filename){};
		void readModel(string basename)
		{
			readOrders(basename);
		}
		
		void readResources(string filename);
		void readRoutes(string filename);
		void readRoute(string filename, string theName);
		void readFunctionAttributes(string filename);
		void readExpressionAttributes(string filename);
		void readSchedule(string filename);
	};
	class Step
	{
		public: 
			void generateJobs(Schedule* theS, CombinedJobController& theController, Order* theOrder);
		    virtual void generateSpecificJobs(CombinedJobController& theController)  {     }
		protected: 
			long itsID;
			SimContext* itsContext;
	};
	class SimModel;
	class ENERGYSIM_DLL_PUBLIC StepReader
	{
		// Reads a line and creates the correct step with the right parameters,
		// the step itself creates the jobs when given an CombinedJobController
		// OR instead of a CJC can we send a RouteFollower
		public:
			Step* readLine(string str);
			SimContext* itsCtx;
			AttributeHandler* itsHandler;
			Schedule* itsSchedule;
			SimModel* model;

		private:
			Step* translateToStep(list<string> arg);
	};
	#include "EnergySim.h"
	struct Order
	{
	public:
		 long ID;
		 long Time;
		 string Route;
		 long stepID;
		 list<long> preReqs;
	}; 
	class Route
	{
		long ID;
		public: string name = "";
		public: vector<Step*> itsSteps =  vector<Step*>();
		public: void addStep(Step* theStep)
		{
			itsSteps.push_back(theStep);
		}
	public: void createThisJobOrder(Schedule* theS, Order* aOrder, CombinedJobController& cj1);
	};

}