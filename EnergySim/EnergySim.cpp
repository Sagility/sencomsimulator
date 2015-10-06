// This is the main DLL file.

#include "stdafx.h"
#include "EnergySim.h"

namespace EnergySim {
		SimContext::SimContext(ISimEngine* engine, SimModel *model)
		{
			_engine = engine;
			if(_engine!=NULL){
				_environment= _engine->environment();
			}
		}
		void SimContext::set_engine(ISimEngine* theEngine)  {  
			_engine=theEngine; 
			if(_engine!=NULL){
				_environment= _engine->environment();
			}		
		}
};