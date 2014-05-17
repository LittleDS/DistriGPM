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
#include <MatchedComponent.h>
#include <Star.h>
using namespace std;

vector<shared_ptr<Star> > SubQuery::decomposeIntoStar(shared_ptr<Graph> queryGraph) {
	vector<shared_ptr<Star>> result;

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

		shared_ptr<Star> star = make_shared<Star>(center);
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

shared_ptr<vector<MatchedComponent> > SubQuery::starQuery(shared_ptr<Star> star) {
	const int center = star->center;

	//To store the decomposed joints and edges
	vector<triple<int, int, int> > joints;
	vector<pair<int, int> > edges;

	//Get a copy of the children and parents list
	vector<int> childrenLocal;
	if (star->children.find(center) != star->children.end())
		childrenLocal = *star->children[center];
	vector<int> parentsLocal;
	if (star->parents.find(center) != star->parents.end())
		parentsLocal = *star->parents[center];

	//Decompose the joints first
	while (!childrenLocal.empty() && !parentsLocal.empty()) {
		int c = childrenLocal.back();
		int p = parentsLocal.back();

		triple<int, int, int> toInsert(p, center, c);
		joints.push_back(toInsert);

		childrenLocal.pop_back();
		parentsLocal.pop_back();
	}

	//Insert the rest into edges
	while (!childrenLocal.empty()) {
		int c = childrenLocal.back();
		pair<int, int> toInsert(center, c);
		edges.push_back(toInsert);
		childrenLocal.pop_back();
	}

	while (!parentsLocal.empty()) {
		int p = parentsLocal.back();
		pair<int, int> toInsert(p, center);
		edges.push_back(toInsert);
		parentsLocal.pop_back();
	}

	//Start to join the matching candidates

	//Store the return result
	shared_ptr<vector<MatchedComponent> > result = make_shared<vector<MatchedComponent> >();

	while (!joints.empty()) {
		//using the label to fetch the matching candidates from VEjoint index
		triple<int, int, int> joint = joints.back();
		joints.pop_back();

		bool isCycle = false;
		if (joint.first == joint.third)
			isCycle = true;

		string labelA = *star->primaryAttribute[joint.first];
		string labelB = *star->primaryAttribute[joint.second];
		string labelC = *star->primaryAttribute[joint.third];

		triple<string, string, string> labelTri(labelA, labelB, labelC);

		shared_ptr<vector<triple<int, int, int> > > jointMatches = vejoint->tripleMatches(labelTri);

		if (jointMatches->size() > 0) {

			if (result->empty()) {
				for (auto i = jointMatches->begin(); i != jointMatches->end(); ++i) {
					if ( (isCycle && i->first != i->third) || (!isCycle && i->first == i->third) )
						continue;
					MatchedComponent mc(joint, *i);
					result->push_back(mc);
				}
			}
			else {

				shared_ptr<vector<MatchedComponent> > newResult = make_shared<vector<MatchedComponent> >();

				for (auto i = jointMatches->begin(); i != jointMatches->end(); ++i) {
					if (isCycle && i->first != i->third)
						continue;

					for (auto j = result->begin(); j != result->end(); ++j) {

						if (j->canJoin(joint, *i)) {

							MatchedComponent tempNew(*j);

							tempNew.join(joint, *i);

							newResult->push_back(tempNew);
						}
					}
				}

				result = newResult;
			}
		}
		else {
			//Return an empty result set because there is no matching candidate for current joint
			result->clear();
			return result;
		}

	}

	while (!edges.empty()) {
		pair<int, int> edge = edges.back();
		edges.pop_back();

		string labelA = *star->primaryAttribute[edge.first];
		string labelB = *star->primaryAttribute[edge.second];

		pair<string, string> labelPair(labelA, labelB);

		shared_ptr<vector<pair<int, int> > > edgeMatches = vejoint->pairMatches(labelPair);

		if (edgeMatches->size() > 0) {

			if (result->empty()) {
				for (auto i = edgeMatches->begin(); i != edgeMatches->end(); ++i) {
					MatchedComponent mc(edge, *i);
					result->push_back(mc);
				}
			}
			else {

				shared_ptr<vector<MatchedComponent> > newResult = make_shared<vector<MatchedComponent> >();

				for (auto i = edgeMatches->begin(); i != edgeMatches->end(); ++i) {

					for (auto j = result->begin(); j != result->end(); ++j) {

						if (j->canJoin(edge, *i)) {

							MatchedComponent tempNew(*j);

							tempNew.join(edge, *i);

							newResult->push_back(tempNew);

						}

					}
				}

				result = newResult;

			}

		}
		else {
			result->clear();
			return result;
		}

	}

	return result;
}

/**
 * We assume that the Star graphs stored in stars are already sorted, which means that they can be joined consecutively.
 */
shared_ptr<vector<MatchedComponent> > SubQuery::joinStar(vector<shared_ptr<Star> > stars, vector<shared_ptr<vector<MatchedComponent> > > matches) {
	shared_ptr<vector<MatchedComponent> > result = make_shared<vector<MatchedComponent> >();
	auto it = matches.begin();

	for (auto i = stars.begin(); i != stars.end(); ++i) {

		auto m = **it;

		if (result->empty()) {
			for (auto j = m.begin(); j != m.end(); ++j)
				result->push_back(*j);
		}
		else {

			shared_ptr<vector<MatchedComponent> > newResult = make_shared<vector<MatchedComponent> >();

			for (auto j = m.begin(); j != m.end(); ++j) {

				for (auto k = result->begin(); k != result->end(); k++) {

					if (j->canJoin(*k)) {

						MatchedComponent cp(*k);

						cp.join(*j);

						newResult->push_back(cp);

					}
				}

			}

			result = newResult;
		}

		it++;
	}

	return result;
}

shared_ptr<vector<MatchedComponent> > SubQuery::evaluate(shared_ptr<Graph> queryGraph) {
	vector<shared_ptr<Star> > stars = decomposeIntoStar(queryGraph);
//	for (auto i = stars.begin(); i != stars.end(); i++) {
//		(*i)->print();
//		cout << "*****************" << endl;
//	}

	vector<shared_ptr<vector<MatchedComponent> > > matches;

	//Need to sort star vector first

	for (auto i = stars.begin(); i != stars.end(); ++i) {
		shared_ptr<vector<MatchedComponent> > starResult = starQuery(*i);
//		for (auto j = starResult->begin(); j != starResult->end(); ++j) {
//			j->print();
//			cout << "~~~~~~~~~~~~~~~~~~~" << endl;
//		}

		matches.push_back(starResult);
	}

	shared_ptr<vector<MatchedComponent> > finalMatch = joinStar(stars, matches);

	return finalMatch;
}











