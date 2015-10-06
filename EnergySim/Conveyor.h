#include "Stdafx.h"
//#include "ComplexResources.h"
#include "SimModel.h"
#include <functional>
#include <sstream>
#include <string>

namespace EnergySim
{
	class Conveyor;
	class ConveyorDisplayer;
	class ENERGYSIM_DLL_PUBLIC ConveyorFollower
	{
	public:
		virtual void inserted(double position){};
		virtual void arrived(double position){};
	};
	class ConveyorFollowerConveyorInfo;
	class ENERGYSIM_DLL_PUBLIC Conveyor
	{
	public:
		Conveyor();
		virtual void insert(ConveyorFollower* theFollower, double position){};
		virtual void remove(ConveyorFollower* theFollower){};
		virtual void display(ConveyorDisplayer* itsDisplayer){};
		virtual void update(){};

	private:
		list<ConveyorFollowerConveyorInfo*> itsInternal;
		double itsLength;
		double itsSpeed;
		SimModel* itsModel;
		ISimEngine* itsEngine;
	};
	class ENERGYSIM_DLL_PUBLIC ConveyorDisplayer
	{
	public:

		void display(ConveyorFollower* follower, double position, Conveyor* conv){};

	};
}