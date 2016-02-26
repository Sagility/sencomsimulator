#include "Stdafx.h"
#include "ComplexResources.h"
#include "SimModel.h"
#include <functional>
#include <sstream>
#include <string>

namespace EnergySim
{
	void writeMyLog(string s)
	{
		return;
		ofstream myfile;
		myfile.open("mytestlogAAA.txt",std::ios::app);
		myfile << s;
		myfile << "\n";
		myfile.close();
		return;
	}
	//class ENERGYSIM_DLL_PUBLIC FunctionJob : public IJob
	//{
	//public:
	//	//constructor
	//	FunctionJob(SimContext *context, std::function<void()> theFP) :IJob(context)
	//	{
	//		fp = theFP;
	//	}
	//	~FunctionJob()
	//	{
	//	}
	//	virtual string ToString()
	//	{
	//		return "FunctionJob\n";
	//	}
	//	virtual void Execute() 
	//	{ 
	//		fp(); 
	//	};
	//	virtual string classname() { return "FunctionJob"; }
	//private:
	//	std::function<void()> fp;
	//};
	//class TwoFunctions
	//{
	//public:
	//	TwoFunctions(std::function<void()> theFirst, std::function<void()> theSecond)
	//	{
	//		a = theFirst;
	//		b = theSecond;
	//	}
	//	void execute()
	//	{
	//		a();
	//		b();
	//	}
	//private:
	//	std::function<void()> a;
	//	std::function<void()> b;
	//};
	//void delayAndDoFunction(std::function<void()> theFP, SimModel* theModel, double theWait)
	//{
	//	if (theWait < 0.01)
	//		theWait = 0.000001;
	//	CombinedJobController* aCJC = new CombinedJobController(theModel->context(), "functionDoer");
	//	DelayJob* aDJob = new DelayJob(theModel->context(), theWait);
	//	aCJC->AddJob(aDJob);
	//	aCJC->AddJob(new FunctionDoJob(theModel->context(), theFP));
	//	theModel->context()->engine()->ScheduleJobNow(aCJC);
	//}
	void PackingMachine::donePacking(double amountPacked)
	{
		currentSilo->currentPacker = NULL;
		currentSilo->outputBeingUsed = false;
		Flow::updateAll();
		itsFlow->onoff(false);
		if (currentSilo->itsStateValue->currentValue < 10)
		{
			currentSilo->use(false);
			currentSilo->clean();
		}
		currentSilo = NULL;
		if (this->type == "Bag")
			currentOrder->_DonePelletBags +=amountPacked;
		if (this->type == "Bulk")
			currentOrder->_DonePowderBags += amountPacked;
		if (this->type == "BigBag")
			currentOrder->_DonePelletBulk += amountPacked;
		currentOrder = NULL;
		lookForWork();
	}
	void PackingMachine::lookForWork()
	{
		if (itsPlant->done)
			return;
		bool found = false;
		double time = 3600;
		bool plantDone = false;
		double remainingGoods = 0;
		double currentTime = itsPlant->itsModel->context()->engine()->simulated_time();
		if (itsPlant->itsModel->context()->engine()->simulated_time() > 1000)
			plantDone = true;
		for each (PlantLine*  pl in itsPlant->itsLines)
		{
			for each (SiloOrder* so in pl->itsOrders)
			{
				if (!so->started)
				{
					plantDone = false;
					continue;					
				}
				if (this->type == "Bag")
					remainingGoods = so->_TotalPelletBags - so->_DonePelletBags;
				if (this->type == "BigBag")
					remainingGoods = so->_TotalPowderBags - so->_DonePowderBags;
				if (this->type == "Bulk")
					remainingGoods = so->_TotalPelletBulk - so->_DonePelletBulk;
				if (remainingGoods < 0.1)
					continue;
				Silo* aSilo = NULL;
				for each (Silo* s in so->usedSilos)
				{
					if (s->outputBeingUsed)
						continue;
					//if (s->empty())
						//continue;
					double sampleTime = so->sampleTime;
					if (s->startFillingTime + sampleTime > currentTime)
						continue;
					if (s->itsStateValue->currentValue < 10)
						continue;
					if (siloName.find(s->name) == std::string::npos) // Changed 150908
						continue;
					aSilo = s;
					break;
				}
				if (aSilo == NULL)
					continue;

				found = true;
				currentSilo = aSilo;
				currentOrder = so;
				goto foundIt;
								//time = so->_TotalPelletBulk / this->capacity;
								//time = time * 3600 * 1000;
								//time = time / this->itsPlant->itsFactor;
								//time = time * this->streamFactor / 100;
								//time = time * so->_density / 1000;
								//goto foundIt;
			}
		}
		if (plantDone)
		{
			itsPlant->done = true;
			return;
		}
		foundIt:
		if (!found)
		{
			std::function<void()> f_func2 = std::bind(&PackingMachine::lookForWork, this);
			if (itsPlant->itsModel->context()->engine()->simulated_time() < 100000000)
				EnergySim::delayAndDoJobFunction(f_func2, itsPlant->itsModel, 3600);
			return;
		}
		currentSilo->currentPacker = this;
		currentSilo->outputBeingUsed = true;
		used = true;
		double speed = capacity * 1000 /currentOrder->_density;
		speed = speed / 1000;
		speed = speed*itsPlant->itsFactor * streamFactor / 100;
		speed = speed / 3600;
		double amount = min(currentSilo->itsStateValue->currentValue, remainingGoods);
		time = amount / speed;
		double random = 100 * rand() / RAND_MAX;
		std::function<void()> f_func2 = std::bind(&PackingMachine::donePacking, this, amount);
		if (itsPlant->itsModel->context()->engine()->simulated_time() < 10000000)
			delayAndDoJobFunction(f_func2, itsPlant->itsModel, time + random);
		itsFlow = new Flow(&(currentSilo->itsStateValue->currentValue), time, itsPlant->itsModel);
		itsFlow->_speed = -speed;
		return;
	}
	void Silo::use(bool value)
	{
		if (value)
		{
			double max = itsStateValue->maxValue;
			itsStateValue = new StateValue(itsModel, 0);
			itsStateValue->maxValue = max;
		}
		if (value)
			_used = true;
		_inUse = value;
		if (!value)
			writeMyLog("Release " + this->name + " " + std::to_string(itsModel->context()->engine()->simulated_time()));
	}
	void SiloPlant::readSiloPlant(string theName)
	{
		name = theName;
		itsLines.push_back(new PlantLine(this->itsModel->context()->engine(), this,"1"));
		itsLines.push_back(new PlantLine(this->itsModel->context()->engine(), this,"2"));
		itsLines.push_back(new PlantLine(this->itsModel->context()->engine(), this,"3"));
		for each(PlantLine* p in itsLines)
			p->itsPlant = this;
		readLines(theName + "_05.txt");
		readSilos(theName + "_01.txt");
		readMachines(theName + "_02.txt");
		readOrders(theName + "_03.txt");
		readMatrix(theName + "_04.txt");
		
	}
	vector<string> split(const string& s, const string& delim, const bool keep_empty = true) {
		vector<string> result;
		if (delim.empty()) {
			result.push_back(s);
			return result;
		}
		string::const_iterator substart = s.begin(), subend;
		while (true) {
			subend = search(substart, s.end(), delim.begin(), delim.end());
			string temp(substart, subend);
			if (keep_empty || !temp.empty()) {
				result.push_back(temp);
			}
			if (subend == s.end()) {
				break;
			}
			substart = subend + delim.size();
		}
		return result;
	}
	void SiloPlant::readSilos(string fileName)
	{
		//
		std::string line;
		std::ifstream infile(fileName);
		while (std::getline(infile, line))
		{
			vector<string> str = split(line, ",", true);
			if (str.size() < 7)
				continue;

			int amount = stoi(str[1]);
			int size = stoi(str[2]);
			int density = stoi(str[3]);
			for (int i = 0; i < amount; i++)
			{
				Silo* aS = new Silo(size, itsModel);
				aS->name = str[0];
				aS->cleaningTime = itsCleaningTime;
				aS->density = density;
				//itsLines.  Add to correct lines
				PlantLine* aPL = NULL;
				if (str[4] == "True")
					if (aPL = getPlantLine(1))
						aPL->addSilo(aS);
				if (str[5] == "True")
					if (aPL = getPlantLine(2))
						aPL->addSilo(aS);
				if (str[6] == "True")
					if (aPL = getPlantLine(3))
						aPL->addSilo(aS);
				this->itsSilos.push_back(aS);
			}
		}
	}
	PlantLine* SiloPlant::getPlantLine(int theNB)
	{
		int count = 1;
		for each (PlantLine* var in itsLines)
		{
			if (count == theNB)
				return var;
			count++;
		}
		return NULL;
	}
	void SiloPlant::readMachines(string fileName)
	{
		std::string line;
		std::ifstream infile(fileName);
		while (std::getline(infile, line))
		{
			vector<string> str = split(line, ",", true);
			if (str.size() < 6)
				continue;

			int amount = stoi(str[1]);
			int capacity = stoi(str[2]);
			int factor = stoi(str[3]);
			for (int i = 0; i < amount; i++)
			{
				PackingMachine* aMS = new PackingMachine();
				aMS->itsPlant = this;
				aMS->name = str[0];
				aMS->capacity = capacity;
				aMS->streamFactor = factor;
				aMS->type = str[4];
				itsOutputMachines.push_back(aMS);
				aMS->siloName = str[5]; // Changed 150908

				aMS->lookForWork();
			}
		}	
	}
	void SiloPlant::readLines(string fileName)
	{
		std::string line;
		std::ifstream infile(fileName);
		int count = 0;
		while (std::getline(infile, line))
		{
			if (count==0)
				itsCleaningTime = stoi(line);
			if (count == 1)
				itsDownTime = stoi(line);
			if (count == 2)
				itsFactor = stoi(line);
			if (count == 3)
				itsHolidays = stoi(line);
			if (count == 4)
				itsMaintenance = stoi(line);
			if (count == 5)
				itsDaysPerWeek = stoi(line);
			if (count == 6)
				itsHoursPerDay = stoi(line);
			count++;
		}	

		double fullTime = 365 * 24;
		double offTime = (itsMaintenance + itsDownTime) * 12;
		double working = itsDaysPerWeek * 52;
		working -= itsHolidays;
		working *= itsHoursPerDay;
		working -= offTime;
		double aFactor = working / fullTime;
		itsFactor = aFactor;
	}
	void SiloPlant::readOrders(string fileName)
	{
		std::string line;
		std::ifstream infile(fileName);
		while (std::getline(infile, line))
		{
			vector<string> str = split(line, ",", true);
			if (str.size() < 12)
				continue;

			PlantLine* aPL = NULL;
			
			string lineName = str[0];
			//lineName = lineName.replace("", "");
			lineName.erase(lineName.begin() + 1, lineName.end());
			int line = stoi(lineName);
			aPL = getPlantLine(line);
			if (aPL == NULL)
				continue;
			SiloOrder* aO = new SiloOrder();
			aO->_speed = stoi(str[1]);
			aO->_time = stoi(str[2]);
			aO->_density = stoi(str[3]);
			aO->_total = aO->_speed*aO->_time * 24 * 1000 / aO->_density;
			aO->_speed = aO->_speed / (3600);
			aO->_speed = aO->_speed / aO->_density;
			aO->_speed *= 1000;
			aO->cleanComp =str[4];
			aO->_PercentPelletBags = stoi(str[9]);
			aO->_PercentPelletBulk = stoi(str[11]);
			aO->_PercentPowderBags = stoi(str[10]);
			aO->_TotalPelletBags = aO->_PercentPelletBags*aO->_total/100;
			aO->_TotalPelletBulk = aO->_PercentPelletBulk *aO->_total/100;
			aO->_TotalPowderBags = aO->_PercentPowderBags *aO->_total/100;
			aO->_grade = str[8];
			aO->samplePercentage = stoi(str[6]);
			aO->sampleTime = stoi(str[7])*3600;
			aPL->addOrder(aO);
		}
	}
	void SiloPlant::readMatrix(string fileName)
	{
		std::string line;
		std::ifstream infile(fileName);
		while (std::getline(infile, line))
		{
			vector<string> str = split(line, ",", true);
			if (str.size() < 3)
				continue;
			pair<string, string> p = pair<string, string>(str[0], str[1]);
			if (str[2]=="OK")
				gradeMatrix->insert(make_pair(p, GradeMixture::OK));
			if (str[2] == "OKClean")
				gradeMatrix->insert(make_pair(p, GradeMixture::OK_IF_CLEANED));
			if (str[2] == "NOK")
				gradeMatrix->insert(make_pair(p, GradeMixture::NOK));
		}	
	}
	void SiloPlant::writeResult(string fileName, bool result)
	{
		ofstream myfile;
		myfile.open(fileName);
		if (result)
			myfile << "SUCCESS TRUE\n";
		else
		{
			myfile << "SUCCESS FALSE\n";
		}

		//myfile.close();
		//return;
	//	myfile << this->itsModel->context()->engine()->simulated_time()  << " was the time\n";
		//myfile << callCount	<< " calls\n";
		
		map<string, int*> aMap = map<string, int*>();
		map<string, int*>::iterator aIt;
		for each (Silo* var in itsSilos)
		{
			if (!var->_used)
				continue;
			aIt = aMap.find(var->name);
			if (aIt != aMap.end())
				(*aIt->second)++;
			else
				aMap.insert(make_pair(var->name, new int(1)));
		}
		for each (auto p in aMap)
			myfile << p.first << " " << *(p.second) << "\n";

		// Iterate machines
		map<string, int*> bMap = map<string, int*>();
		map<string, int*>::iterator bIt;
		for each (PackingMachine* var in itsOutputMachines)
		{
			if (!var->used)
				continue;
			bIt = bMap.find(var->name);
			if (bIt != bMap.end())
				(*bIt->second)++;
			else
				bMap.insert(make_pair(var->name, new int(1)));
		}
		for each (auto p in bMap)
			myfile << p.first << " " << *(p.second) << "\n";

		myfile.close();
	}
	void PlantLine::processNextOrderUpdate(int theCounter)
	{
		itsPlant->callCount++;
		if (theCounter < itsNextDoneUpdateCounter)
			return;
		itsNextDoneUpdateCounter = theCounter;
		processNextOrder();
	}
	void PlantLine::scheduleNextOrderUpdate(double theTime)
	{
		itsPlant->callCount++;
		if (theTime<1)
			theTime = 1;
		double now = this->itsEngine->simulated_time();
		double next = now + theTime;
		if (next + 0.01 > itsNextUpdateTime)
			if (itsNextUpdateTime > 0.01 + now)
				return;
		itsNextUpdateTime = next;
		std::function<void()> f_func2 = std::bind(&PlantLine::processNextOrderUpdate, this,itsNextUpdateCounter++);
		delayAndDoJobFunction(f_func2, itsPlant->itsModel, theTime);
	}
	void PlantLine::processNextOrder()
	{
		itsPlant->callCount++;
		// Too many updates are coming
		
		//for each (Silo* a in itsSilos)
		//	a->itsStateValue->updateState();
		Flow::updateAll();
		
		double aTime = itsPlant->itsModel->context()->engine()->simulated_time();
		if (itsCurrentOrder != NULL)
		{
			double aTotalTime = itsCurrentOrder->_total / itsCurrentOrder->_speed;
			std::function<void()> f_func2 = std::bind(&PlantLine::processNextOrder, this);
			double doneTime = aTime - itsCurrentOrder->fillStartTime;
			double remainingTime = aTotalTime - doneTime;
			if (remainingTime < 0.001)
			{// Order done
				itsCurrentSilo = NULL;
				itsCurrentOrder->done = true;
				itsCurrentOrder = NULL;
				if (itsCurrentFlow != NULL)
				{
					itsCurrentFlow->onoff(false);
					itsCurrentFlow->_StateValue->disConnect(itsCurrentFlow);
					itsCurrentFlow = NULL;
				}
				scheduleNextOrderUpdate(0);
				return;
			}
			double remaining = itsCurrentOrder->_total - itsCurrentOrder->_DoneIn;
			if (itsCurrentSilo != NULL)
			{
				double roomLeft = itsCurrentSilo->itsStateValue->maxValue - itsCurrentSilo->itsStateValue->currentValue;
				if (roomLeft > 0.01)
				{
					// Needs to take into account if someone is emptying at the same time
					double fullTime = roomLeft / itsCurrentSilo->itsStateValue->getSpeed();
					double time = min(remainingTime, fullTime);
					if (time < 1)
						time = 1;
					//delayAndDoFunction(f_func2, itsPlant->itsModel, time);
					scheduleNextOrderUpdate(time);
					return;
				}
				// Need to switch silos
				itsCurrentFlow->onoff(false);
				bool found = false;
				for each (Silo* a in itsSilos)
				{
					if ((a->used()))
						continue;
					itsCurrentSilo = a;
					found = true;
					break;
				}
				if (!found)
				{
					itsCurrentFlow = NULL;
					itsCurrentSilo = NULL;
					itsPlant->fail(this->name + "A");
					return;
				}
				itsCurrentFlow = itsCurrentSilo->startUsing(itsCurrentOrder->_grade, itsCurrentOrder);
				//double doAmount = min(remaining, itsCurrentSilo->itsStateValue->maxValue);
				double time = min(remainingTime, itsCurrentSilo->itsStateValue->maxValue / itsCurrentSilo->itsStateValue->getSpeed());
				scheduleNextOrderUpdate(time);
				return;
			}
			else
			{
				itsPlant->fail(this->name + "B");
			}
		}
		// Does it have a current order
		// If yes, is it fulfilled
		// If yes, close and rescedule this function now
		// if no, go ahead continue

		SiloOrder* aOrder = NULL;
    	// No started order, search for a new order
		for each(auto a in itsOrders)
		{
			if (a->started)
				continue;
			if (!a->done)
			{
				aOrder = a;
				break;
			}
		}
		if (aOrder == NULL)
			return finished();
		aOrder->started = true;
		itsCurrentOrder = aOrder;
		// PickSilo
		Silo* aSilo = NULL;
		if (aOrder->currentSilo == NULL)
		{
			bool needsCleaning = false;
			for each (Silo* a in itsSilos)
			{
				if (a->used())
					continue;
				if (!(a->empty()))
					continue;
				if (itsPlant->isOK(a->itsLastGrade, aOrder->_grade) == GradeMixture::OK)
				{
					aSilo = a;
					break;
				}
			}
			if (aSilo == NULL)
				itsPlant->fail(this->name + "C");
			aSilo->use(true);
			aSilo->itsLastGrade = aOrder->_grade;
		}
		else
		{
			aSilo = aOrder->currentSilo;  // Should not happen
		}
		aOrder->currentSilo = aSilo;
		itsCurrentSilo = aSilo;
		itsCurrentFlow= itsCurrentSilo->startUsing(itsCurrentOrder->_grade, itsCurrentOrder);
		//itsCurrentOrder->usedSilos.push_back(itsCurrentSilo);		
		// We have an order and an silo
		itsCurrentFlow->_speed = aOrder->_speed;
		scheduleNextOrderUpdate(0);
		return;
	}
	void SiloPlant::start()
	{
		for each(PlantLine* aPL in itsLines)
			aPL->processNextOrder();
	}
	void UpdateOrder::valueReached(StateValue* theST, double theValue, bool theSign){}
	void FinishOrder::valueReached(StateValue* theST, double theValue, bool theSign)
	{
		itsOrder->done = true;
		itsFlow->_speed = 0;
	}
	Silo::Silo(double size, SimModel* theModel)
	{
		itsModel = theModel;
		itsStateValue = new StateValue(itsModel, 0);
		itsStateValue->maxValue = size;
		itsGradeSpeed.insert(pair<string, double>("DEFAULT", 10));
	}
	StateValue::StateValue(SimModel* theModel, double theStartValue)
	{
		currentValue = theStartValue;
		itsModel = theModel;
		updateTime = itsModel->context()->engine()->simulated_time();
	}
	void StateValue::connect(Flow* theFlow)
	{
	//	updateState();
		itsFlows.push_back(theFlow);
		theFlow->_StateValue = this;
	//	updateSubscribtions();
	}
	void StateValue::disConnect(Flow* theFlow)
	{
		updateState();
		itsFlows.remove(theFlow);
		theFlow->_StateValue = NULL;
		updateSubscribtions();
	}
	void StateValue::updateSubscribtions()
	{
		if (itsFlows.size() < 1)
			return;
		// Now that we have flows, make sure to subscribe 
		double rate = 0;
		for each(Flow* aF in itsFlows)
			rate += aF->_speed;
		if (abs(rate) < 0.00001)
			return;
		double time = 10;
		if (rate > 0)
		{
			 time = (maxValue - currentValue) / rate;
		}
		else
		{
			 time = (currentValue - minValue) / rate;
		}
		if (time > 0.01)
		{
			std::function<void()> f_do = std::bind(&StateValue::updateState, this);
			delayAndDoJobFunction(f_do, itsModel, time);
		}
		for each(Flow* aF in itsFlows)
		{
			if (abs(aF->_speed) < 0.0001)
				continue;
			double left = aF->_goal - aF->_total;
			time = left / aF->_speed;
			std::function<void()> f_do = std::bind(&StateValue::updateState, this);
			delayAndDoJobFunction(f_do, itsModel, time);
		}
	}
	double 	StateValue::getSpeed()
	{
		double rate = 0;
		for each(Flow* aF in itsFlows)
		{
			if (aF->on)
				rate += aF->_speed;
		}
		return rate;
	}
	void StateValue::updateState()
	{
		// Update values
		double currentTime = itsModel->context()->engine()->simulated_time();
		if (abs(currentTime - updateTime) > 0.0001)
		{
			double rate = 0;
			for each(Flow* aF in itsFlows)
				rate += aF->update();
			currentDelta = currentTime - updateTime;
			currentValue += rate;
			updateTime = currentTime;
		}
		else
			return;
		// See if reached min or max and if so publish 
		if (currentValue - maxValue > -0.0001)
		{
			currentValue = maxValue;
			for each(pair<IStateValueMax*, void*> p in itsMaxListners)
				p.first->valueReached(this, p.second);
			itsMaxListners.clear();			
		}
		if (minValue-currentValue > 0.0001)
		{
			currentValue = minValue;
			for each(pair<IStateValueMin*, void*> p in itsMinListners)
				p.first->valueReached(this, p.second);
			itsMinListners.clear();			
		}
	}
	void Flow::onoff(bool onOff)
	{
		if (onOff)
			update();
		on = onOff;
		return;
		if (_StateValue == NULL)
			return;
		_StateValue->updateState();
		if (on)
			_speed = _realSpeed;
		else
			_speed = 0;
		_StateValue->updateSubscribtions();
	}
	double Flow::update()
	{
		double currentTime = itsModel->context()->engine()->simulated_time();
		if (!on)
		{
			_updateTime = currentTime;
			return 0;
		}
		double delta = currentTime - _updateTime;
		double deltaProduced = delta*_speed;
		_total += abs(deltaProduced);
		_updateTime = currentTime;
		//if (_total + 0.001 > _goal)
		//{
		//	for each (auto a in itsListners)
		//		a->flowFinished(this);
		//	itsListners.clear();
		//}
		double aD = *_ptrDouble;
		aD += deltaProduced;
		*_ptrDouble = aD;
		return deltaProduced;
	}
	list<Flow*> Flow::allFlows =  list<Flow*>();
	void Flow::updateAll()
	{
		for each(Flow* f in allFlows)
			f->update();
	}
	Flow* Silo::startUsing(string theGrade, SiloOrder* theOrder)
	{
		string s = std::to_string( itsModel->context()->engine()->simulated_time());
		writeMyLog(theGrade + " " + this->name + " " + s);
		theOrder->usedSilos.push_back(this);
		use(true);
		itsLastGrade = theGrade;
		startFillingTime = itsModel->context()->engine()->simulated_time();
		if (theOrder->fillStartTime < -0.5)
			theOrder->fillStartTime = itsModel->context()->engine()->simulated_time();
		Flow* aFlow = new Flow(&(itsStateValue->currentValue), itsModel->context()->engine()->simulated_time(), itsModel);
		aFlow->_speed = theOrder->_speed;
		itsStateValue->connect(aFlow);
		return aFlow;
	}
}