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
	const int center;
	Star(int _center):center(_center) { }
	~Star() { }
private:
	Star():center(-1) { }

};



#endif /* STAR_H_ */
