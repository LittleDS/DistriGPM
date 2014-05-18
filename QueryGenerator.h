/*
 * QueryGenerator.h
 *
 *  Created on: May 17, 2014
 *      Author: Administrator
 */

#ifndef QUERYGENERATOR_H_
#define QUERYGENERATOR_H_

#include <memory>
#include <Graph.h>
#include <Star.h>
#include <vector>
#include <set>
using namespace std;

class QueryGenerator {
private:
	shared_ptr<Graph> dataGraph;
	unsigned int componentSize;
	unsigned int pathLength;

	vector<int> keySetofChildren;
	void DFSPath(int currentVertex, shared_ptr<vector<int> > Path);
	set<int> visitedNode;
public:
	QueryGenerator(shared_ptr<Graph> _dataGraph, int _componentSize, int _pathLength):
		dataGraph(_dataGraph),
		componentSize(_componentSize),
		pathLength(_pathLength)
	{
		for (auto i = dataGraph->children.begin(); i != dataGraph->children.end(); ++i) {
			keySetofChildren.push_back(i->first);
		}
	}

	shared_ptr<Graph> generateSubgraphQuery();
	shared_ptr<Star> generateStarQuery();

	shared_ptr<Graph> generatePathQuery();
	shared_ptr<Graph> generateQueryPattern();

};

#endif /* QUERYGENERATOR_H_ */
