/*
 * QueryGenerator.cpp
 *
 *  Created on: May 17, 2014
 *      Author: Administrator
 */

#include <QueryGenerator.h>
#include <Graph.h>
#include <Star.h>
#include <time.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

shared_ptr<Graph> QueryGenerator::generateSubgraphQuery() {

	vector<pair<int, int> > edgepool;
	vector<pair<int, int> > selectedEdges;

	int startVertex, endVertex;
	srand(time(NULL));

	int N = keySetofChildren.size();
	startVertex = keySetofChildren[rand() % N];
	vector<int> childrenOfStartVertex = *(dataGraph->children[startVertex]);
	int M = childrenOfStartVertex.size();
	endVertex = childrenOfStartVertex[rand() % M];
	pair<int, int> newEdge(startVertex, endVertex);

	selectedEdges.push_back(newEdge);


	//Add all the children of startVertex to the edgepool
	for (auto i = childrenOfStartVertex.begin(); i != childrenOfStartVertex.end(); i++) {
		pair<int, int> t(startVertex, *i);
		if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
			edgepool.push_back(t);
	}

	//Add all the children of endVertex to the edgepool
	if (dataGraph->children.find(endVertex) != dataGraph->children.end()) {
		vector<int> childrenOfEndVertex = *(dataGraph->children[endVertex]);
		for (auto i = childrenOfEndVertex.begin(); i != childrenOfEndVertex.end(); ++i) {
			pair<int, int> t(endVertex, *i);
			if (find(edgepool.begin(), edgepool.end(),t) == edgepool.end())
				edgepool.push_back(t);
		}
	}

	//Add all the parents of the startVertex to the edgepool
	if (dataGraph->parents.find(startVertex) != dataGraph->parents.end()) {
		vector<int> parentsOfStartVeretx = *(dataGraph->parents[startVertex]);
		for (auto i = parentsOfStartVeretx.begin(); i != parentsOfStartVeretx.end(); ++i) {
			pair<int, int> t(*i, startVertex);
			if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
				edgepool.push_back(t);
		}
	}

	//Add all the parents of the endVertex to the edgepool
	if (dataGraph->parents.find(endVertex) != dataGraph->parents.end()) {
		vector<int> parentsOfEndVertex = *(dataGraph->parents[endVertex]);
		for (auto i = parentsOfEndVertex.begin(); i != parentsOfEndVertex.end(); ++i) {
			pair<int, int> t(*i, endVertex);
			if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
				edgepool.push_back(t);
		}
	}

	unsigned int edgeCount = 1;
	while (edgeCount < componentSize) {
		int pos = rand() % edgepool.size();
		pair<int, int> nextEdge = edgepool[pos];
		//Add the new edge to the graph
		if (find(selectedEdges.begin(), selectedEdges.end(), nextEdge) == selectedEdges.end()) {
			edgeCount++;
			selectedEdges.push_back(nextEdge);

			//Update the edgepool
			if (dataGraph->children.find(nextEdge.first) != dataGraph->children.end()) {
				vector<int> childrenOfSourceVertex = *(dataGraph->children[nextEdge.first]);
				for (auto i = childrenOfSourceVertex.begin(); i != childrenOfSourceVertex.end(); i++) {
					pair<int, int> t(nextEdge.first, *i);
					if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
						edgepool.push_back(t);
				}
			}

			if (dataGraph->children.find(nextEdge.second) != dataGraph->children.end()) {
				vector<int> childrenOfTargetVertex = *(dataGraph->children[nextEdge.second]);
				for (auto i = childrenOfTargetVertex.begin(); i != childrenOfTargetVertex.end(); i++) {
					pair<int, int> t(nextEdge.second, *i);
					if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
						edgepool.push_back(t);
				}
			}

			if (dataGraph->parents.find(nextEdge.first) != dataGraph->parents.end()) {
				vector<int> parentsOfSourceVertex = *(dataGraph->parents[nextEdge.first]);
				for (auto i = parentsOfSourceVertex.begin(); i != parentsOfSourceVertex.end(); ++i) {
					pair<int, int> t(*i, nextEdge.first);
					if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
						edgepool.push_back(t);
				}
			}

			if (dataGraph->parents.find(nextEdge.second) != dataGraph->parents.end()) {
				vector<int> parentsOfTargetVertex = *(dataGraph->parents[nextEdge.second]);
				for (auto i = parentsOfTargetVertex.begin(); i != parentsOfTargetVertex.end(); i++) {
					pair<int, int> t(*i, nextEdge.second);
					if (find(edgepool.begin(), edgepool.end(), t) == edgepool.end())
						edgepool.push_back(t);
				}
			}
		}

	}

	shared_ptr<Graph> result = make_shared<Graph>();
	for (auto i = selectedEdges.begin(); i != selectedEdges.end(); i++) {
		result->addEdge(i->first, *(dataGraph->primaryAttribute[i->first]),
				        i->second, *(dataGraph->primaryAttribute[i->second]));
	}
	return result;
}
shared_ptr<Star> QueryGenerator::generateStarQuery() {
	vector<pair<int, int> > edgepool;
	vector<pair<int, int> > selectedEdges;

	int centerVertex;
	srand(time(NULL));

	//Pick up the center vertex first
	int N = keySetofChildren.size();
	while (true) {
		centerVertex = keySetofChildren[rand() % N];
		if (dataGraph->parents.find(centerVertex) != dataGraph->parents.end()) {
			if (dataGraph->children[centerVertex]->size() +
				dataGraph->parents[centerVertex]->size() >= componentSize) {

				for (auto i = dataGraph->children[centerVertex]->begin(); i != dataGraph->children[centerVertex]->end(); ++i) {
					pair<int, int> t(centerVertex, *i);
					edgepool.push_back(t);
				}

				for (auto i = dataGraph->parents[centerVertex]->begin(); i != dataGraph->parents[centerVertex]->end(); ++i) {
					pair<int, int> t(*i, centerVertex);
					edgepool.push_back(t);
				}

				break;
			}
		}
	}


	//Select componentSize edges from the edgepool
	random_shuffle(edgepool.begin(), edgepool.end());

	for (unsigned int i = 0; i < componentSize; i++) {
		selectedEdges.push_back(edgepool[i]);
	}

	shared_ptr<Star> result = make_shared<Star>(centerVertex);
	for (auto i = selectedEdges.begin(); i != selectedEdges.end(); i++) {
		result->addEdge(i->first, *(dataGraph->primaryAttribute[i->first]),
				        i->second, *(dataGraph->primaryAttribute[i->second]));
	}
	return result;
}

void QueryGenerator::DFSPath(int currentVertex, shared_ptr<vector<int> > Path) {
	if (Path->size() == componentSize)
		return;

	if (dataGraph->children.find(currentVertex) == dataGraph->children.end())
		return;

	for (auto i = dataGraph->children[currentVertex]->begin(); i != dataGraph->children[currentVertex]->end(); ++i) {
		if (visitedNode.find(*i) == visitedNode.end()) {
			Path->push_back(*i);
			visitedNode.insert(*i);
			DFSPath(*i, Path);
			if (Path->size() == componentSize)
				return;
			Path->pop_back();
			visitedNode.erase(*i);
		}
	}
	return;
}

shared_ptr<Graph> QueryGenerator::generatePathQuery() {

	int startVertex;
	srand(time(NULL));
	shared_ptr<Graph> result = make_shared<Graph>();

	int N = keySetofChildren.size();
	while (true) {
		startVertex = keySetofChildren[rand() % N];
		shared_ptr<vector<int> > generatedPath = make_shared<vector<int> >();
		visitedNode.clear();
		visitedNode.insert(startVertex);
		generatedPath->push_back(startVertex);
		DFSPath(startVertex, generatedPath);
		if (generatedPath->size() == componentSize) {

			for (unsigned int i = 0; i < componentSize - 1; ++i) {
				int s = (*generatedPath)[i];
				int t = (*generatedPath)[i + 1];
				result->addEdge(s, *(dataGraph->primaryAttribute[s]),
						        t, *(dataGraph->primaryAttribute[t]));
			}

			break;
		}
	}

	return result;
}



