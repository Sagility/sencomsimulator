// EnergySim.h

#pragma once
#include <vector>
#include <algorithm>
#include "config.h"
#include "SimLogEnvironment.h"
#include "SimEngine.h"
using namespace std;

namespace EnergySim {

	// forward declarations
	class IJob; 
	class EventArgs;
	class ISimEngine;
	class SimModel; 
	class IEnvironment;

	class ENERGYSIM_DLL_PUBLIC SimContext
	{
	private:
		ISimEngine* _engine;
		IEnvironment *_environment;
		
	public:
		SimModel * aModel;
		//constructor
		SimContext(){};

		SimContext(ISimEngine* engine, SimModel *model);
		ISimEngine* engine() const  { return _engine; }
		void set_engine(ISimEngine* theEngine) ; 

		SimModel* model() const { return aModel; }
		void         set_model(SimModel*theModel)  {  aModel=theModel; }

		IEnvironment* environment() const { return _environment; }
		
	};

};

