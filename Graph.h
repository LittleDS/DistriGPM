/*
 * Joint.h
 *
 *  Created on: 2014-3-26
 *      Author: Lei Yang
 */
#ifndef GRAPH_H_
#define GRAPH_H_
#include <string>
#include <map>
#include <vector>
#include <memory>

using namespace std;

typedef vector<int> int_vector;
typedef int_vector::iterator int_vector_it;

class Graph {
	public:
		//The value will be set after loading the graph
		int totalEdges;
		
		bool graphLoaded;

	    //We require that each vertex only has one label in this work
		map<int, shared_ptr<string> > primaryAttribute;

		//The adjacency list of each vertex	    
	    map<int, shared_ptr<vector<int> > > children;
	    map<int, shared_ptr<vector<int> > > parents;

	    map<int, int> indegree;
	    map<int, int> outdegree;

	    //Constructor
	    Graph();
	    Graph(const Graph &another);
	    
	    void loadGraphFromFile(string &filename);
	    void addEdge(int s, string &aS, int t, string &aT);
	    void calculateDegree();
	    void Combine(Graph &another);
	    void print();
	    void outputGraph(string &outname);
	    void outputParMETIS(string &filename);
	    void divideGraph(int n, string &partfilename);
	    ~Graph();

};
#endif /* GRAPH_H_ */
