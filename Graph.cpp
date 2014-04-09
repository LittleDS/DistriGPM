/*
 * Graph.cpp
 *
 *  Created on: Mar 23, 2014
 *      Author: Lei Yang
 */
#include <Graph.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>

using namespace std;
//Default constructor
Graph::Graph():totalEdges(0), graphLoaded(false) { }

Graph::Graph(const Graph &another):
	totalEdges(another.totalEdges),
	graphLoaded(true),
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

		graphLoaded = true;
	}

	ifs.close();
}

/**
 * There is currently some potential errors with this function
 * Since the labels of the vertices in the graph is already there
 * When we try to insert a new edge into the graph, the parameter aS and aT must be consistent with
 * the labels already in the graph, this function doesn't check the consistency
 */
void Graph::addEdge(int s, string &aS, int t, string &aT) {
	if (primaryAttribute.find(s) == primaryAttribute.end()) {
		shared_ptr<string> ts = make_shared<string>(aS);
		primaryAttribute.insert({s, ts});
	}

	if (primaryAttribute.find(t) == primaryAttribute.end()) {
		shared_ptr<string> ts = make_shared<string>(aT);
		primaryAttribute.insert({t, ts});
	}

	if (children.find(s) != children.end()) {
		children[s]->push_back(t);
	}
	else {
		shared_ptr<vector<int> > childVector = make_shared<vector<int> >();
		childVector->push_back(t);
		children.insert({s, childVector});
	}

	if (parents.find(t) != parents.end()) {
		parents[t]->push_back(s);
	}
	else {
		shared_ptr<vector<int> > parentVector = make_shared<vector<int> >();
		parentVector->push_back(s);
		parents.insert({t, parentVector});
	}

	//Update the in and out degree
	if (outdegree.find(s) != outdegree.end()) {
		outdegree[s]++;
	}
	else {
		outdegree.insert({s, 1});
	}

	if (indegree.find(t) != indegree.end()) {
		indegree[t]++;
	}
	else {
		indegree.insert({t, 1});
	}
}

/**
 * Calculate the indegree and outdegree according to the children and parents table
 */
void Graph::calculateDegree() {
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

void Graph::divideGraph(int n, string &partfilename) {
	const int totalVertices = primaryAttribute.size();
	int* parts = new int[totalVertices];

	ifstream ifs(partfilename);

	if (!ifs) {
		cout << "The file doesn't exist!" << endl;
		return;
	}

	int division;
	int i = 0;
	while (ifs >> division) {
		parts[i++] = division;
	}

	vector<shared_ptr<Graph> > graphs(n);

	map<int, shared_ptr<string> >::iterator it;

	for (it = primaryAttribute.begin(); it != primaryAttribute.end(); ++it) {

		//Since the graph vertex ID starts from 1, we need to minus 1 here
		int D = parts[it->first - 1];

		if (graphs[D] == NULL) {
			graphs[D] = make_shared<Graph>();
		}


		//If a node doesn't have any children, even though it's assigned to this partition
		//we can skip inserting it into the primaryAttribute map here
		//if it's the children of any vertex in its partition, it will be eventually inserted
		//by the addEdge method
		if (children.find(it->first) != children.end()) {

			graphs[D]->primaryAttribute.insert({it->first, it->second});

			//Assign all the out-going edges of one vertex to its partition
			for (auto i = children[it->first]->begin(); i != children[it->first]->end(); ++i) {
				graphs[D]->addEdge(it->first, *it->second, *i, *primaryAttribute[*i]);
			}
		}
	}

	int count = 0;
	for (auto i = graphs.begin(); i != graphs.end(); ++i) {

		//so awkward to convert int to string in C++;
		//the to_string doesn't work for cygwin, don't know why
		ostringstream convert;
		convert << count++;
		string outfile = partfilename + convert.str();

		//Output the partition graph
		(*i)->outputGraph(outfile);
	}

	ifs.close();
	delete[] parts;
}

/**
 * All the resource is managed by the smart pointers
 */
Graph::~Graph() {

}
