// EventArgs.h

#pragma once
#include "config.h"
#include <vector>
#include <algorithm>
using namespace std;

namespace EnergySim {


	// base class for EventArgs
	class ENERGYSIM_DLL_PUBLIC EventArgs{
	public:
		EventArgs(){};
		virtual ~EventArgs(){};
	};

}