/*
 * VEJoint.h
 *
 *  Created on: 2014-3-26
 *      Author: Lei Yang
 */

#ifndef VEJOINT_H_
#define VEJOINT_H_

#include <memory>
#include <Graph.h>
#include <string>
#include <Triple.h>
#include <Partition.h>

using namespace std;

class VEJoint {
private:
	//The original data graph
	shared_ptr<Partition> graphPartition;
	string filename;
	unsigned int HUB_THRESHOLD = 0;

	bool hasHub = false;
	int totalHubs = 0;
public:
	VEJoint() {}

	map<pair<int, int>, shared_ptr<vector<pair<int, int> > > > edgeIndex;
	map<triple<int, int, int>, shared_ptr<vector<triple<int, int, int> > > > jointIndex;
	map<int, shared_ptr<vector<int> > > hubsMap;
	map<int, shared_ptr<map<int, shared_ptr<vector<int> > > > > hubOutNeighbors;
	map<int, shared_ptr<map<int, shared_ptr<vector<int> > > > > hubInNeighbors;

	VEJoint(string _filename):filename(_filename) {
		graphPartition = make_shared<Partition>();
		graphPartition->loadGraphFromFile(filename);
	}

	VEJoint(shared_ptr<Partition> inputPartition) {
		graphPartition = inputPartition;
	}


	void indexEdges();
	void indexJoints();
	void indexJointsNoHubsIndex();
	void outputIndex(string &filename);
	void outputIndexNoHubsIndex(string &filename);
	void loadIndex(string &filename);
	void loadIndexWithHub(string &filename);
	void loadIndexWithoutHub(string &filename);
	shared_ptr<vector<triple<int, int, int> > > tripleMatches(triple<int, int, int> labels);
	shared_ptr<vector<triple<int, int, int> > > tripleMatchesNoHubs(triple<int, int, int> labels);
	shared_ptr<vector<pair<int, int> > > pairMatches(pair<int, int> labels);
};

#endif /* VEJOINT_H_ */
