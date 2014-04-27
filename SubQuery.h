/*
 * SubQuery.h
 *
 *  Created on: Apr 10, 2014
 *      Author: Administrator
 */

#ifndef SUBQUERY_H_
#define SUBQUERY_H_

#include <Graph.h>
#include <vector>
using namespace std;

class SubQuery {
private:
	//Which is essentially a graph partition
	shared_ptr<Graph> dataGraph;
public:
	SubQuery():dataGraph(NULL) { }
	SubQuery(shared_ptr<Graph> _dataGraph):dataGraph(_dataGraph) { }
	void evaluate(shared_ptr<Graph> queryGraph);
	vector<shared_ptr<Graph> > decomposeIntoStar(shared_ptr<Graph> queryGraph);
};


#endif /* SUBQUERY_H_ */
