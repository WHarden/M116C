#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>
#include "cache.h"
#include <string>
#include<bitset>
using namespace std;

struct trace
{
	bool MemR;
	bool MemW;
	int adr;
	int data;
};

/*
Either implement your memory_controller here or use a separate .cpp/.c file for memory_controller and all the other functions inside it (e.g., LW, SW, Search, Evict, etc.)
*/

int main (int argc, char* argv[]) // the program runs like this: ./program <filename> <mode>
{
	// input file (i.e., test.txt)
	string filename = argv[1];

	ifstream fin;

	// opening file
	fin.open(filename.c_str());
	if (!fin){ // making sure the file is correctly opened
		cout << "Error opening " << filename << endl;
		exit(1);
	}

	// reading the text file
	string line;
	vector<trace> myTrace;
	int TraceSize = 0;
	string s1,s2,s3,s4;
    while( getline(fin,line) )
      	{
            stringstream ss(line);
            getline(ss,s1,',');
            getline(ss,s2,',');
            getline(ss,s3,',');
            getline(ss,s4,',');
            myTrace.push_back(trace());
            myTrace[TraceSize].MemR = stoi(s1);
            myTrace[TraceSize].MemW = stoi(s2);
            myTrace[TraceSize].adr = stoi(s3);
            myTrace[TraceSize].data = stoi(s4);
            //cout<<myTrace[TraceSize].MemW << endl;
            TraceSize+=1;
        }


	// Defining cache and stat
    cache myCache;
    int myMem[MEM_SIZE];


	int traceCounter = 0;
	bool cur_MemR;
	bool cur_MemW;
	int cur_adr;
	int cur_data;

	// this is the main loop of the code
	while(traceCounter < TraceSize){

		cur_MemR = myTrace[traceCounter].MemR;
		cur_MemW = myTrace[traceCounter].MemW;
		cur_data = myTrace[traceCounter].data;
		cur_adr = myTrace[traceCounter].adr;
		traceCounter += 1;
		myCache.controller (cur_MemR, cur_MemW, &cur_data, cur_adr, myMem); // in your memory controller you need to implement your FSM, LW, SW, and MM.
	}

	float L1_miss_rate, L2_miss_rate, AAT;
	//compute the stats here:
	vector<float> result = myCache.return_result();
	L1_miss_rate=result[0]/+result[1]; L2_miss_rate= result[2] / +result[3]; AAT=1+(L1_miss_rate * (8+(L2_miss_rate)*100));

	cout<< "(" << L1_miss_rate<<","<<L2_miss_rate<<","<<AAT<<")"<<endl;

	// closing the file
	fin.close();

	return 0;
}
