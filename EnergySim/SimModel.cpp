#include "stdafx.h"
#include "SimModel.h"
#include "CRResource.h"
#include <regex>

#include <fstream>

/*
*/

namespace EnergySim
{
	SimModel* EnergySim::aM = NULL;
	SimModel::SimModel()
	{
		reader.itsValue = new TextReader("a");
		reader.itsValue->model = this;
		env = (SimLogEnvironment*) SimLogEnvironment::CreateEnvironment();
		env->set_use_file(true);
		ISimEngine *engine = new RealSimEngine(env); //This engine runs in real time
		//ISimEngine *engine = new SimEngine(env);
		_ctx = new SimContext();
		_ctx->set_engine(engine);
		itsParser.itsValue = IParser::createParser(new AttributeHandler());
		_ctx->engine()->AddEngineFinishListener(this);
		_ctx->aModel = this;
		reader.itsValue->model = this;
//		IEvent::itsClock = new Clock(_ctx->engine());
		itsResHandler = new ResourceHandler(this);
		IEvent::itsClock = new Clock(_ctx->engine());
	}

	void SimModel::finish()
	{
		IEvent::publishEvent(EventType::ET_SIMULATION_END, 0, vector<long>(), _ctx->engine()->simulated_time());
		IEvent::flush();
	}

	void SimModel::setRes()
	{ 
		aResID = 10; 
		myFunc(preReqs.itsValue, preReqs);
	}
	void setAValue(SimModel* aModel)
	{
		getSet<float, SimModel> aResID = aModel->aResID;
		aResID = 115;
	}

	bool checkExists(string file)
	{
		ifstream file_to_check(file.c_str());
		if (file_to_check.is_open())
			return true;
		return false;
		file_to_check.close();
	}

	bool TextReader::openFile(string fileName)
	{
		tokens.clear();
		
		if (!checkExists(fileName))
			return false;
		infile = std::ifstream(fileName);	
		if (infile.bad())
			return false;
		if (!infile.is_open())
			return false;
		return true;
	}
	void TextReader::closeFile()
	{
		infile.close();
		tokens.clear();
	}
	bool TextReader::next()
	{
		string s;
		if (std::getline(infile, s))
		{
			tokens.clear();
			/*
			std::regex words_regex("[^\\s]+");
			auto words_begin =
				std::sregex_iterator(s.begin(), s.end(), words_regex);
			auto words_end = std::sregex_iterator();

			int length = std::distance(words_begin, words_end);
			for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
				std::smatch match = *i;
				std::string match_str = match.str();
				tokens.push_back(match_str);
			}
			*/
			stringstream ss(s); // Insert the string into a stream
			string buf;

			while (ss >> buf)
				tokens.push_back(buf);


			if (tokens.size() < 1)
				return false;
			return true;
		}
		return false;
	}
	void TextReader::readResources()
	{
		openFile(itsBaseName + ".res");
		while (next())
		{
				Resource* aRes = new Resource(new Clock(model->context()->engine()), tokens.front());
				/*TODO  ADD Resname*/   //  aRes->eID() = stoi(tokens.front(), NULL);
				tokens.pop_front();
				int cap = stoi(tokens.front(), NULL);
				double d = rand();
				d = d / RAND_MAX;	// RANDOM STUFF 
				
				d = 1.05 - d;
			//	cap = cap*d;
				aRes->setCapacity(cap);
				model->resources.itsValue.push_back(aRes);
		}
		closeFile();
	}
	void TextReader::readRoutes()
	{
		string name = itsBaseName;
		for (int i = 0; i < 501; i++)
		{
			name = itsBaseName + to_string(i);
			if (!openFile(name + ".route"))
				continue;
			Route* aR = new Route();
			aR->name = name;
			StepReader aSR;
			aSR.itsCtx = model->context();
			aSR.itsHandler = model->itsParser.itsValue->getAttributeHandler();
			aSR.itsSchedule = model->inSchedule.itsValue;
			aSR.model = model;
			while (next())
			{
				string value = "";
				for (string s : tokens)
					value += s + " ";
				if(tokens.size()>0)
					if (tokens.front().find("*") ==0)
						break;
				aR->addStep(aSR.readLine(value));
			}
			closeFile();
			model->addRoute(aR);
		}
	}
	void TextReader::readAttributes()
	{
		openFile(itsBaseName + ".att");
		while (next())
		{
			string name = tokens.front();
			tokens.pop_front();
			double startValue = std::stod(tokens.front());
			tokens.pop_front(); // Start value
			string value = "";
			for (string s : tokens)
				value += s + " ";
			model->itsParser.itsValue->addAttribute(value, name, startValue);
		}
		model->itsParser.itsValue->getAttributeHandler()->registerForUpdates(model->context()->engine());
		closeFile();
		model->itsParser.itsValue->getAttributeHandler()->replace("simdelta", new ConstantValue(0));
		//model->itsParser.itsValue->getAttributeHandler()->updateAllAttributes();
		model->itsParser.itsValue->getAttributeHandler()->reportAllAttributes(0);
	}
	void TextReader::readSchedule()
	{
		Schedule* s = new Schedule("Schedulino",*(model->context()));
		model->inSchedule.itsValue = s;

		openFile(itsBaseName + ".sched");
		while (next())
		{
			long i1, i2, i3, i4, i5;
			i1 = stoi(tokens.front(), NULL);
			tokens.pop_front();
			i2 = stoi(tokens.front(), NULL);
			tokens.pop_front();
			i3 = stoi(tokens.front(), NULL);
			tokens.pop_front();
			i4 = stoi(tokens.front(), NULL);
			tokens.pop_front();

			list<long> listOfRes;
			while (tokens.size() > 0)
			{
				i5 = stoi(tokens.front(), NULL);
				listOfRes.push_back(i5);
				tokens.pop_front();
			}
			s->addJob(i1, i2, i3, i4, listOfRes);
		}
		closeFile();
		s->sort();
	}
	void TextReader::readpreRequisits()
	{
		while (next())
		{

		}
	}

	void TextReader::readOrders()
	{
		openFile(itsBaseName + ".order");
		while (next())
		{
			Order* aO = new Order();
			aO->Route = tokens.front();
			tokens.pop_front();
			// FIX aO->ID = stoi(tokens.front(), NULL);
			// FIX tokens.pop_front();
			// FIX aO->Time = stoi(tokens.front(), NULL);
			// FIX	tokens.pop_front();
			aO->count = stoi(tokens.front(), NULL);

			// aO->Time = 0; // RANDOM STUFF
			tokens.pop_front();
			model->orders.itsValue.push_back(aO);
		}
		closeFile();
		model->inSchedule.itsValue->filterOutConcreteProcess();
	}

	void writeLogg(string s)
	{
		return;
		ofstream myfile;
		myfile.open("debuglog.txt", std::ios::app);
		myfile << s;
		myfile << "\n";
		myfile.close();
		return;
	}

	void SimModel::runModel()
	{
	//	IEvent::publishEvent(EventType::ET_SIMULATION_START, 0, vector<long>(), _ctx->engine()->simulated_time());
	//	IEvent::publishEvent(EventType::ET_SIMULATION_START, vector<string>(3) = { "A", "B", "C" });
		list<CombinedJobController*> *aController = new list<CombinedJobController*>();

//		SetterHandler::getSetterHandler()->triggerSetting(this);

		CombinedJobController* aControl = new CombinedJobController(context(),"Control");

		aController->push_back(aControl);
		RealSimEngine* aRSE = (RealSimEngine*) this->context()->engine();
		aControl->AddJob(new ControlJob(context(), 30, aControl, aRSE->itsOR));
		_ctx->engine()->ScheduleJobNow(aControl);

		double delay = 10.001;

		for (Order *o : orders.itsValue)
		{
			for (int i = 0; i < o->count ; i++)
			{
				writeLogg(o->Route + " "+ to_string(i));
				char buffer[33];
				CombinedJobController* aCJC = new CombinedJobController(context(), std::to_string(o->ID) +"_" + itoa(i,buffer,10));
				for (Route *r : routes.itsValue)
				{
					o->Time = 0;
					if (r->name != o->Route)
						continue;
					r->createThisJobOrder(NULL, o, *aCJC);
					if (i == o->count - 1)
					{
						int line = 666;
						for (auto j : aCJC->_jobqueue)
						{
							WaitForResourcesJob* aWFRJ = dynamic_cast<WaitForResourcesJob*> (j);
							if (aWFRJ != NULL)
							{
								line = stoi(aWFRJ->itsResReq->itsAlternates.front()->need->front().second->name());
								break;
							}
						}
						if (line < 4)
						{
							aCJC->AddJob(new EndOrderJob(line, this));
							if (line==1)
								aCJC->AddJob(new SetAttributeJob(this->itsParser.itsValue->getAttributeHandler(), "resmodeone" + to_string(line), 0, context()));
							if (line == 2)
								aCJC->AddJob(new SetAttributeJob(this->itsParser.itsValue->getAttributeHandler(), "resmodetwo" + to_string(line), 0, context()));
							if (line == 3)
								aCJC->AddJob(new SetAttributeJob(this->itsParser.itsValue->getAttributeHandler(), "resmodethree" + to_string(line), 0, context()));
						}
					}
					aController->push_back(aCJC);
					// FIX add turn off resource job as set attribute
					//FIX _ctx->engine()->ScheduleJobNow(aCJC);
					writeLogg(o->Route + " "+ to_string(i) + " : " + to_string(delay));
					_ctx->engine()->ScheduleJobAt(aCJC, delay, 0);
					delay += 0.001;
					//_ctx->engine()->ScheduleJobAtFront(aCJC);
					break;
				}
			}
		}
		_ctx->engine()->Run();
	}
}