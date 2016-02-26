#include "stdafx.h"
#include "Objective.h"

#include <stdio.h>
#include <windows.h>

//unsigned long __stdcall NET_RvThr(void * pParam);
DWORD WINAPI ThreadProc();
HANDLE hPipe1, hPipe2;
BOOL Finished;

void ObjectiveReporter::print(unsigned char ov, double value, double time, int lineID)
{
	//return;
	ofstream myfile;
	if (started)
	{
		myfile.open("sentmessages.txt", std::ios::app);
	}
	else
	{
		myfile.open("sentmessages.txt", std::ios::out);
		myfile << "Type	Time	Line	Value" << endl;
		started = true;
	}
	int aType = ov;
	myfile << aType;
	myfile << "\t";
	myfile << time;
	myfile << "\t";
	myfile << lineID;
	myfile << "\t";
	myfile << value;
	myfile << endl;
	myfile.close();
	return;
}

void ObjectiveReporter::init()
{
	LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\myNamedPipe2");


	//Thread Init Data
	//DWORD threadId;
	//HANDLE hThread = NULL;

	BOOL Write_St = TRUE;

	Finished = FALSE;

	hPipe1 = CreateFile(lpszPipename1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);


	if ((hPipe1 == NULL || hPipe1 == INVALID_HANDLE_VALUE))
	{
		printf("Could not open the pipe  - (error %d)\n", GetLastError());

	}
	else
	{
		// print(8, 666, 666, 666);
		//TEST hThread = CreateThread( NULL, 0, &NET_RvThr, NULL, 0, NULL);
		//do
		//{
		//	printf("Enter your message: ");
		//	scanf("%s", buf);
		//	if (strcmp(buf, "quit") == 0)
		//		Write_St = FALSE;
		//	else
		//	{
		//		WriteFile(hPipe1, buf, dwBytesToWrite, &cbWritten, NULL);
		//		memset(buf, 0xCC, 100);

		//	}

		//} while (Write_St);

		//CloseHandle(hPipe1);
		//Finished = TRUE;
	}

//	getchar();


}

void repMain(int line, double time, double power,int  mode, ObjectiveReporter* aOR)
{
	if (mode == 0) //OFF
		aOR->report(0,0, line);
	if (mode == 1) //IDLE
		aOR->report(0, power/10, line);
	if (mode == 2) // HEAT
		aOR->report(0, power / 2, line);
	if (mode == 3)// PRODUCTION
		aOR->report(0, power * (0.46 + time / 4), line);
		//CHANGED 160224 aOR->report(0, power * (0.5 + time/4), line);
}
void repExtruder(int line, double time, double power, int  mode, ObjectiveReporter* aOR)
{
	if (mode == 0) //OFF
	{
		aOR->report(1, 0, line);
		return;
	}
	//CHANGED 160224 aOR->report(1, power *(0.26 + time/100), line);
	aOR->report(1, power *(0.245 + time / 100), line);
}
void repGrinder(int line, double time, double power, int  mode, ObjectiveReporter* aOR)
{
	if (mode == 0) //OFF
		aOR->report(2, 0, line);
	if (mode == 1) //IDLE
		aOR->report(2, power / 50, line);
	if (mode == 2) // HEAT
		aOR->report(2, power *(0.01 + time/50), line);
	if (mode == 3)// PRODUCTION
		aOR->report(2, power * (0.028 + time/500), line);
}
void repHyd(int line, double time, double power, int  mode, ObjectiveReporter* aOR)
{
	double phase = time / 350;
	if (mode == 0) //OFF
		aOR->report(3, 0, line);
	if (mode == 1) //IDLE
		aOR->report(3, power *0.04, line);
	if (mode == 2) //HEAT
		aOR->report(3, power *0.04, line);
	if (mode == 3) //PRODUCTION
		aOR->report(3, power * (0.04 +  time*0.3),line);	
}

double ObjectiveReporter::makeWave(double d)
{
	if (d < 0)
		return 0;
	if (d > 0.99)
		return 0;
	if (d < 0.5)
		return d * 2;
	return (1 - d)*2;
}

void ObjectiveReporter::reportAll()
{
	double power = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("respowerone");
	int mode = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("resmodeone");
	int cycletime = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("restimeone");
	
	int randNum = rand() % (100 - 1 + 1) + 1;
	randNum -= 50;
	double time = itsEngine->simulated_time();
	power = power + randNum * 50;
	if (power < 0)
		power = 0;

	double phase = time;
	while (phase > cycletime)
		phase = phase - cycletime;
	phase = phase / cycletime;
	phase = abs(phase);
	phase = makeWave(phase);
	repMain(1, phase, power, mode, this);
	repExtruder(1, phase, power, mode, this);
	repGrinder(1, phase, power, mode, this);
	repHyd(1, phase, power, mode, this);

	power = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("respowertwo");
	mode = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("resmodetwo");
	cycletime = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("restimetwo");
	phase = time;
	while (phase > cycletime)
		phase = phase - cycletime;
	phase = phase / cycletime;
	phase = abs(phase);
	phase = makeWave(phase);
	repMain(2, phase, power, mode, this);
	repExtruder(2, phase, power, mode, this);
	repGrinder(2, phase, power, mode, this);
	repHyd(2, phase, power, mode, this);

	power = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("respowerthree");
	mode = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("resmodethree");
	cycletime = itsModel->itsParser.itsValue->getAttributeHandler()->getAttribute("restimethree");
	phase = time;
	while (phase > cycletime)
		phase = phase - cycletime;
	phase = phase / cycletime;
	phase = abs(phase);
	phase = makeWave(phase);
	repMain(3, phase, power, mode, this);
	repExtruder(3, phase, power, mode, this);
	repGrinder(3, phase, power, mode, this);
	repHyd(3, phase, power, mode, this);
}

void ObjectiveReporter::report(unsigned char id, int value, unsigned char lineID)
{
	report(id, value, itsEngine->simulated_time()*1000, lineID);
}

void ObjectiveReporter::report(unsigned char id, int value, int time, unsigned char lineID)
{
	print(id, value, time, lineID);
	DWORD cbWritten;
	unsigned char buf[100];

	string aStr = std::to_string(lineID); // "1,2,3,4";
	aStr += ",";
	aStr += std::to_string(id);
	aStr += ",";
	aStr += std::to_string(value);
	aStr += ",";
	aStr += std::to_string(time);

	for (int i = 0; i <100; i++)
		buf[i] = 0;
	for (int i = 0; i < aStr.length(); i++)
		buf[i] = aStr[i]; 

	WriteFile(hPipe1, buf, 100, &cbWritten, NULL);
}


void ObjectiveReporter::testing()
{
	//DWORD cbWritten;
	//DWORD dwBytesToWrite = (DWORD)strlen(buf);
	//WriteFile(hPipe1, buf, dwBytesToWrite, &cbWritten, NULL);
	//memset(buf, 0xCC, 100);
}

/*
unsigned long __stdcall NET_RvThr(void * pParam) {
	BOOL fSuccess;
	char chBuf[100];
	DWORD dwBytesToWrite = (DWORD)strlen(chBuf);
	DWORD cbRead;
	int i;

	while (1)
	{
		fSuccess = ReadFile(hPipe2, chBuf, dwBytesToWrite, &cbRead, NULL);
		if (fSuccess)
		{
			printf("C++ App: Received %d Bytes : ", cbRead);
			for (i = 0; i<cbRead; i++)
				printf("%c", chBuf[i]);
			printf("\n");
		}
		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
		{
			printf("Can't Read\n");
			if (Finished)
				break;
		}
	}
	return 0;
}
*/























