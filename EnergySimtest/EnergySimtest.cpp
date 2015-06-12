// EnergySimtest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<iostream>

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

using namespace std;
using namespace EnergySim;


int _tmain(int argc, _TCHAR* argv[])
{
	SimModel* aModelino = new SimModel();
	aModelino->readModel();
	cout << "Finished reading" << endl;
	aModelino->runModel();
	cout << "Finished simulation" << endl;
	return 0;

}

