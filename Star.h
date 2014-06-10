/*
 * Star.h
 *
 *  Created on: May 4, 2014
 *      Author: Administrator
 */

#ifndef STAR_H_
#define STAR_H_

#include <Graph.h>

class Star: public Graph {

public:
	int center;
	Star():center(-1) { }
	Star(int _center):center(_center) { }
	~Star() { }

	shared_ptr<vector<int> > serialize();
	void deseralize(shared_ptr<vector<int> > input);
	void deseralize(vector<int>::iterator &it);
};



#endif /* STAR_H_ */
