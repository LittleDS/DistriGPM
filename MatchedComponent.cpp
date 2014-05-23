/*
 * MatchedComponent.cpp
 *
 *  Created on: May 2, 2014
 *      Author: Administrator
 */

#include <MatchedComponent.h>
#include <map>
#include <utility>
#include <iostream>

using namespace std;

bool MatchedComponent::canJoin(pair<int, int> q, pair<int, int> m) {
	//First check if the key already exists in the mapping table
	if (mapping.find(q.first) != mapping.end()) {
		//If it already exists, check whether the corresponding value matches
		if (mapping[q.first] != m.first)
			return false;
	}
	else { //If the key doesn't exist, check if the value is already be matched with other keys
		if (valueSet.find(m.first) != valueSet.end())
			return false;
	}

	if (mapping.find(q.second) != mapping.end()) {
		if (mapping[q.second] != m.second)
			return false;
	}
	else {
		if (valueSet.find(m.second) != valueSet.end())
			return false;
	}

	return true;
}

bool MatchedComponent::canJoin(triple<int, int, int> q, triple<int, int, int> m) {
	if (mapping.find(q.first) != mapping.end()) {
		if (mapping[q.first] != m.first)
			return false;
	}
	else {
		if (valueSet.find(m.first) != valueSet.end())
			return false;
	}

	if (mapping.find(q.second) != mapping.end()) {
		if (mapping[q.second] != m.second)
			return false;
	}
	else {
		if (valueSet.find(m.second) != valueSet.end())
			return false;
	}

	if (mapping.find(q.third) != mapping.end()) {
		if (mapping[q.third] != m.third)
			return false;
	}
	else {
		if (valueSet.find(m.third) != valueSet.end())
			return false;
	}

	return true;
}

bool MatchedComponent::canJoin(MatchedComponent &another) {
	for (auto i = mapping.begin(); i != mapping.end(); i++) {
		if (another.mapping.find(i->first) != another.mapping.end()) {
			if (another.mapping[i->first] != i->second)
				return false;
		}
		else {
			if (another.valueSet.find(i->second) != another.valueSet.end())
				return false;
		}
	}
	return true;
}

void MatchedComponent::join(pair<int, int> q, pair<int, int> m) {
	mapping.insert(make_pair(q.first, m.first));
	valueSet.insert(m.first);

	mapping.insert(make_pair(q.second, m.second));
	valueSet.insert(m.second);
}

void MatchedComponent::join(triple<int, int, int> q, triple<int, int, int> m) {
	mapping.insert(make_pair(q.first, m.first));
	valueSet.insert(m.first);

	mapping.insert(make_pair(q.second, m.second));
	valueSet.insert(m.second);

	mapping.insert(make_pair(q.third, m.third));
	valueSet.insert(m.third);
}

void MatchedComponent::join(MatchedComponent &another) {
	for (auto i = another.mapping.begin(); i != another.mapping.end(); ++i) {
		mapping.insert(make_pair(i->first, i->second));
		valueSet.insert(i->second);
	}
}

void MatchedComponent::print() {
	for (auto i = mapping.begin(); i != mapping.end(); i++) {
		cout << i->first << " " << i->second << endl;
	}
}

shared_ptr<vector<int> > MatchedComponent::serialize() {
	shared_ptr<vector<int> > result = make_shared<vector<int> >();
	result->push_back(mapping.size() * 2);
	for (auto i = mapping.begin(); i != mapping.end(); i++) {
		result->push_back(i->first);
		result->push_back(i->second);
	}
	return result;
}

void MatchedComponent::deseralize(shared_ptr<vector<int> > input) {
	vector<int>::iterator it = input->begin();
	int N = *it;
	it++;
	for (int i = 0; i < N / 2; i++) {
		int first = *it;
		it++;
		int second = *it;
		it++;
		mapping.insert(make_pair(first, second));
		valueSet.insert(second);
	}
}

void MatchedComponent::deseralize(vector<int>::iterator &it) {
	int N = *it;
	it++;
	for (int i = 0; i < N / 2; i++) {
		int first = *it;
		it++;
		int second = *it;
		it++;
		mapping.insert(make_pair(first, second));
		valueSet.insert(second);
	}
}
