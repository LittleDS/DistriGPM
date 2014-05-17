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
#include <sstream>
#include <iostream>
#include <Triple.h>
using namespace std;

/**
 * Index the edges
 */
void VEJoint::indexEdges() {
	//Check whether the data graph is loaded
	if (graphPartition == NULL) {
		cout << "The data graph is not loaded." << endl;
		return;
	}

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

/**
 * Index the joints
 */
void VEJoint::indexJoints() {
	//Check whether the data graph is loaded
	if (graphPartition == NULL) {
		cout << "The data graph is not loaded." << endl;
		return;
	}
	totalHubs = 0;
	//short name for children
	auto localChildren = graphPartition->children;
	auto localParents = graphPartition->parents;

	for (auto i = localChildren.begin(); i != localChildren.end(); ++i) {
		if (localParents.find(i->first) == localParents.end())
			continue;

		string labelA = *graphPartition->primaryAttribute[i->first];

		auto outNBSize =  localChildren[i->first]->size();
		auto inNBSize = localParents[i->first]->size();

		//If this is a hub, then we need to take care of it specially
		if (outNBSize * inNBSize >= HUB_THRESHOLD) {
			//Set the flag to be true
			//it's used to output the index files to disk
			hasHub = true;
			totalHubs++;
			//Process the out neighbors of this hub
			if (hubOutNeighbors.find(i->first) == hubOutNeighbors.end()) {
				shared_ptr<map<string, shared_ptr<vector<int> > > > nbs = make_shared<map<string, shared_ptr<vector<int> > > >();
				hubOutNeighbors.insert(make_pair(i->first, nbs));
			}
			auto outNBLabelMap = hubOutNeighbors[i->first];
			for (auto j = localChildren[i->first]->begin(); j != localChildren[i->first]->end(); ++j) {
				string childLabel = *graphPartition->primaryAttribute[*j];
				if (outNBLabelMap->find(childLabel) == outNBLabelMap->end()) {
					shared_ptr<vector<int> > childrenWithSameLabels= make_shared<vector<int> >();
					outNBLabelMap->insert(make_pair(childLabel, childrenWithSameLabels));
				}

				(*outNBLabelMap)[childLabel]->push_back(*j);
			}


			//Process the in neighbors of this hub
			if (hubInNeighbors.find(i->first) == hubInNeighbors.end()) {
				shared_ptr<map<string, shared_ptr<vector<int> > > > nbs = make_shared<map<string, shared_ptr<vector<int> > > >();
				hubInNeighbors.insert(make_pair(i->first, nbs));
			}
			auto inNBLabelMap = hubInNeighbors[i->first];
			for (auto j = localParents[i->first]->begin(); j != localParents[i->first]->end(); ++j) {
				string parentLabel = *graphPartition->primaryAttribute[*j];
				if (inNBLabelMap->find(parentLabel) == inNBLabelMap->end()) {
					shared_ptr<vector<int> > parentsWithSameLabels= make_shared<vector<int> >();
					inNBLabelMap->insert(make_pair(parentLabel, parentsWithSameLabels));
				}

				(*inNBLabelMap)[parentLabel]->push_back(*j);
			}

		}
		else {
			for (auto j = localChildren[i->first]->begin(); j != localChildren[i->first]->end(); ++j) {
				string labelB = *graphPartition->primaryAttribute[*j];

				for (auto k = localParents[i->first]->begin(); k != localParents[i->first]->end(); ++k) {

					//The label should be obtained from the data graph
					string labelC = *graphPartition->primaryAttribute[*k];

					//Make the triple
					triple<string, string, string> labelTri(labelC, labelA, labelB);
					triple<int, int, int> IDTri(*k, i->first, *j);

					if (jointIndex.find(labelTri) == jointIndex.end()) {
						shared_ptr<vector<triple<int, int, int> > > IDList = make_shared<vector<triple<int, int, int> > >();
						jointIndex.insert(make_pair(labelTri, IDList));
					}

					jointIndex[labelTri]->push_back(IDTri);
				}
			}
		}
	}
	//cout << totalHubs << endl;
}

/**
 * Output the index to files
 */
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

	if (hasHub) {
		ofstream ofs2(filename + "Hubs");
		if (!ofs2) {
			cout << "The file cannot be open." << endl;
			return;
		}

		for (auto i = hubOutNeighbors.begin(); i != hubOutNeighbors.end(); ++i) {
			ofs2 << i->first << " " << *graphPartition->primaryAttribute[i->first] << " "
			     << i->second->size() << " " << hubInNeighbors[i->first]->size() << endl;
			auto outLabels = i->second;
			for (auto j = outLabels->begin(); j != outLabels->end(); j++) {
				ofs2 << j->first << endl;
				for (auto k = j->second->begin(); k != j->second->end(); k++) {
					ofs2 << *k << " ";
				}
				ofs2 << endl;
			}

			auto inLabels = hubInNeighbors[i->first];
			for (auto j = inLabels->begin(); j != inLabels->end(); j++) {
				ofs2 << j->first << endl;
				for (auto k = j->second->begin(); k != j->second->end(); k++) {
					ofs2 << *k << " ";
				}
				ofs2 << endl;
			}

		}

		ofs2.close();
	}
}

/**
 * Load the index from file
 */
void VEJoint::loadIndex(string &filename) {
	ifstream ifs(filename + "Edge");
	if (!ifs) {
		cout << "ERROR: Opening the file." << endl;
		return;
	}

	string line;
	while (getline(ifs, line)) {
		istringstream labelline(line);
		string labelA, labelB;
		labelline >> labelA >> labelB;
		pair<string, string> labelPair(labelA, labelB);

		getline(ifs, line);
		istringstream idline(line);

		shared_ptr<vector<pair<int, int> > > ids = make_shared<vector<pair<int, int> > >();
		int idA, idB;

		while (idline >> idA) {
			idline >> idB;
			pair<int, int> idPair(idA, idB);
			ids->push_back(idPair);
		}
		edgeIndex.insert(make_pair(labelPair, ids));
	}

	ifs.close();

	ifstream ifs1(filename + "Joint");
	if (!ifs1) {
		cout << "ERROR: Opening the file." << endl;
		return;
	}

	while (getline(ifs1, line)) {
		istringstream labelline(line);
		string labelA, labelB, labelC;
		labelline >> labelA >> labelB >> labelC;
		triple<string, string, string> labelTri(labelA, labelB, labelC);

		getline(ifs1, line);
		istringstream idline(line);

		shared_ptr<vector<triple<int, int, int> > > ids = make_shared<vector<triple<int, int, int> > >();

		int idA, idB, idC;

		while (idline >> idA) {
			idline >> idB >> idC;
			triple<int, int, int> idTri(idA, idB, idC);
			ids->push_back(idTri);
		}

		jointIndex.insert(make_pair(labelTri, ids));
	}

	ifs1.close();
}

/**
 * Load the hub information from disk and build the hubMap
 */
void VEJoint::loadIndexWithHub(string &filename) {
	hasHub = true;
	loadIndex(filename);

	totalHubs = 0;

	ifstream ifs(filename + "Hubs");
	if (!ifs) {
		cout << "ERROR: Opening the file." << endl;
		return;
	}

	string line;
	while (getline(ifs, line)) {
		istringstream IDLine(line);
		int ID, outdegree, indegree;
		string label;
		IDLine >> ID >> label >> outdegree >> indegree;

		//Build the hubsMap
		if (hubsMap.find(label) == hubsMap.end()) {
			shared_ptr<vector<int> > l = make_shared<vector<int > >();
			hubsMap.insert(make_pair(label, l));
		}
		hubsMap[label]->push_back(ID);
		totalHubs++;

		shared_ptr<map<string, shared_ptr<vector<int> > > > outAnagrams = make_shared<map<string, shared_ptr<vector<int> > > >();
		hubOutNeighbors.insert(make_pair(ID, outAnagrams));
		for (int i = 0; i < outdegree; i++) {
			//Read the label first
			getline(ifs, line);
			string label(line);

			//Create the item in the map then
			shared_ptr<vector<int> > neighborIDs = make_shared<vector<int> >();
			outAnagrams->insert(make_pair(label, neighborIDs));

			getline(ifs, line);
			istringstream IDs(line);
			int neighborID;
			while (IDs >> neighborID) {
				neighborIDs->push_back(neighborID);
			}
		}

		shared_ptr<map<string, shared_ptr<vector<int> > > > inAnagrams = make_shared<map<string, shared_ptr<vector<int> > > >();
		hubInNeighbors.insert(make_pair(ID, inAnagrams));
		for (int i = 0; i < indegree; i++) {
			//Do the same thing as out neighbors
			getline(ifs, line);
			string label(line);

			shared_ptr<vector<int> > neighborIDs = make_shared<vector<int> >();
			inAnagrams->insert(make_pair(label, neighborIDs));

			getline(ifs, line);
			istringstream IDs(line);
			int neighborID;
			while (IDs >> neighborID) {
				neighborIDs->push_back(neighborID);
			}
		}
	}

	ifs.close();
}

shared_ptr<vector<triple<int, int, int> > > VEJoint::tripleMatches(triple<string, string, string> labels) {
	shared_ptr<vector<triple<int, int, int> > > result;
	if (jointIndex.find(labels) != jointIndex.end()) {
		 result = make_shared<vector<triple<int, int, int> > >(*jointIndex[labels]);
	}
	else {
		result = make_shared<vector<triple<int, int, int> > >();
	}

	string midLabel = labels.second;
	if (hubsMap.find(midLabel) != hubsMap.end()) {
		shared_ptr<vector<int> > hubs = hubsMap[midLabel];
		for (auto i = hubs->begin(); i != hubs->end(); i++) {
			shared_ptr<map<string, shared_ptr<vector<int> > > > outChildren = hubOutNeighbors[*i];
			shared_ptr<map<string, shared_ptr<vector<int> > > > inParents = hubInNeighbors[*i];
			shared_ptr<vector<int> > outChildrenWithLabelC;
			if (outChildren->find(labels.third) != outChildren->end())
				outChildrenWithLabelC = (*outChildren)[labels.third];

			shared_ptr<vector<int> > inParentsWithLabelA;
			if (inParents->find(labels.first) != inParents->end())
				inParentsWithLabelA = (*inParents)[labels.first];

			if (outChildrenWithLabelC != NULL && inParentsWithLabelA != NULL ) {
				for (auto j = inParentsWithLabelA->begin(); j != inParentsWithLabelA->end(); ++j) {
					for (auto k = outChildrenWithLabelC->begin(); k != outChildrenWithLabelC->end(); ++k) {
						triple<int, int, int> ids(*j, *i, *k);
						result->push_back(ids);
					}
				}
			}
		}
	}

	return result;
}

shared_ptr<vector<pair<int, int> > > VEJoint::pairMatches(pair<string, string> labels) {
	shared_ptr<vector<pair<int, int> > >result;
	if (edgeIndex.find(labels) != edgeIndex.end()) {
		result = make_shared<vector<pair<int, int> > >(*edgeIndex[labels]);
	}
	else
		result = make_shared<vector<pair<int, int> > >();

	return result;
}
