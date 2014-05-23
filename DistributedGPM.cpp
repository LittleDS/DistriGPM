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
#include <QueryGenerator.h>
using namespace std;
void outputVector(shared_ptr<vector<int> > input) {
	for (auto i = input->begin(); i != input->end(); i++) {
		cout << *i << " ";
	}
	cout << endl;
}
int main(int argc, char *argv[]) {
//	string dataFile = "LinkedINData";
//	shared_ptr<Graph> dataGraph = make_shared<Graph>();
//	dataGraph->loadGraphFromFile(dataFile);
//	cout << "Finished Loading" << endl;
//	QueryGenerator qg(dataGraph, 4, 2);
//	shared_ptr<Graph> query = qg.generatePathQuery();
//	query->print();
//	shared_ptr<VEJoint> vj = make_shared<VEJoint>();
//	vj->indexEdges();
//	vj->indexJoints();
//	vj->outputIndex(dataFile);
//	vj->loadIndexWithHub(dataFile);
//
//	cout << "Finished Loading VEJoint" << endl;

//	SubQuery sq;
//	shared_ptr<Graph> queryGraph = make_shared<Graph>();
//	string queryFile = "queryGraph";
//	queryGraph->loadGraphFromFile(queryFile);
//	queryGraph->print();

//	vector<shared_ptr<Star> > stars = sq.decomposeIntoStar(queryGraph);
//	for (auto i = stars.begin(); i != stars.end(); i++) {
//		(*i)->print();
//		cout << "*****************" << endl;
//	}
//
//	unsigned __int64 freq;
//	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
//	double timerFrequency = (1.0/freq);
//
//	unsigned __int64 startTime;
//	QueryPerformanceCounter((LARGE_INTEGER *)&startTime);
//
//
//	shared_ptr<vector<MatchedComponent> > finalMatch = sq.evaluate(queryGraph);
//
//
//	unsigned __int64 endTime;
//	QueryPerformanceCounter((LARGE_INTEGER *)&endTime);
//	double timeDifferenceInMilliseconds = ((endTime-startTime) * timerFrequency);
//
//	for (auto i = finalMatch->begin(); i != finalMatch->end(); ++i) {
//		i->print();
//		cout << "!------------------------------!" << endl;
//	}
//
//	cout << "Running time " << timeDifferenceInMilliseconds << endl;


	MPI::Init(argc, argv);
	int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	vector<int> dataToShare;
	int dataLength;

	if (rank == 0) {
		cout << "Number of processes: " << size << endl;
		shared_ptr<Graph> queryGraph = make_shared<Graph>();
		string queryFile = "queryGraph";
		queryGraph->loadGraphFromFile(queryFile);
		cout << "Process " << rank << " finished loading query graph." << endl;
		SubQuery sq;
		vector<shared_ptr<Star> > decomposedStars = sq.decomposeIntoStar(queryGraph);

		cout << "Process " << rank << " finished decomposing the query graph."
				<< endl;

		dataToShare.push_back(decomposedStars.size());
		for (auto i = decomposedStars.begin(); i != decomposedStars.end();
				i++) {
			shared_ptr<Star> t = *i;
			shared_ptr<vector<int> > v = t->serialize();
			dataToShare.insert(dataToShare.end(), v->begin(), v->end());
		}
		cout << "Process " << rank << " finished serializing the star queries."
				<< endl;
		dataLength = dataToShare.size();

		MPI::COMM_WORLD.Bcast(&dataLength, 1, MPI_INT, 0);

		dataToShare.resize(dataLength);

		MPI::COMM_WORLD.Bcast((void*) dataToShare.data(), dataToShare.size(), MPI_INT, 0);

		vector<shared_ptr<vector<MatchedComponent> > > matches;

		//Collect the star query answers from the slaves
		for (unsigned int k = 0; k < decomposedStars.size(); k++) {

			shared_ptr<vector<MatchedComponent> > returnedStarMatches =
					make_shared<vector<MatchedComponent> >();

			for (int i = 1; i < size; i++) {
				//Probe first
				MPI::Status status;
				MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, 0, status);
				const auto sender = status.Get_source();
				const auto length = status.Get_count(MPI::INT);
				shared_ptr<vector<int> > buf = make_shared<vector<int> >(length);

				MPI::COMM_WORLD.Recv((void *) buf->data(), length, MPI_INT, i, 0);

				cout << "Received data from " << sender << endl;

				vector<int>::iterator it = buf->begin();
				int tN = *it;
				it++;
				for (int j = 0; j < tN; j++) {
					shared_ptr<MatchedComponent> tm = make_shared<MatchedComponent>();
					tm->deseralize(it);
					returnedStarMatches->push_back(*tm);
					tm.reset();
				}

				buf.reset();

			}

			matches.push_back(returnedStarMatches);
		}

		//Join the star back
		shared_ptr<vector<MatchedComponent> > finalMatch = sq.joinStar(decomposedStars, matches);
		for (auto i = finalMatch->begin(); i != finalMatch->end(); ++i) {
			i->print();
			cout << "!------------------------------!" << endl;
		}
	}
	else {

		MPI::COMM_WORLD.Bcast(&dataLength, 1, MPI_INT, 0);

		dataToShare.resize(dataLength);

		MPI::COMM_WORLD.Bcast((void*) dataToShare.data(), dataToShare.size(), MPI_INT, 0);

		//Rebuild the stars from the stream
		vector<int>::iterator it = dataToShare.begin();
		int N = *it;
		it++;
		vector<shared_ptr<Star> > stars;
		for (int i = 0; i < N; i++) {
			shared_ptr<Star> t = make_shared<Star>();
			t->deseralize(it);
			stars.push_back(t);
		}

		//Load the corresponding VEJoint
		shared_ptr<VEJoint> vj = make_shared<VEJoint>();
		string indexFile = "LinkedINData";
		vj->loadIndexWithHub(indexFile);
		SubQuery sq(vj);
		cout << "Process " << rank << " finished loading index." << endl;
		//Answer the star queries
		for (auto i = stars.begin(); i != stars.end(); i++) {
			shared_ptr<vector<MatchedComponent> > starMatches = sq.starQuery(*i);

			shared_ptr<vector<int> > dataToSend = make_shared<vector<int> >();
			dataToSend->push_back(starMatches->size());
			for (auto j = starMatches->begin(); j != starMatches->end(); ++j) {
				shared_ptr<vector<int> > ser = j->serialize();

				dataToSend->insert(dataToSend->end(), ser->begin(), ser->end());

				ser.reset();
			}
			MPI::COMM_WORLD.Send((void *) dataToSend->data(), dataToSend->size(), MPI_INT, 0, 0);

			dataToSend.reset();
		}
	}

	MPI::Finalize();
	return 0;

}

