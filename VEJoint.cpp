/*
 * Joint.cpp
 *
 *  Created on: 2014
 *      Author: Administrator
 */
#include <VEJoint.h>
#include <Graph.h>
#include <utility>
#include <fstream>
#include <iostream>
#include <Triple.h>
using namespace std;

/**
 * Index the edges
 */
void VEJoint::IndexEdges(shared_ptr<Graph> graphPartition) {
	//Short name for the children in graph partition
	auto localChildren = graphPartition->children;

	//For all the vertices which have children
	for (auto i = localChildren.begin(); i != localChildren.end(); ++i) {

		//Get the label of the source vertex
		string labelA = *graphPartition->primaryAttribute[i->first];

		//For all the children
		for (auto j = localChildren[i->first]->begin(); j != localChildren[i->first]->end(); ++j) {
			//current child label
			string labelB = *graphPartition->primaryAttribute[*j];

			//Create the pairs
			pair<string, string> labelPair(labelA, labelB);
			pair<int, int> IDPair(i->first, *j);

			if (edgeIndex.find(labelPair) == edgeIndex.end()) {
				shared_ptr<vector<pair<int, int> > > IDList = make_shared<vector<pair<int, int> > >();
				edgeIndex.insert(make_pair(labelPair, IDList));
			}
			//Insert into the index
			edgeIndex[labelPair]->push_back(IDPair);
		}
	}
}

void VEJoint::IndexJoints(shared_ptr<Graph> graphPartition) {
	//short name for children
	auto localChildren = graphPartition->children;

	for (auto i = localChildren.begin(); i != localChildren.end(); ++i) {

		string labelA = *graphPartition->primaryAttribute[i->first];

		for (auto j = localChildren[i->first]->begin(); j != localChildren[i->first]->end(); ++j) {
			string labelB = *graphPartition->primaryAttribute[*j];

			//Check the global data graph to find the 2-hop children
			if (dataGraph->children.find(*j) != dataGraph->children.end()) {

				shared_ptr<vector<int> > secLvChildren = dataGraph->children[*j];

				for (auto k = secLvChildren->begin(); k != secLvChildren->end(); ++k) {

					//The label should be obtained from the data graph
					string labelC = *dataGraph->primaryAttribute[*k];

					//Make the triple
					triple<string, string, string> labelTri(labelA, labelB, labelC);
					triple<int, int, int> IDTri(i->first, *j, *k);

					if (jointIndex.find(labelTri) == jointIndex.end()) {
						shared_ptr<vector<triple<int, int, int> > > IDList = make_shared<vector<triple<int, int, int> > >();
						jointIndex.insert(make_pair(labelTri, IDList));
					}

					jointIndex[labelTri]->push_back(IDTri);
				}
			}
		}
	}
}

void VEJoint::outputIndex(string &filename) {
	ofstream ofs(filename + "Edge");
	if (!ofs) {
		cout << "The file cannot be open." << endl;
		return ;
	}

	for (auto i = edgeIndex.begin(); i != edgeIndex.end(); ++i) {
		pair<string, string> labelPair = i->first;
		ofs << labelPair.first << " " << labelPair.second << endl;
		shared_ptr<vector<pair<int, int> > > IDPairList = i->second;

		for (auto j = IDPairList->begin(); j != IDPairList->end(); ++j) {
			ofs << j->first << " " << j->second << " ";
		}
		ofs << endl;
	}

	ofs.close();

	ofstream ofs1(filename + "Joint");
	if (!ofs1) {
		cout << "The file cannot be open." << endl;
		return;
	}

	for (auto i = jointIndex.begin(); i != jointIndex.end(); ++i) {
		triple<string, string, string> labelTri = i->first;
		ofs1 << labelTri.first << " " << labelTri.second << " " << labelTri.third << endl;

		shared_ptr<vector<triple<int, int, int> > > IDTriList = i->second;

		for (auto j = IDTriList->begin(); j != IDTriList->end(); ++j) {
			ofs1 << j->first << " " << j->second << " " << j->third << " ";
		}
		ofs1 << endl;
	}

	ofs1.close();
}
