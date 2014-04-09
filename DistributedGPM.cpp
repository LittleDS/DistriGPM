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
#include <Graph.h>
#include <VEJoint.h>
using namespace std;
 
int main(int argc, char *argv[]) {
	string filename = "LinkedIN";
	VEJoint vj(filename);

	string indexname = "LinkedIn";
	vj.loadIndex(filename);

	string anothername = "L";
	vj.outputIndex(anothername);


//	gp->outputParMETIS(parFile);

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

