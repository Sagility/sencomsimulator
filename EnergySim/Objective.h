#include "stdafx.h"
#include<string>

#include "SimLogEnvironment.h"

#include "EnergySim.h"
#include "SimEngine.h"
#include "CombinedJobController.h"
#include "ClaimRelease.h"
#include "Parsing.h"
#include "Jobs.h"
#include "TimeValues.h"
#include "SimAdmin.h"
#include "Entity.h"
#include "SimModel.h"
#include <ctime>
#include "Admin.h"
#include "FileReader.h"

#include "ComplexResources.h"			// PROJECT DEPENDENT	
#include "Counter.h"					// PROJECT DEPENDENT

#include<iostream>
#include "SemconInterface.h"

using namespace SemconInterface;

using namespace std;
using namespace EnergySim;

enum ObjectiveValue { Main,Extruder,Hydraulic, Grinder };

class ObjectiveReporter
{
public:
	void reportAll();
	void print(unsigned char ov, double value, double time, int lineID);
	void report(unsigned char id, int value, int time, unsigned char lineID);
	void report(unsigned char id, int value, unsigned char lineID);
	void init();
	void testing();
	ISimEngine* itsEngine;
	SimModel* itsModel;
private:
	bool started = false;
	double makeWave(double d);
//	char buf[100];
};
