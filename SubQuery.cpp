/*
 * SubQuery.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: Administrator
 */

#include <SubQuery.h>
#include <vector>
#include <memory>
#include <Graph.h>
#include <iostream>
using namespace std;

vector<shared_ptr<Graph> > SubQuery::decomposeIntoStar(shared_ptr<Graph> queryGraph) {
	vector<shared_ptr<Graph>> result;

	queryGraph->calculateDegree();

	while (true) {
		int max = 0;
		int center = -1;
		for (auto i = queryGraph->primaryAttribute.begin(); i != queryGraph->primaryAttribute.end(); ++i) {
			int v = queryGraph->indegree[i->first] + queryGraph->outdegree[i->first];
			if (v > max) {
				max = v;
				center = i->first;
			}
		}

		if (center == -1) break;

		shared_ptr<Graph> star = make_shared<Graph>();
		auto centerLabel = queryGraph->primaryAttribute[center];

		if (queryGraph->children.find(center) != queryGraph->children.end()) {
			vector<int> clist(*queryGraph->children[center]);
			for (auto i = clist.begin(); i != clist.end(); ++i) {
				auto childLabel = queryGraph->primaryAttribute[*i];
				star->addEdge(center, *centerLabel, *i, *childLabel);
				queryGraph->removeEdge(center, *i);
			}
		}

		if (queryGraph->parents.find(center) != queryGraph->parents.end()) {
			vector<int> plist(*queryGraph->parents[center]);
			for (auto i = plist.begin(); i != plist.end(); ++i) {
				auto parentLabel = queryGraph->primaryAttribute[*i];
				star->addEdge(*i, *parentLabel, center, *centerLabel);
				queryGraph->removeEdge(*i, center);
			}
		}

		result.push_back(star);
	}

	return result;
}




