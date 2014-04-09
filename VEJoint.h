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

using namespace std;

class VEJoint {
private:
	//The original data graph
	shared_ptr<Graph> dataGraph;
	string filename;
	map<pair<string, string>, shared_ptr<vector<pair<int, int> > > > edgeIndex;
	map<triple<string, string, string>, shared_ptr<vector<triple<int, int, int> > > > jointIndex;

	VEJoint() {}

public:

	VEJoint(string _filename):filename(_filename) {
		dataGraph = make_shared<Graph>();
		dataGraph->loadGraphFromFile(filename);
	}

	void IndexEdges(shared_ptr<Graph> graphPartition);
	void IndexJoints(shared_ptr<Graph> graphPartition);
	void outputIndex(string &filename);
};

#endif /* VEJOINT_H_ */
