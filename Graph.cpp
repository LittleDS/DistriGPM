/*
 * Graph.cpp
 *
 *  Created on: Mar 23, 2014
 *      Author: Lei Yang
 */
#include <Graph.h>
#include <Partition.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <algorithm>

using namespace std;
//Default constructor
Graph::Graph():totalEdges(0) { }

Graph::Graph(const Graph &another):
	totalEdges(another.totalEdges),
	primaryAttribute(another.primaryAttribute),
	children(another.children),
	parents(another.parents),
	indegree(another.indegree),
	outdegree(another.outdegree)
	{
    }


void Graph::loadGraphFromFile(string &filename) {
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
		string label;
		idline >> label;

		//Insert the value pair into the map
		primaryAttribute.insert({ID, make_shared<string>(label)});

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


void Graph::addEdge(int s, string &aS, int t, string &aT) {

	if (primaryAttribute.find(s) == primaryAttribute.end()) {
		shared_ptr<string> ts = make_shared<string>(aS);
		primaryAttribute.insert({s, ts});
	}
	else {
		string tempS = *primaryAttribute[s];
		if (tempS != aS) {
			cout << "The label doesn't match for source vertex" << endl;
			return;
		}
	}

	if (primaryAttribute.find(t) == primaryAttribute.end()) {
		shared_ptr<string> ts = make_shared<string>(aT);
		primaryAttribute.insert({t, ts});
	}
	else {
		string tempT = *primaryAttribute[t];
		if (tempT != aT) {
			cout << "The label doesn't match for target vertex" << endl;
			return;
		}
	}

	if (children.find(s) != children.end()) {
		if (find(children[s]->begin(), children[s]->end(), t) == children[s]->end()) {
			children[s]->push_back(t);
			outdegree[s]++;
		}
	}
	else {
		shared_ptr<vector<int> > childVector = make_shared<vector<int> >();
		childVector->push_back(t);
		children.insert({s, childVector});
		outdegree.insert({s, 1});
	}

	if (parents.find(t) != parents.end()) {
		if (find(parents[t]->begin(), parents[t]->end(), t) == parents[t]->end()) {
			parents[t]->push_back(s);
			indegree[t]++;
		}
	}
	else {
		shared_ptr<vector<int> > parentVector = make_shared<vector<int> >();
		parentVector->push_back(s);
		parents.insert({t, parentVector});
		indegree.insert({t, 1});
	}

}

void Graph::removeEdge(int s, int t) {
	if (primaryAttribute.find(s) == primaryAttribute.end() ||
		primaryAttribute.find(t) == primaryAttribute.end())
		return;

	//Remove t from the children list of s
	if (children.find(s) == children.end())
		return;

	bool found = false;

	auto clist = children[s];
	for (auto i = clist->begin(); i != clist->end(); ++i) {
		if (*i == t) {
			clist->erase(i);
			outdegree[s]--;
			found = true;
			break;
		}
	}

	if (clist->size() == 0)
		children.erase(s);

	//Remove s from the parents list of t
	if (parents.find(t) == parents.end())
		return;

	auto flist = parents[t];
	for (auto i = flist->begin(); i != flist->end(); ++i) {
		if (*i == s) {
			flist->erase(i);
			indegree[t]--;
			break;
		}
	}

	if (flist->size() == 0)
		parents.erase(t);

	if (found)
		totalEdges--;
}
/**
 * Calculate the indegree and outdegree according to the children and parents table
 */
void Graph::calculateDegree() {
	indegree.clear();
	outdegree.clear();
	map<int, shared_ptr<string> >::iterator it;
	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {
		//Calculate the out-degree
		if (children.find(it->first) != children.end()) {
			outdegree.insert({it->first, children[it->first]->size()});
		}
		else {
			outdegree.insert({it->first, 0});
		}

		//Then calculate the in-degree
		if (parents.find(it->first) != parents.end()) {
			indegree.insert({it->first, parents[it->first]->size()});
		}
		else {
			indegree.insert({it->first, 0});
		}
	}
}

/**
 * Merge another graph into the current graph
 * The another graph must be disjoint with this graph
 * We don't do the redundancy check here
 */
void Graph::Combine(Graph &another) {
	map<int, shared_ptr<string> >::iterator it;

	for (it = another.primaryAttribute.begin(); it != another.primaryAttribute.end(); ++it) {
		if (primaryAttribute.find(it->first) == primaryAttribute.end()) {
			primaryAttribute.insert({it->first, it->second});
		}

		//Add all the children in
		if (another.children.find(it->first) != another.children.end()) {
			if (children.find(it->first) == children.end()) {
				shared_ptr<vector<int> > childVector = make_shared<vector<int> >();
				children.insert({it->first, childVector});
			}

			auto cit = another.children[it->first];
			for (int_vector_it i = cit->begin(); i != cit->end(); ++i) {
				children[it->first]->push_back(*i);
			}

		}

		//Add all the parents in
		if (another.parents.find(it->first) != another.parents.end()) {
			if (parents.find(it->first) == parents.end()) {
				shared_ptr<vector<int> > parentVector = make_shared<vector<int> >();
				parents.insert({it->first, parentVector});
			}

			auto pit = another.parents[it->first];
			for (int_vector_it i = pit->begin(); i != pit->end(); ++i) {
				parents[it->first]->push_back(*i);
			}
		}
	}

	totalEdges += another.totalEdges;
	indegree.clear();
	outdegree.clear();
	calculateDegree();
}

/**
 * Print the graph structure for test purpose
 */
void Graph::print() {
	map<int, shared_ptr<string> >::iterator it;
	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {
		cout << it->first << " " << (*(it->second)) << endl;
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

/**
 *
 */
void Graph::outputGraph(string &outname) {
	ofstream ofs(outname);
	if (!ofs) {
		cout << "File cannot be opened" << endl;
		return;
	}

	map<int, shared_ptr<string> >::iterator it;
	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {
		ofs << it->first << " " << *(it->second) <<endl;
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

/**
 * Output the graph in the serial ParMETIS format
 */
void Graph::outputParMETIS(string &filename) {
	ofstream ofs(filename);
	if (!ofs) {
		cout << "File cannot be opened" << endl;
		return;
	}

	//Output the number of vertices and edges first
	ofs << primaryAttribute.size() << " " << totalEdges << endl;

	//Output the adjacency list of the graph and transfer the graph into undirected version
	map<int, shared_ptr<string> >::iterator it;
	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {

		//In order to remove the two-vertex cycle, we add all the neighbors of a vertex into a set
		//and output this set directly
		set<int> neighbor;
		if (children.find(it->first) != children.end()) {
			for (auto i = children[it->first]->begin(); i != children[it->first]->end(); ++i) {
				neighbor.insert(*i);
			}
		}

		if (parents.find(it->first) != parents.end()) {
			for (auto i = parents[it->first]->begin(); i !=  parents[it->first]->end(); ++i) {
				neighbor.insert(*i);
			}
		}

		set<int>::iterator sit;
		for (sit = neighbor.begin(); sit != neighbor.end(); ++sit) {
			ofs << *sit << " ";
		}
		ofs << endl;
	}

	//Close the file
	ofs.close();
}


/**
 * Calculate the diameter of the graph
 * Please only use it for small graph
 */
int Graph::diameter() {
	int totalVertices = primaryAttribute.size();
	int **matrix = new int*[totalVertices];
	for (int i = 0; i < totalVertices; i++)
		matrix[i] = new int[totalVertices];

	for (int i = 0; i < totalVertices; i++) {
		for (int j = 0; j < totalVertices; j++) {
			if (i != j) {
				matrix[i][j] = 10000;
			}
			else
				matrix[i][j] = 0;
		}
	}

	for (auto i = primaryAttribute.begin(); i != primaryAttribute.end(); ++i) {
		if (children.find(i->first) != children.end()) {
			for (auto j = children[i->first]->begin(); j != children[i->first]->end(); ++j) {
				matrix[i->first][*j] = 1;
				matrix[*j][i->first] = 1;
			}
		}
	}

	for (int k = 0; k < totalVertices; k++) {
		for (int i = 0; i < totalVertices; i++) {
			for (int j = 0; j < totalVertices; j++) {
				if (matrix[i][j] > matrix[i][k] + matrix[k][j])
					matrix[i][j] = matrix[i][k] + matrix[k][j];
			}
		}
	}

	int max = 0;
	for (int i = 0; i < totalVertices; i++) {
		for (int j = 0; j < totalVertices; j++) {
			//cout << matrix[i][j] << " ";
			if (matrix[i][j] != 10000 && matrix[i][j] > max) {
				max = matrix[i][j];
			}
		}
		//cout << endl;
	}

	//Delete the resource
	for (int i = 0; i < totalVertices; i++)
		delete [] matrix[i];
	delete [] matrix;

	return max;
}

void Graph::divideGraph(int n, string &partfilename, string &output) {
	const int totalVertices = primaryAttribute.size();
	int* parts = new int[totalVertices];

	ifstream ifs(partfilename);

	if (!ifs) {
		cout << "The file doesn't exist!" << endl;
		return;
	}

	//Read in the partition file
	int division;
	int i = 0;
	while (ifs >> division) {
		parts[i++] = division;
	}

	ifs.close();

	vector<shared_ptr<Partition> > partitions(n);

	for (auto it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {

		int D = parts[it->first];

		if (partitions[D] == NULL) {
			partitions[D] = make_shared<Partition>();
		}

		//If a node doesn't have any children, even though it's assigned to this partition
		//we can skip inserting it into the primaryAttribute map here
		//if it's the children of any vertex in its partition, it will be eventually inserted
		//by the addEdge method
		if (children.find(it->first) != children.end()) {

			partitions[D]->primaryAttribute.insert({it->first, it->second});

			//Update the core part
			partitions[D]->corePart.insert(it->first);

			//Assign all the out-going edges of one vertex to its partition
			for (auto i = children[it->first]->begin(); i != children[it->first]->end(); ++i) {
				partitions[D]->addEdge(it->first, *it->second, *i, *primaryAttribute[*i]);
//				cout << it->first << " " << *it->second << " " << *i << " " << *primaryAttribute[*i] << endl;
//				partitions[D]->print();
			}
		}

		//Add the parents if any
		if (parents.find(it->first) != parents.end()) {
			if (partitions[D]->corePart.find(it->first) == partitions[D]->corePart.end()) {

				partitions[D]->primaryAttribute.insert({it->first, it->second});

				//Update the core part
				partitions[D]->corePart.insert(it->first);

			}
			for (auto i = parents[it->first]->begin(); i != parents[it->first]->end(); ++i) {
				partitions[D]->addEdge(*i, *primaryAttribute[*i], it->first, * it->second);
			}
		}
	}


	//Directly output these partitions into file
	int count = 0;
	for (auto i = partitions.begin(); i != partitions.end(); ++i) {

		//so awkward to convert int to string in C++;
		//the to_string doesn't work for cygwin, don't know why
		ostringstream convert;
		convert << count++;
		string outfile = output + convert.str();

		//Output the partition graph
		(*i)->outputGraph(outfile);
	}

	delete[] parts;
}
/**
 * All the resource is managed by the smart pointers
 */
Graph::~Graph() {

}
