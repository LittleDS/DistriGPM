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
#include <set>
#include <algorithm>
using namespace std;

bool isIntersected(map<int, int> &a, map<int, int> &b) {
	for (auto i = a.begin(); i != a.end(); i++) {
		if (b.find(i->first) != b.end())
			return true;
	}
	return false;
}

bool isIntersected(vector<int> &a, vector<int> &b) {
	for (auto i = a.begin(); i != a.end(); i++) {
		for (auto j = b.begin(); j != b.end(); j++) {
			if (*i == *j)
				return true;
		}
	}
	return false;
}

vector<vector<int> > SubQuery::decomposeIntoPieces(shared_ptr<Graph> queryGraphInput) {

	vector<vector<int> > result;

	//Make a copy
	shared_ptr<Graph> queryGraph = make_shared<Graph>(*queryGraphInput);

//	set<int> cand;
//	for (auto i = queryGraph->primaryAttribute.begin(); i != queryGraph->primaryAttribute.end(); i++) {
//		if (queryGraph->outdegree[i->first] > 0 && queryGraph->indegree[i->first] > 0) {
//			cand.insert(i->first);
//		}
//	}
//
//	while (true) {
//		unsigned int min = 2147483647;
//		int w = -1, u = -1, v = -1;
//
//		for (auto i = cand.begin(); i != cand.end(); i++)
//			if (queryGraph->children.find(*i) != queryGraph->children.end() &&
//				queryGraph->parents.find(*i) != queryGraph->parents.end())
//			{
//				shared_ptr<vector<int> > parentsList = queryGraph->parents[*i];
//				shared_ptr<vector<int> > childrenList = queryGraph->children[*i];
//
//				int labelB = queryGraph->primaryAttribute[*i];
//
//				for (auto p = parentsList->begin(); p != parentsList->end(); p++) {
//					int labelA = queryGraph->primaryAttribute[*p];
//
//					for (auto c = childrenList->begin(); c != childrenList->end(); c++) {
//						int labelC = queryGraph->primaryAttribute[*c];
//						triple<int, int, int> labelTriple(labelA, labelB, labelC);
//						if (vejoint->jointIndex.find(labelTriple) != vejoint->jointIndex.end()) {
//							auto matches = vejoint->jointIndex[labelTriple];
//							if (matches->size() < min) {
//								min = matches->size();
//								w = *p;
//								u = *i;
//								v = *c;
//							}
//						}
//						else {
//							result.clear();
//							return result;
//						}
//					}
//				}
//			}
//
//		if (min != 2147483647 && u != -1) {
//			vector<int> tri;
//			tri.push_back(w);
//			tri.push_back(u);
//			tri.push_back(v);
//			result.push_back(tri);
//			queryGraph->removeEdge(w, u);
//			queryGraph->removeEdge(u, v);
//		}
//		else
//			break;
//	}

	for (auto i = queryGraph->children.begin(); i != queryGraph->children.end(); i++) {
		auto childrenList = i->second;
		if (childrenList->size() > 0) {
			for (auto j = childrenList->begin(); j != childrenList->end(); j++) {
				vector<int> pa;
				pa.push_back(i->first);
				pa.push_back(*j);
				result.push_back(pa);
			}
		}
	}


	//Sort result
	vector<vector<int> > sortedResult;
	auto first = result.begin();
	sortedResult.push_back(*first);
	result.erase(first);

	auto i = result.begin();

	while (result.size() > 0) {
		bool valid = false;

		for (auto j = sortedResult.begin(); j != sortedResult.end(); j++) {
			if (isIntersected(*j, *i)) {
				valid = true;
				break;
			}
		}

		if (valid) {
			sortedResult.push_back(*i);
			result.erase(i);
			i = result.begin();
		}
		else
			i++;

	}


//	//Random order
//	reverse(result.begin(), result.end());
//
//	vector<vector<int> > sortedResult;
//	auto first = result.begin();
//	sortedResult.push_back(*first);
//	result.erase(first);
//
//	auto i = result.begin();
//
//	while (result.size() > 0) {
//		bool valid = false;
//
//		for (auto j = sortedResult.begin(); j != sortedResult.end(); j++) {
//			if (isIntersected(*j, *i)) {
//				valid = true;
//				break;
//			}
//		}
//
//		if (valid) {
//			sortedResult.push_back(*i);
//			result.erase(i);
//			i = result.begin();
//		}
//		else
//			i++;
//
//	}
	return sortedResult;

}

vector<shared_ptr<Star> > SubQuery::decomposeIntoStar(shared_ptr<Graph> queryGraphInput) {
	vector<shared_ptr<Star>> result;

	shared_ptr<Graph> queryGraph = make_shared<Graph>(*queryGraphInput);

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
				star->addEdge(center, centerLabel, *i, childLabel);
				queryGraph->removeEdge(center, *i);
			}
		}

		if (queryGraph->parents.find(center) != queryGraph->parents.end()) {
			vector<int> plist(*queryGraph->parents[center]);
			for (auto i = plist.begin(); i != plist.end(); ++i) {
				auto parentLabel = queryGraph->primaryAttribute[*i];
				star->addEdge(*i, parentLabel, center, centerLabel);
				queryGraph->removeEdge(*i, center);
			}
		}

		result.push_back(star);
	}

	auto head = result.begin();
	while (head != result.end()) {
		auto current = head + 1;
		while (current != result.end()) {
			if (isIntersected((*head)->primaryAttribute, (*current)->primaryAttribute)) {
				iter_swap(head + 1, current);
				break;
			}
			current++;
		}
		head++;
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

		int labelA = star->primaryAttribute[joint.first];
		int labelB = star->primaryAttribute[joint.second];
		int labelC = star->primaryAttribute[joint.third];

		triple<int, int, int> labelTri(labelA, labelB, labelC);

		shared_ptr<vector<triple<int, int, int> > > jointMatches = vejoint->tripleMatches(labelTri);
//		for (auto i = jointMatches->begin(); i != jointMatches->end(); i++) {
//			cout << i->first << " " << i->second << " " << i->second << endl;
//		}
//		cout << "------------------" << endl;
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

		int labelA = star->primaryAttribute[edge.first];
		int labelB = star->primaryAttribute[edge.second];

		pair<int, int> labelPair(labelA, labelB);

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
shared_ptr<vector<MatchedComponent> > SubQuery::joinStar(const vector<shared_ptr<Star> > &stars, const vector<shared_ptr<vector<MatchedComponent> > > &matches) {
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


shared_ptr<vector<MatchedComponent> > SubQuery::evaluateOld(shared_ptr<Graph> queryGraph) {
	vector<vector<int> > pes = decomposeIntoPieces(queryGraph);
	shared_ptr<vector<MatchedComponent> > finalMatch = componentQuery(queryGraph, pes);

	return finalMatch;
}

shared_ptr<vector<MatchedComponent> > SubQuery::componentQuery(shared_ptr<Graph> queryGraphInput, vector<vector<int> > &pieces) {

	shared_ptr<vector<MatchedComponent> > result = make_shared<vector<MatchedComponent> >();

	for (auto p = pieces.begin(); p != pieces.end(); p++) {
		//using the label to fetch the matching candidates from VEjoint index
		if (p->size() == 3) {
			triple<int, int, int> joint((*p)[0], (*p)[1], (*p)[2]);

			bool isCycle = false;

			if (joint.first == joint.third)
				isCycle = true;

			int labelA = queryGraphInput->primaryAttribute[joint.first];
			int labelB = queryGraphInput->primaryAttribute[joint.second];
			int labelC = queryGraphInput->primaryAttribute[joint.third];

			triple<int, int, int> labelTri(labelA, labelB, labelC);

			shared_ptr<vector<triple<int, int, int> > > jointMatches = vejoint->tripleMatches(labelTri);
//			for (auto i = jointMatches->begin(); i != jointMatches->end(); i++) {
//				cout << i->first << " " << i->second << " " << i->third << endl;
//			}
//			cout << "------------------" << endl;

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
		else if (p->size() == 2) {

			pair<int, int> edge((*p)[0], (*p)[1]);

			int labelA = queryGraphInput->primaryAttribute[edge.first];
			int labelB = queryGraphInput->primaryAttribute[edge.second];

			pair<int, int> labelPair(labelA, labelB);

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

	}

	return result;
}


