/*
 ============================================================================
 Name        : DistributedGPM.c
 Author      : Lei Yang
 Version     :
 Copyright   : Lei Yang's Thesis
 Description : A distributed version of graph pattern matcing algorithm
 ============================================================================
 */
#include <math.h> 
#include "mpi.h" 
#include <iostream>
#include <sstream>
#include <Graph.h>
#include <VEJoint.h>
#include <SubQuery.h>
#include <windows.h>

using namespace std;

int main(int argc, char *argv[]) {
	string dataFile = "LinkedINData";
	shared_ptr<VEJoint> vj = make_shared<VEJoint>();
//	vj->indexEdges();
//	vj->indexJoints();
//	vj->outputIndex(dataFile);
	vj->loadIndexWithHub(dataFile);

	cout << "Finished Loading VEJoint" << endl;
	SubQuery sq(vj);
	shared_ptr<Graph> queryGraph = make_shared<Graph>();
	string queryFile = "queryGraph";
	queryGraph->loadGraphFromFile(queryFile);

	unsigned __int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	double timerFrequency = (1.0/freq);

	unsigned __int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&startTime);


	shared_ptr<vector<MatchedComponent> > finalMatch = sq.evaluate(queryGraph);


	unsigned __int64 endTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&endTime);
	double timeDifferenceInMilliseconds = ((endTime-startTime) * timerFrequency);

	for (auto i = finalMatch->begin(); i != finalMatch->end(); ++i) {
		i->print();
		cout << "!------------------------------!" << endl;
	}

	cout << "Running time " << timeDifferenceInMilliseconds << endl;


//	MPI::Init(argc, argv);
//	size = MPI::COMM_WORLD.Get_size();
//	rank = MPI::COMM_WORLD.Get_rank();
//
//	n=1000; // number of intervals
//
//	MPI::COMM_WORLD.Bcast(&n, 1, MPI::INT, 0);
//	h = 1.0 / (double) n;
//	sum = 0.0;
//	for (i = rank + 1; i <= n; i += size) {
//		x = h * ((double) i - 0.5);
//		sum += (4.0 / (1.0 + x * x));
//	}
//	mypi = h * sum;
//
//	MPI::COMM_WORLD.Reduce(&mypi, &pi, 1, MPI::DOUBLE, MPI::SUM, 0);
//	if (rank == 0)
//		cout << "pi is approximately " << pi << ", Error is "
//				<< fabs(pi - PI25DT) << endl;
//
//	MPI::Finalize();
	return 0;
}

