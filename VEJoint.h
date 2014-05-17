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
	unsigned int HUB_THRESHOLD = 500;
	bool hasHub = false;
	int totalHubs = 0;
public:
	VEJoint() {}

	map<pair<string, string>, shared_ptr<vector<pair<int, int> > > > edgeIndex;
	map<triple<string, string, string>, shared_ptr<vector<triple<int, int, int> > > > jointIndex;
	map<string, shared_ptr<vector<int> > > hubsMap;
	map<int, shared_ptr<map<string, shared_ptr<vector<int> > > > > hubOutNeighbors;
	map<int, shared_ptr<map<string, shared_ptr<vector<int> > > > > hubInNeighbors;

	VEJoint(string _filename):filename(_filename) {
		graphPartition = make_shared<Partition>();
		graphPartition->loadGraphFromFile(filename);
	}

	VEJoint(shared_ptr<Partition> inputPartition) {
		graphPartition = inputPartition;
	}


	void indexEdges();
	void indexJoints();
	void outputIndex(string &filename);
	void loadIndex(string &filename);
	void loadIndexWithHub(string &filename);
	shared_ptr<vector<triple<int, int, int> > > tripleMatches(triple<string, string, string> labels);
	shared_ptr<vector<pair<int, int> > > pairMatches(pair<string, string> labels);
};

#endif /* VEJOINT_H_ */
