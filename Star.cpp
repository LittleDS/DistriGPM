/*
 * Star.cpp
 *
 *  Created on: May 21, 2014
 *      Author: Administrator
 */

#include <Star.h>

using namespace std;

shared_ptr<vector<int> > Star::serialize() {
	shared_ptr<vector<int> > re = make_shared<vector<int> >();
	re->push_back(center);

	re->push_back(primaryAttribute.size());
	for (auto i = primaryAttribute.begin(); i != primaryAttribute.end(); ++i) {
		re->push_back(i->first);
		re->push_back(i->second);
	}

	int count = 0;
	for (auto i = children.begin(); i != children.end(); i++) {
		count += i->second->size();
	}

	re->push_back(count);

	for (auto i = children.begin(); i != children.end(); i++) {
		for (auto j = i->second->begin(); j != i->second->end(); j++) {
			re->push_back(i->first);
			re->push_back(*j);
		}
	}

	return re;
}

void Star::deseralize(shared_ptr<vector<int> > input) {
	auto i = input->begin();
	center = *i;
	i++;

	int n = *i;
	i++;

	for (int j = 0; j < n; j++) {
		int ID = *i;
		i++;
		int label = *i;
		i++;
		primaryAttribute.insert(make_pair(ID, label));
	}

	int E = *i;
	i++;

	for (int j = 0; j < E; j++) {
		int source = *i;
		i++;
		int target = *i;
		i++;
		addEdge(source, primaryAttribute[source], target, primaryAttribute[target]);
	}
}

void Star::deseralize(vector<int>::iterator &it) {
	center = *it;
	it++;

	int n = *it;
	it++;

	for (int j = 0; j < n; j++) {
		int ID = *it;
		it++;
		int label = *it;
		it++;
		primaryAttribute.insert(make_pair(ID, label));
	}

	int E = *it;
	it++;

	for (int j = 0; j < E; j++) {
		int source = *it;
		it++;
		int target = *it;
		it++;
		addEdge(source, primaryAttribute[source], target, primaryAttribute[target]);
	}

}

