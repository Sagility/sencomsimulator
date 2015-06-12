#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include "config.h"
#include "Job.h"
#include "SimEngineTimer.h"
#include "TimeValues.h"
#include <list>
#include "ClaimRelease.h"
#include "Entity.h"
#include <functional>

using namespace std;

namespace EnergySim
{
	class ENERGYSIM_DLL_PUBLIC StringLookUp
	{
	public:
		StringLookUp() { startValue = 100; }
		long getNB(string s)
		{
			map <string, long >::iterator it = aMap.find(s);
			if (it == aMap.end())
			{
				startValue++;
				aMap.insert(pair<string, long>(s, startValue));
				return startValue;
			}
			return ( (*it).second   );
		};

		string getString(long value) 
		{
			for (pair<string, long> a : aMap)
				if (a.second == value)
					return a.first;
			return "";
		}
	private:
		long startValue;
		map < string, long > aMap ;
	};
	template<class T> class PlaceHolder
	{
		friend class StringLookUp;
	public:
		PlaceHolder(long theID){ itsID = theID; }
		PlaceHolder(string theName){ itsName = theName; }
		T* get()
		{
			if (itsObject != nullptr)
				return itsObject;
	};
	protected:
		static StringLookUp* itsLookUp;
	private:
		T* itsObject;
		long itsID;
		string itsName;
	};
	class SetterHandler;
	class ENERGYSIM_DLL_PUBLIC ISetter
	{
	public:
		virtual void setValue(double value){};
	};
	template<typename T> class ENERGYSIM_DLL_PUBLIC RealSetter : public ISetter
	{
		public:
			RealSetter(std::function<void(T)> theFunc) { itsFunc = theFunc; }
			void setValue(double value){ itsFunc(value); };
		private:
			std::function<void(T)> itsFunc;
	};
	class ENERGYSIM_DLL_PUBLIC MyRealSetter : public ISetter
	{
	public:
		MyRealSetter(std::function<void(double)> theFunc) { itsFunc = theFunc; }
		void setValue(double value){ itsFunc(value); };
	private:
		std::function<void(double)> itsFunc;
	};
	class ENERGYSIM_DLL_PUBLIC MyA
	{
	public:
		MyA(SetterHandler* aSS);
		void setMyInt(int aI) { i = aI; }
		int i;
	};
	class ENERGYSIM_DLL_PUBLIC MyB
	{
	public:
		MyB(SetterHandler* aSS);
		void setInt(int aI) { i = aI; }
		void setBool(bool aB) { b = aB; }
		void setDouble(double ad) { d = ad; }
		void setLong(long aL) { l = aL; }
		int i;
		bool b;
		double d;
		long l;
	};
	class ENERGYSIM_DLL_PUBLIC MyC
	{
	public:
		MyC(SetterHandler* aSS);
		void setInt(int aI, int bI) { i = aI + bI; }
		int i;

	};

	class ENERGYSIM_DLL_PUBLIC SetterHandler
	{
	public:

		SetterHandler();
		void store(list<long> keys, ISetter* theBS);
		ISetter* get(list<long> keys);

		long getNB(string s)
		{
			map <string, long >::iterator it = aMap.find(s);
			if (it == aMap.end())
			{
				startValue++;
				aMap.insert(pair<string, long>(s, startValue));
				return startValue;
			}
			return ((*it).second);
		};
		string getString() { return ""; }

		void set()
		{
			for (auto p : bMap)
			{
				p.second->setValue(false);
			}
		}

		void store(std::string name, EnergySim::ISetter* setter)
		{
			manipulatorMap.insert(make_pair(name, setter));
		}
		void call(string name, double value)
		{
			auto p = manipulatorMap.find(name);
			if (p != manipulatorMap.end())
				p->second->setValue(value);
		}
		void wakeUp(double time);
		
		map <long, ISetter*> bMap;
		static SetterHandler* getSetterHandler();
		void triggerSetting(SimModel* model);
	private:
		map <string, ISetter*> manipulatorMap;
		map <pair<string, double>, double*> callMap;
		

		list<pair<list<long>, ISetter*>> keysAndSetters;
		long startValue;
		map < string, long > aMap;
	};
}