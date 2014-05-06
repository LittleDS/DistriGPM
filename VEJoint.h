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


public:
	VEJoint() {}

	map<pair<string, string>, shared_ptr<vector<pair<int, int> > > > edgeIndex;
	map<triple<string, string, string>, shared_ptr<vector<triple<int, int, int> > > > jointIndex;

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
};

#endif /* VEJOINT_H_ */
