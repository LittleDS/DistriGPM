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

	    //We require that each vertex only has one label in this work
		map<int, int> primaryAttribute;

		//The adjacency list of each vertex	    
	    map<int, shared_ptr<vector<int> > > children;
	    map<int, shared_ptr<vector<int> > > parents;

	    map<int, int> indegree;
	    map<int, int> outdegree;

	    //Constructor
	    Graph();
	    Graph(const Graph &another);
	    
	    //Load the data graph from disk
	    virtual void loadGraphFromFile(string &filename);

	    //Add an edge to the graph, the source and target vertex might be new
	    void addEdge(int s, int aS, int t, int aT);

	    //Remove an existing edge from the graph
	    void removeEdge(int s, int t);

	    //Calculate the in and out degree of the graph
	    void calculateDegree();

	    //Merge another graph into this graph
	    void Combine(Graph &another);

	    //Test-purposed print function
	    virtual void print();

	    //Output the data graph to file
	    virtual void outputGraph(string &outname);

	    //Output to the serial ParMETIS format
	    void outputParMETIS(string &filename);

	    //Calculate the diameter of the graph using Floyd algorithm
	    //Should only be called on small graph
	    int diameter();

	    //According to the part file from ParMETIS, divide the graph into partition
	    void divideGraph(int n, string &partfilename, string &output);

	    virtual ~Graph();

};
#endif /* GRAPH_H_ */
