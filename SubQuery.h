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
#include <VEJoint.h>
#include <Star.h>
#include <MatchedComponent.h>
using namespace std;

class SubQuery {
private:
	//Which is essentially a graph partition
	shared_ptr<VEJoint> vejoint;
public:
	SubQuery():vejoint(NULL) { }
	SubQuery(shared_ptr<VEJoint> _vejoint):vejoint(_vejoint) { }

	vector<shared_ptr<Star> > decomposeIntoStar(shared_ptr<Graph> queryGraph);

	shared_ptr<vector<MatchedComponent> > starQuery(shared_ptr<Star> star);

	shared_ptr<vector<MatchedComponent> > joinStar(vector<shared_ptr<Star> > &stars, vector<shared_ptr<vector<MatchedComponent> > > &matches);
	shared_ptr<vector<MatchedComponent> > evaluate(shared_ptr<Graph> queryGraph);
};


#endif /* SUBQUERY_H_ */
