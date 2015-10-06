// EnergySimtest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<string>

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

//#include "ComplexResources.h"
#include "Counter.h"

#include "xercesc\parsers\XercesDOMParser.hpp"
#include "xercesc\dom\DOM.hpp"
#include "xercesc\sax\HandlerBase.hpp"
#include "xercesc\util\XMLString.hpp"
#include "xercesc\util\PlatformUtils.hpp"
#include<iostream>

#include "SemconInterface.h"

using namespace SemconInterface;
using namespace std;
using namespace XERCES_CPP_NAMESPACE;
using namespace EnergySim;

namespace SemconInterface
{
	void writeLog(int i)
	{
		//ofstream myfile;
		//myfile.open("mytestlog.txt", std::ios::app);
		//myfile << i;
		//myfile << "\n";
		//myfile.close();
		return;
	}
	class MyErrorHandler : public HandlerBase
	{
	public:
		list<string*> errors = list<string*>();
		void error(const SAXParseException &exc)
		{
			int i = (int)exc.getLineNumber();
			string* s1 = new string(to_string(i));

			wstring ws(exc.getMessage());
			string test(ws.begin(), ws.end());

			string* s2 = new string(test); //wchar_t
			errors.push_back(s1);
			errors.push_back(s2);
			//exit(-1);
		}
	};

	// DOMNodeList
	string findValue(DOMNode* theNode, string attribute);
	string findValueIntree(DOMNode* theNode, string attribute)
	{
		DOMNodeList* aDNL = theNode->getChildNodes();
		{
			for (int i = 0; i < aDNL->getLength(); i++)
			{
				DOMNode* aNode = aDNL->item(i);
				string aRet = findValue(aNode, attribute);
				if (aRet != "")
					return aRet;
			}
		}
		return "";
	}
	DOMNode* findNode(DOMNode* theNode, string attribute)
	{
		string theNodeName = XMLString::transcode(theNode->getNodeName());
		if (theNodeName == attribute)
			return theNode;

		DOMNodeList* aDNL = theNode->getChildNodes();
		{
			for (int i = 0; i < aDNL->getLength(); i++)
			{
				DOMNode* aNode = aDNL->item(i);
				string theChildName = XMLString::transcode(aNode->getNodeName());
				if (theChildName == attribute)
					return aNode;
				aNode = findNode(aNode, attribute);
				if (aNode != NULL)
					return aNode;
			}
		}
		return NULL;
	}
	void findNodes(DOMNode* theNode, string attribute, list<DOMNode*>* aList)
	{
		string theNodeName = XMLString::transcode(theNode->getNodeName());
		if (theNodeName == attribute)
			aList->push_back(theNode);

		DOMNodeList* aDNL = theNode->getChildNodes();
		{
			for (int i = 0; i < aDNL->getLength(); i++)
			{
				DOMNode* aNode = aDNL->item(i);
				string theChildName = XMLString::transcode(aNode->getNodeName());
				if (theChildName == attribute)
					aList->push_back(theNode);
				findNodes(aNode, attribute, aList);
			}
		}
		return;
	}
	string findValue(DOMNode* theNode, string attribute)
	{
		XMLCh* temp = XMLString::transcode(attribute.c_str());
		string theNodeName = XMLString::transcode(theNode->getNodeName());
		if (theNode->getNodeType() != DOMNode::NodeType::ELEMENT_NODE)
			return "";

		DOMElement* aE = (DOMElement*)theNode;
		DOMNodeList* aDNL = aE->getElementsByTagName(temp);
		if (attribute == XMLString::transcode(theNode->getNodeName()))
		{
			DOMNode::NodeType type = theNode->getNodeType();
			DOMNamedNodeMap* aNM = theNode->getAttributes();
			XMLSize_t tmp2 = aNM->getLength();
			const XMLCh* tmp1 = theNode->getNodeValue();
			DOMNode* bNode = theNode->getFirstChild();
			const XMLCh* btmp1 = bNode->getNodeValue();
			return XMLString::transcode(btmp1);
		}
		for (int i = 0; i < aDNL->getLength(); i++)
		{
			DOMNode* aNode = aDNL->item(i);

			string ret = XMLString::transcode(aNode->getNodeName());
			if (ret == attribute)
			{
				DOMNode* bNode = aNode->getFirstChild();
				DOMNode::NodeType btype = bNode->getNodeType();
				string bName = XMLString::transcode(bNode->getNodeName());
				const XMLCh* btmp1 = bNode->getNodeValue();


				DOMNode::NodeType type = aNode->getNodeType();
				DOMNamedNodeMap* aNM = aNode->getAttributes();
				XMLSize_t tmp2 = aNM->getLength();
				const XMLCh* tmp1 = aNode->getNodeValue();
				ret = XMLString::transcode(btmp1);
				return ret;
			}
		}
		return "";
	}
	void findScheduledJob(DOMDocument* theDoc)
	{
		DOMNode* aN = findNode(theDoc, "resourceRequirement");
		list<DOMNode*>* myList = new list<DOMNode*>();
		findNodes(theDoc, "resourceRequirement", myList);

		int gh = 0;
		string aS = "";
		for each (DOMNode* aN in *myList)
		{
			string ret = findValue(aN, "capacity");
			aS += ret + " ";
			if (ret == "5")
				gh++;
		}

		return;


		XMLCh* temp = XMLString::transcode("utilization");
		DOMNodeList* aDNL = theDoc->getElementsByTagName(temp);

		XMLSize_t aCount = aDNL->getLength();

		string aDesc = "";

		list<SchedJob*>* aL = new list<SchedJob*>();

		/*
		Combine schedjob and chain list.
		*/

		while (aCount > 0)
		{
			SchedJob* aSJ = new SchedJob();
			aL->push_back(aSJ);
			DOMNode* aDN = aDNL->item(aCount - 1);

			aSJ->start = findValueIntree(aDN, "fromDate");
			aSJ->end = findValueIntree(aDN, "toDate");
			aSJ->orderName = findValueIntree(aDN, "orderName");
			aSJ->name = findValueIntree(aDN, "jobName");
			aSJ->mode = findValueIntree(aDN, "mode");
			aCount--;
		}
		return;
	}
	// Find orders
	// Find resources
	// Find jobs
	void combine(list<SchedJob*>* aL, list<SchedJob*>* bL)
	{
		for each (SchedJob* aSJ in *aL)
		{
			for each (SchedJob* bSJ in *bL)
			{
				if (aSJ->name != bSJ->name)
					continue;
				if (aSJ->orderName != bSJ->orderName)
					continue;
				//aSJ->resourceUse = bSJ->resourceUse;
				//bSJ->resourceUse = NULL;
				break;
			}
		}
	}
	void findJobDescription(DOMDocument* theDoc)
	{
		XMLCh* temp = XMLString::transcode("utilization");
		DOMNodeList* aDNL = theDoc->getElementsByTagName(temp);
		XMLSize_t aCount = aDNL->getLength();
		while (aCount > 0)
		{
			DOMNode* aDN = aDNL->item(aCount - 1);
			aCount--;
		}
	}
	// Search for jobs in the original document
	// For each job, grap all modes
	void findValueInDoc(DOMDocument* theDoc, string attribute)
	{
		XMLCh* temp = XMLString::transcode(attribute.c_str());
		DOMNodeList* aDNL = theDoc->getElementsByTagName(temp);
		XMLSize_t aCount = aDNL->getLength();

		string orderName = "";

		list<SchedJob*>* aL = new list<SchedJob*>();
		while (aCount > 0)
		{
			DOMNode* aDN = aDNL->item(aCount - 1);
			SchedJob* aSJ = new SchedJob();
			aL->push_back(aSJ);

			aSJ->orderName = orderName;
			aSJ->name = findValueIntree(aDN, "name");

			aSJ->mode = findValueIntree(aDN, "mode");
			aCount--;
		}
	}


	/* 
	Search for this tag TAG
	For each	
		create an object according to following function
		set value in object to TAG value


		Node* , string TAG, Object creator*, object member*

		string*
	*/
	template<typename MemberType, class T>
	void* getTree(DOMNode* node, string tag, MemberType T::* theValue) //T* (ObjectCreator)(),
	{
		 
		XMLCh* temp = XMLString::transcode(tag.c_str()); 
		list<DOMNode*>* aList = new list<DOMNode*>();

		list<T*>* aRet = new list<T*>();

		findNodes(node, tag,  aList);
		for each (DOMNode* nr in *aList)
		{
			T* aPtr = new T();
			aPtr->*theValue =45;
			aRet->push_back(aPtr);

		}

		//DOMNodeList* aDNL = node->getElementsByTagName(temp);
		return aRet;
	}

	class TestClass01
	{
	public:
		TestClass01(){};
		int itsStr;

	};

	SenComModel* SenComModel::getModel(string definitionFileName, string schedFilename)
	{
		SenComModel* aM = new SenComModel();

		try {
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch) {

			cout << "Error during initialization! :\n";
			return aM;
		}

		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Never);
		parser->setDoNamespaces(true);    // optional
		ErrorHandler* errHandler = (ErrorHandler*) new MyErrorHandler();
		parser->setErrorHandler(errHandler);


		char* xmlFile = "output_A1.xml";
		//xmlFile = "A1.xml";
		//char* xmlFile = "employees.xml";

		try {
			parser->parse(xmlFile);
		}
		catch (...) {
			cout << "Exception message is: \n";
			return aM;
		}

		/*
		Ta ut resurserna.
		*/

		DOMDocument* aDoc = parser->adoptDocument();


		DOMNode* aN = findNode(aDoc, "resourceSchedules");
		string b;
		void* aPtr = getTree<int, TestClass01>(aN, "utilization",  &TestClass01::itsStr);

		XMLCh* temp = XMLString::transcode("resourceSchedule"); // First schedule level
		DOMNodeList* aDNL = aDoc->getElementsByTagName(temp);

		string attribute = "resourceName";
		XMLSize_t aCount = aDNL->getLength();
		XMLSize_t aSize = aCount;
		while (aCount > 0)
		{
			DOMNode* aDN = aDNL->item(aSize - aCount);
			string aName = findValue(aDN,attribute);
			aCount--;
		}
		

		//findScheduledJob(aDoc);

		return aM;
	}


	void SenComModel::geResources(string definitionFileName, string schedFilename)
	{
		SenComModel* aM = new SenComModel();

		try {
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch) {

			cout << "Error during initialization! :\n";
			return;
		}

		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Never);
		parser->setDoNamespaces(true);    // optional
		ErrorHandler* errHandler = (ErrorHandler*) new MyErrorHandler();
		parser->setErrorHandler(errHandler);


		char* xmlFile = "A1.xml";
		//xmlFile = "A1.xml";
		//char* xmlFile = "employees.xml";

		try {
			parser->parse(xmlFile);
		}
		catch (...) {
			cout << "Exception message is: \n";
			return;
		}
		DOMDocument* aDoc = parser->adoptDocument();
		DOMNode* aN = findNode(aDoc, "resources");
		string b;

		XMLCh* temp = XMLString::transcode("resource"); // First schedule level
		DOMNodeList* aDNL = aDoc->getElementsByTagName(temp);

		string attribute1 = "name";
		string attribute2 = "capacity";
		XMLSize_t aCount = aDNL->getLength();
		XMLSize_t aSize = aCount;
		while (aCount > 0)
		{
			DOMNode* aDN = aDNL->item(aSize - aCount);
			string aName = findValue(aDN, attribute1);
			string aCap = findValue(aDN, attribute2);
			aCount--;
		}
		//findScheduledJob(aDoc);
	}
	void SenComModel::getJobs(string definitionFileName, string schedFilename)
	{
		SenComModel* aM = new SenComModel();

		try {
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch) {

			cout << "Error during initialization! :\n";
			return;
		}

		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Never);
		parser->setDoNamespaces(true);    // optional
		ErrorHandler* errHandler = (ErrorHandler*) new MyErrorHandler();
		parser->setErrorHandler(errHandler);


		char* xmlFile = "A1.xml";
		//xmlFile = "A1.xml";
		//char* xmlFile = "employees.xml";

		try {
			parser->parse(xmlFile);
		}
		catch (...) {
			cout << "Exception message is: \n";
			return;
		}
		DOMDocument* aDoc = parser->adoptDocument();
		DOMNode* aN = findNode(aDoc, "resources");
		string b;

		XMLCh* temp = XMLString::transcode("resource"); // First schedule level
		DOMNodeList* aDNL = aDoc->getElementsByTagName(temp);

		string attribute1 = "name";
		string attribute2 = "capacity";
		XMLSize_t aCount = aDNL->getLength();
		XMLSize_t aSize = aCount;
		while (aCount > 0)
		{
			DOMNode* aDN = aDNL->item(aSize - aCount);
			string aName = findValue(aDN, attribute1);
			string aCap = findValue(aDN, attribute2);
			aCount--;
		}
		//findScheduledJob(aDoc);
	}
	SenComModel::SenComModel(){}
	SenComModel::~SenComModel(){}

	int internalMain(int argc, char* argv[])
	{
		string inFile1 = "output_A1.xml";
		string inFile2 = "A1.xml";
		SenComModel* aModel = SenComModel::getModel(inFile1, inFile2);
		aModel->geResources(inFile1, inFile2);
		return 0;
	}



	/* Övrigt nedanför denna*/
	int rain(int argc, char* argv[])
	{
		//return main3(argc, argv);

		try {
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch) {

			cout << "Error during initialization! :\n";
			return 1;
		}




		//DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();

		XercesDOMParser* parser = new XercesDOMParser();
		parser->setValidationScheme(XercesDOMParser::Val_Never);
		parser->setDoNamespaces(true);    // optional

		ErrorHandler* errHandler = (ErrorHandler*) new MyErrorHandler();
		parser->setErrorHandler(errHandler);


		char* xmlFile = "output_A1.xml";
		xmlFile = "A1.xml";
		//char* xmlFile = "employees.xml";

		try {
			parser->parse(xmlFile);
		}
		catch (...) {
			cout << "Exception message is: \n";
			return -1;
		}

		DOMDocument* aDoc = parser->adoptDocument();
		XMLCh* temp = XMLString::transcode("job");
		DOMNodeList* aDNL = aDoc->getElementsByTagName(temp);

		findScheduledJob(aDoc);
		return 0;


		XMLSize_t aCount = aDNL->getLength();

		if (aCount > 0)
		{
			DOMNode* aDN = aDNL->item(0);

			DOMElement* aE = (DOMElement*)aDN;



			DOMNodeList* bDNL = aE->getElementsByTagName(XMLString::transcode("mode"));

			string aDesc = "";
			DOMNodeList* cDNL = aDN->getChildNodes();
			{
				for (int i = 0; i<cDNL->getLength(); i++)
				{
					DOMNode* aNode = cDNL->item(i);
					const XMLCh* nodeName = aNode->getNodeName();
					const XMLCh* nodeValue = aNode->getNodeValue();
					const XMLCh* localName = aNode->getLocalName();
					DOMNode::NodeType nodeType = aNode->getNodeType();
					//const XMLCh* nodeName = aNode->();
					const XMLCh* textContent = aNode->getTextContent();
					if (nodeType == DOMNode::NodeType::ELEMENT_NODE)
					{
						string s1 = XMLString::transcode(nodeName);
						string s2 = XMLString::transcode(textContent);

						string s3 = "No Child";
						if (aNode->getFirstChild() != NULL)
							s3 = XMLString::transcode(aNode->getFirstChild()->getNodeName());

						aDesc += s1 + " " + s2 + " " + s3;
						aDesc += "\n";
					}

					i = i;
				}
			}

			DOMNode::NodeType aNT = aDN->getNodeType();

			DOMNamedNodeMap* aMap = aDN->getAttributes();

			XMLSize_t aMSize = aMap->getLength();

			DOMNode* aC = aDN->getFirstChild();

			DOMNode::NodeType bNT = aC->getNodeType();


			const XMLCh* aValue = aC->getNodeValue();
			const XMLCh* bValue = aDN->getNodeValue();
			/* lÄGG TILL update*/

			DOMElement* aDE = aDoc->getDocumentElement();

			DOMNode::NodeType cNT = aDoc->getNodeType();
			//aDE->ge

			int g = 678;
		}

		DOMNode* aNode = aDoc->getFirstChild();
		DOMNode* bNode = aNode->getFirstChild();

		//aNOode->

		XMLSize_t aError = parser->getErrorCount();
		//parser->e


		return 0;

		SimModel* aM = new SimModel();
		Counter* aC = new Counter(aM->context()->engine(), aM->context());
		for (int i = 0; i < 100; i++)
		{
			std::function<void()> aFP = std::bind(&writeLog, i);
			std::function<void()> bFP = std::bind(&writeLog, i);


			aC->insert(new CounterUser(aFP, bFP, i * 100));
		}

		aM->runModel();
		double slutet = aM->context()->engine()->simulated_time();
		delete aM;
		return 0;



		list<CombinedJobController*> loads = list<CombinedJobController*>();
		for (int i = 0; i < 1000; i++)
		{
			CombinedJobController* var = new CombinedJobController(aM->context(), "B");
			var->AddJob(new DelayJob(aM->context(), i * 10));
			loads.push_back(var);
		}

		int steps = 5000;

		for (int i = 1; i < steps; i++)
		{
			EnergySim::Resource* aRes = new EnergySim::Resource(new Clock(aM->context()->engine()), "A");
			aRes->setCapacity(1);
			aM->resources.itsValue.push_back(aRes);
			Process* aP = new Process();
			aP->processID = i;// processID;
			list<pair<long, Resource*>>* aRList = new list<pair<long, Resource*>>();
			aRList->push_back(pair<long, Resource*>(1, aRes));
			ConcreteProcess* aCP = new ConcreteProcess(aRList, i);
			aP->itsAlternates.push_back(aCP);
			double time = 3.5;

			if (i == 1)
			{
				for each (CombinedJobController* var in loads)
				{
					var->AddJob(new WaitForResourcesJob(aP, aM, var->ID()));
				}
			}
			else
			{
				for each (CombinedJobController* var in loads)
				{
					var->AddJob(new WaitForResourcesJob(aP, aM, var->ID()));
					var->AddJob(new FinishProcessJob(aP->processID - 1, var->ID(), aM));
					var->AddJob(new DelayJob(aM->context(), time));
				}
			}
		}
		for each (CombinedJobController* var in loads)
		{
			var->AddJob(new DelayJob(aM->context(), 40));
			var->AddJob(new FinishProcessJob(steps - 1, var->ID(), aM));
		}
		for each (CombinedJobController* var in loads)
			aM->context()->engine()->ScheduleJobNow(var);
		//aM->context()->engine()->Run();

		aM->runModel();

		double slut = aM->context()->engine()->simulated_time();
		return 0;
	}
}
