// SimLogEnvironment.h

#pragma once
#include "EnergySim.h"
#include <fstream>

using namespace std;

namespace EnergySim {
    class ENERGYSIM_DLL_PUBLIC IEnvironment
    {
	public:
		static IEnvironment *CurrentEnvironment;
	protected:
		bool _debug;
        bool _running;
        string _starttime;
        double _time_horizon;
		IEnvironment(){};
		virtual ~IEnvironment(){};
	public:
		virtual bool debug() const{return _debug;}
		virtual void set_debug(bool debug){ _debug = debug;};
		string starttime() const{return _starttime;};
		void starttime(string starttime) { _starttime=starttime;};
		double time_horizon() const{return _time_horizon;}
		void time_horizon(double time_horizon){_time_horizon=time_horizon;}

        virtual void DebugLog(string msg)=0;
        virtual void ErrorLog(string msg)=0;
		virtual bool running() const{return _running;};
        virtual void Log(string msg)=0;
        virtual void Pause()=0;
        virtual void Run()=0;
    };

	class ENERGYSIM_DLL_PUBLIC SimLogEnvironment:public IEnvironment{
	private:
	//	 TextWriter tw = null;
        string _defaultfilename;
		bool _use_file;
		SimLogEnvironment();
	protected:
		fstream _file;
		void openfile();
		void write2file(string message);
	public:
		static IEnvironment* CreateEnvironment()
        {
            if(CurrentEnvironment == NULL) CurrentEnvironment = new SimLogEnvironment();
            return CurrentEnvironment;
        }
		virtual ~SimLogEnvironment();
		virtual void DebugLog(string msg);
        virtual void ErrorLog(string msg);
        virtual void Log(string msg);
        virtual void Pause();
        virtual void Run();
		bool use_file() const {return _use_file;};
		void set_use_file(bool use_file);
	};
};
