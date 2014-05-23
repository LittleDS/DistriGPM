/*
 * Partition.cpp
 *
 *  Created on: Apr 19, 2014
 *      Author: Administrator
 */
#include <string>
#include <fstream>
#include <iostream>
#include <Partition.h>
#include <sstream>

using namespace std;

Partition::Partition() {

}

Partition::~Partition() {

}

void Partition::outputGraph(string &outname) {
	ofstream ofs(outname);
	if (!ofs) {
		cout << "File cannot be opened" << endl;
		return;
	}

	map<int, int>::iterator it;
	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {
		int core = corePart.find(it->first) == corePart.end() ? 0 : 1;
		ofs << it->first << " " << it->second << " " << core << endl;

		if (children.find(it->first) != children.end()) {
			for (auto i = children[it->first]->begin(); i != children[it->first]->end(); ++i) {
				ofs << *i << " ";
			}
			ofs << endl;
		}
		else
			ofs << "-1" << endl;
	}

	ofs.close();
}

void Partition::loadGraphFromFile(string &filename) {
	//Open the stream of input
	ifstream ifs(filename, ifstream::in);
	if (!ifs) {
		cout << "The input file doesn't exist!" << endl;
		return;
	}

	//Read a line from the data graph file
	string line;

	//Since every two lines in the file is a group
	while (getline(ifs, line)) {
		istringstream idline(line);

		//Read the vertex ID and the label
		int ID;
		idline >> ID;
		int label;
		idline >> label;
		int isCore;
		idline >> isCore;

		//Insert the value pair into the map
		primaryAttribute.insert({ID, label});
		if (isCore) corePart.insert(ID);

		//Read the children information line
		string childString;
		getline(ifs, childString);
		istringstream neighborline(childString);

		//The child ID
		int child;

		//Make the first time we create the children vector for current ID
		bool first = true;

		//The shared_ptr for the children list
		shared_ptr<vector<int> > childVector;

		//Read all the children in
		while (neighborline >> child) {
			if (child != -1) {

				//If it's the first time, initialize the children vector
				if (first) {
					childVector = make_shared<vector<int> >();
					children.insert({ID, childVector});
					first = false;
				}

				//Insert the current child back to the vector
				childVector->push_back(child);

				totalEdges++;

				//Check the parent and update the parent info
				if (parents.find(child) != parents.end()) {
					parents[child]->push_back(ID);
				}
				else {
					//Create the parents vector
					shared_ptr<vector<int> > parentVector = make_shared<vector<int> >();
					parentVector->push_back(ID);
					parents.insert({child, parentVector});
				}
			}
			else {
				break;
			}
		}

	}

	ifs.close();
}

void Partition::print() {
	map<int, int>::iterator it;
	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {
		cout << it->first << " " << it->second;
		if (corePart.find(it->first) == corePart.end())
			cout << " 1" << endl;
		else
			cout << " 0" << endl;

		cout << outdegree[it->first] << " " << indegree[it->first] << endl;
		map<int, shared_ptr<vector<int> > >::iterator cit = children.find(it->first);
		if (cit != children.end()) {
			cout << "Children:" << endl;
			for (int_vector_it i = cit->second->begin(); i != cit->second->end(); ++i) {
				cout << *i << " ";
			}
			cout << endl;
		}

		map<int, shared_ptr<vector<int> > >::iterator pit = parents.find(it->first);
		if (pit != parents.end()) {
			cout << "Parents:" << endl;
			for (int_vector_it i = pit->second->begin(); i != pit->second->end(); ++i) {
				cout << *i << " ";
			}
			cout << endl;
		}
	}
}
