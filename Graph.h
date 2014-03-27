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
using namespace std;
class Graph {
	public:
		////We require that each vertex only has one label in this work
		map<int, string> primaryAttribute;
		
		//The adjacency list of each vertex	    
	    map<int, vector<int> > children;
	    map<int, vector<int> > parents;
	    
		//The value will be set after loading the graph
		int totalEdges;	    
	    
	    bool graphLoaded;
	    
	    map<int, int> indegree;
	    map<int, int> outdegree;

	    //Constructor
	    Graph();
	    Graph(Graph &another);
	    
	    void loadGraphFromFile(string filename);
	    void addEdge(int s, string aS, int t, string aT);
	    void calculateDegree();
	    void Combine(Graph another);

};
#endif /* GRAPH_H_ */
