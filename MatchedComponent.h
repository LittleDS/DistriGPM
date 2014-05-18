/*
 * MatchedComponent.h
 *
 *  Created on: May 2, 2014
 *      Author: Administrator
 */

#ifndef MATCHEDCOMPONENT_H_
#define MATCHEDCOMPONENT_H_

#include <map>
#include <set>
#include <Triple.h>
#include <utility>

using namespace std;

class MatchedComponent {
private:
	map<int, int> mapping;
	set<int> valueSet;
public:
	MatchedComponent(triple<int, int, int> q, triple<int, int,int> m) {
		mapping.insert(make_pair(q.first, m.first));
		valueSet.insert(m.first);

		mapping.insert(make_pair(q.second, m.second));
		valueSet.insert(m.second);

		//Avoid insert a redundant element into the map
		if (mapping.find(q.third) == mapping.end()) {
			mapping.insert(make_pair(q.third, m.third));
			valueSet.insert(m.third);
		}
	}

	MatchedComponent(pair<int, int> q, pair<int, int> m) {
		mapping.insert(make_pair(q.first, m.first));
		valueSet.insert(m.first);

		mapping.insert(make_pair(q.second, m.second));
		valueSet.insert(m.second);
	}

	MatchedComponent(const MatchedComponent &another) {
		mapping.insert(another.mapping.begin(), another.mapping.end());
		valueSet.insert(another.valueSet.begin(), another.valueSet.end());
	}

	bool canJoin(pair<int, int> q, pair<int, int> m);
	bool canJoin(triple<int, int, int> q, triple<int, int, int> m);
	bool canJoin(MatchedComponent &another);

	void join(pair<int, int> q, pair<int, int> m);
	void join(triple<int, int, int> q, triple<int, int, int> m);
	void join(MatchedComponent &another);

	inline void combine(MatchedComponent &another) {
		mapping.insert(another.mapping.begin(), another.mapping.end());
		valueSet.insert(another.valueSet.begin(), another.valueSet.end());
	}

	void print();
	~MatchedComponent() { }
};



#endif /* MATCHEDCOMPONENT_H_ */
