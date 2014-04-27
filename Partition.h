/*
 * Partition.h
 *
 *  Created on: Apr 19, 2014
 *      Author: Administrator
 */

#ifndef PARTITION_H_
#define PARTITION_H_

#include <Graph.h>
#include <set>

class Partition: public Graph {
public:
	set<int> corePart;
	Partition();
	//The subclass method for outputGraph to file
	//The difference is that it also output a mark for each vertex to show whether it's in core or not
	void outputGraph(string &outname);

	//Corresponding to the different file format of partition, the loadGraphFromFile is overrided
    void loadGraphFromFile(string &filename);

    //Print the partition to console for test purpose
    void print();
	~Partition();
};




#endif /* PARTITION_H_ */
