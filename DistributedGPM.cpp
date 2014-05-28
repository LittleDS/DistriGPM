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
#include <fstream>
#include <sstream>
#include <Graph.h>
#include <VEJoint.h>
#include <SubQuery.h>
#include <windows.h>
#include <QueryGenerator.h>

using namespace std;
void outputVector(shared_ptr<vector<int> > input) {
	for (auto i = input->begin(); i != input->end(); i++) {
		cout << *i << " ";
	}
	cout << endl;
}

inline string itos(int value) {
	ostringstream convert;
	convert << value;
	return convert.str();
}

int main(int argc, char *argv[]) {
//	string dataFile = "LinkedINDataNew1";
//	shared_ptr<Partition> p = make_shared<Partition>();
//	p->loadGraphFromFile(dataFile);
//	shared_ptr<VEJoint> v = make_shared<VEJoint>(p);
//	v->indexEdges();
//	v->indexJoints();
//	v->outputIndex(dataFile);

	/*
	 * Generate Query
	 *
	 */
//	string dataFile = "AmazonNew";
//	shared_ptr<Graph> dataGraph = make_shared<Graph>();
//	dataGraph->loadGraphFromFile(dataFile);
//	shared_ptr<QueryGenerator> qg = make_shared<QueryGenerator>(dataGraph, 5, 2);
//	for (int i = 0; i < 50; i++) {
//		string fn = "Amazonquery" + itos(i);
//		shared_ptr<Graph> q = qg->generateStarQuery();
//		q->outputGraph(fn);
//	}

//	string part = "LinkedINDataNewPar.part";
//	dataGraph->divideGraph(2, part, dataFile);
//	string parFile = "LinkedINDataNewPar";
//	dataGraph->outputParMETIS(parFile);
//	string dataFile = "LinkedINData";
//	shared_ptr<Graph> dataGraph = make_shared<Graph>();
//	dataGraph->loadGraphFromFile(dataFile);
//	cout << "Finished Loading" << endl;
//	QueryGenerator qg(dataGraph, 4, 2);
//	shared_ptr<Graph> query = qg.generatePathQuery();
//	query->print();

	/*
	 * Build the index
	 */
//	string dataFile = "AmazonNew";
//	shared_ptr<VEJoint> vj = make_shared<VEJoint>(dataFile);
//	vj->indexEdges();
//	vj->indexJoints();
//	vj->outputIndex(dataFile);
//	cout << "Finished Loading VEJoint" << endl;

	/*
	 * Single machine query
	 */
	string dataFile = "AmazonNew";
	shared_ptr<VEJoint> vj = make_shared<VEJoint>();
	vj->loadIndexWithHub(dataFile);
	cout << "Finished loading index" << endl;
	SubQuery sq(vj);

	for (int i = 0; i < 50; i++) {
		shared_ptr<Graph> queryGraph = make_shared<Graph>();
		string queryFile = "Amazonquery" + itos(i);
		queryGraph->loadGraphFromFile(queryFile);

		//vector<shared_ptr<Star> > stars = sq.decomposeIntoStar(queryGraph);
		unsigned __int64 freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		double timerFrequency = (1.0/freq);

		unsigned __int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&startTime);

		shared_ptr<vector<MatchedComponent> > finalMatch;
		for (int j = 0; j < 1; j++) {
			finalMatch = sq.evaluate(queryGraph);
		}

		unsigned __int64 endTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&endTime);
		double timeDifferenceInMilliseconds = ((endTime-startTime) * timerFrequency);
		cout << "Query " << i << " Total matches " << finalMatch->size() << endl;
		for (auto i = finalMatch->begin(); i != finalMatch->end(); ++i) {
			i->print();
			cout << "!------------------------------!" << endl;
		}

		cout << "Running time " << timeDifferenceInMilliseconds / 1.0 << endl;
	}

	/*
	 *
	 * Distributed
	 *
	 */
//	MPI::Init(argc, argv);
//	int size = MPI::COMM_WORLD.Get_size();
//	int rank = MPI::COMM_WORLD.Get_rank();
//	vector<int> dataToShare;
//	int dataLength;
//	shared_ptr<VEJoint> vj;
//	if (rank != 0) {
//		//Load the corresponding VEJoint
//		vj = make_shared<VEJoint>();
//
//		string indexFile = "LinkedINDataNew";
//
//		string indexToLoad = indexFile + itos(rank - 1);
//
//		vj->loadIndexWithHub(indexToLoad);
//		string logname = "Log" + itos(rank);
//		ofstream logFile(logname);
//		logFile << "Process " << rank << " finished loading index" << endl;
//		logFile.close();
//	}
//
//	MPI::COMM_WORLD.Barrier();
//
//	if (rank == 0) {
//		int fileID = rank - 1;
//		ostringstream convert;
//		convert << fileID;
//
//		shared_ptr<Graph> queryGraph = make_shared<Graph>();
//		string queryFile = "queryGraph";
//		queryGraph->loadGraphFromFile(queryFile);
//
//		SubQuery sq;
//		vector<shared_ptr<Star> > decomposedStars = sq.decomposeIntoStar(queryGraph);
//
//		dataToShare.push_back(decomposedStars.size());
//		for (auto i = decomposedStars.begin(); i != decomposedStars.end();
//				i++) {
//			shared_ptr<Star> t = *i;
//			shared_ptr<vector<int> > v = t->serialize();
//			dataToShare.insert(dataToShare.end(), v->begin(), v->end());
//		}
//		dataLength = dataToShare.size();
//
//		MPI::COMM_WORLD.Bcast(&dataLength, 1, MPI_INT, 0);
//
//		dataToShare.resize(dataLength);
//
//		MPI::COMM_WORLD.Bcast((void*) dataToShare.data(), dataToShare.size(), MPI_INT, 0);
//
//		string logname = "Log" + itos(rank);
//		ofstream logFile(logname);
//
//		logFile << "Process " << rank << " finished sending stars" << endl;
//
//		//MPI::COMM_WORLD.Barrier();
//
//		vector<shared_ptr<vector<MatchedComponent> > > matches;
//
//		//Collect the star query answers from the slaves
//		for (unsigned int k = 0; k < decomposedStars.size(); k++) {
//
//			shared_ptr<vector<MatchedComponent> > returnedStarMatches =
//					make_shared<vector<MatchedComponent> >();
//
//			for (int i = 1; i < size; i++) {
//				//Probe first
//				MPI::Status status;
//				MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, 0, status);
//				const auto sender = status.Get_source();
//				const auto length = status.Get_count(MPI::INT);
//				shared_ptr<vector<int> > buf = make_shared<vector<int> >(length);
//
//				MPI::COMM_WORLD.Recv((void *) buf->data(), length, MPI_INT, sender, 0);
//
//				//outputVector(buf);
//
//				logFile << "Received data from " << sender << endl;
//
//				vector<int>::iterator it = buf->begin();
//				int tN = *it;
//				it++;
//				for (int j = 0; j < tN; j++) {
//					shared_ptr<MatchedComponent> tm = make_shared<MatchedComponent>();
//					tm->deseralize(it);
//					returnedStarMatches->push_back(*tm);
//					tm.reset();
//				}
//
//				buf.reset();
//
//			}
//
//			matches.push_back(returnedStarMatches);
//		}
//
//		//Join the star back
//		shared_ptr<vector<MatchedComponent> > finalMatch = sq.joinStar(decomposedStars, matches);
//		for (auto i = finalMatch->begin(); i != finalMatch->end(); ++i) {
//			i->print(logFile);
//			logFile << "!------------------------------!" << endl;
//		}
//
//		logFile.close();
//	}
//	else {
//
//		SubQuery sq(vj);
//
//		MPI::COMM_WORLD.Bcast(&dataLength, 1, MPI_INT, 0);
//
//		dataToShare.resize(dataLength);
//
//		MPI::COMM_WORLD.Bcast((void*) dataToShare.data(), dataToShare.size(), MPI_INT, 0);
//
//		//MPI::COMM_WORLD.Barrier();
//
//		//Rebuild the stars from the stream
//		vector<int>::iterator it = dataToShare.begin();
//		int N = *it;
//		it++;
//		vector<shared_ptr<Star> > stars;
//		for (int i = 0; i < N; i++) {
//			shared_ptr<Star> t = make_shared<Star>();
//			t->deseralize(it);
//			stars.push_back(t);
//		}
//
//		//Answer the star queries
//		for (auto i = stars.begin(); i != stars.end(); i++) {
//			shared_ptr<vector<MatchedComponent> > starMatches = sq.starQuery(*i);
//
//			shared_ptr<vector<int> > dataToSend = make_shared<vector<int> >();
//			dataToSend->push_back(starMatches->size());
//			for (auto j = starMatches->begin(); j != starMatches->end(); ++j) {
//				shared_ptr<vector<int> > ser = j->serialize();
//
//				dataToSend->insert(dataToSend->end(), ser->begin(), ser->end());
//
//				ser.reset();
//			}
//			//outputVector(dataToSend);
//			MPI::COMM_WORLD.Send((void *) dataToSend->data(), dataToSend->size(), MPI_INT, 0, 0);
//
//			dataToSend.reset();
//		}
//	}
//
//	MPI::Finalize();
//	return 0;

}

