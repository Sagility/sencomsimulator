#include "stdafx.h"
#include "EnergySim.h"
#include "SimEngine.h"
#include "SimLogEnvironment.h"

namespace EnergySim { 

	IEnvironment* IEnvironment::CurrentEnvironment=NULL;
	SimLogEnvironment::SimLogEnvironment()
	{
		_defaultfilename = "c:\\tmp\\log\\LOG"+ DateTime::currentDateTime() + ".txt";
		_use_file=false;
	}
	void SimLogEnvironment::Pause()
	{
		_running = false;
	}

	void SimLogEnvironment::Run()
	{
		_running = true;
	}

	void SimLogEnvironment::DebugLog(string msg)
	{
		if (_debug)
		{
		//	cout << msg << endl;
			if(_use_file)
			{
				write2file(msg);
			}
		}
	}
	void SimLogEnvironment::Log(string msg)
	{
		cout << msg << endl;
		if(_use_file)
		{
			write2file(msg);
		}
	}

	void SimLogEnvironment::ErrorLog(string msg)
	{
		cout << msg << endl;
		cerr << msg << endl;
		if(_use_file)
		{
			write2file(msg);
		}
	}
	void SimLogEnvironment::set_use_file(bool use_file) 
	{ 
		if(_use_file==use_file) return;
		_use_file= use_file;
		if(_use_file==false){
			if(_file.is_open()){
				_file.close();
			}
		}else{
			openfile();
		}
	}
	void SimLogEnvironment::openfile()
	{
		_file.open(_defaultfilename.c_str(), std::ios::out);		
	}
	void SimLogEnvironment::write2file(string msg)
	{
		if(_file.is_open()){
			_file << msg << endl;
			_file.flush();
		}
	}
	SimLogEnvironment::~SimLogEnvironment(){
		if(_file.is_open())_file.close();
	}

}
