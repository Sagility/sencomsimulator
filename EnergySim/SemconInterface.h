#pragma once
#include "Stdafx.h"
#include "config.h"
#include<string>
#include <list>
using namespace std;





namespace SemconInterface
{
	/* POD objects*/
	struct ENERGYSIM_DLL_PUBLIC SchedJobMode
	{
	public:
		string name;
		int duration;
		list<pair<string, int>>* resourceUse = new list<pair<string, int>>();
	};
	struct ENERGYSIM_DLL_PUBLIC SchedJob
	{
	public:
		string start = "";
		string end = "";
		string name = "";
		string orderName = "";
		list<SchedJobMode*> modes = list<SchedJobMode*>();
		string mode;
	};
	struct ENERGYSIM_DLL_PUBLIC ResInSchedule
	{
	public:
		string name;
		int capacity;
		list<pair<string, string>> availibes = list<pair<string, string>>();
	};
	struct ENERGYSIM_DLL_PUBLIC ResSchedule
	{
	public:
		string resName;
		string orderName;
		string jobName;
		string mode;
		string startDate;
		string endDate;
	};

	class ENERGYSIM_DLL_PUBLIC SenComModel
	{
	public:
		static SenComModel* getModel(string definitionFileName, string schedFilename);
		void geResources(string definitionFileName, string schedFilename);
		void getJobs(string definitionFileName, string schedFilename);
		SenComModel();


		string message = "";
		list<ResInSchedule*> _resources = list<ResInSchedule*>();
		list<SchedJobMode*> _jobs = list<SchedJobMode*>();
	private:
		~SenComModel();
	};
	
	ENERGYSIM_DLL_PUBLIC int internalMain(int argc, char* argv[]);
	ENERGYSIM_DLL_PUBLIC int rain(int argc, char* argv[]);
}