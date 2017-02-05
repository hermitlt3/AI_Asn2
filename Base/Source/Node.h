#ifndef NODE_H_
#define NODE_H_

#include "Vector3.h"
#include "Grid.h"

static int nodeID = 0;

struct Grid;
// Node class for A* pathfinding
struct Node
{
	// Constructor
	Node() {
		parent = nullptr;
		visited = false;
		ID = nodeID ++ ;
		G = 0;
		F = 0;
		H = 0;
	};
	// Destructor
	~Node() {};

	// Parent node to trace back 
	Node *parent;	

	// The movement cost to move from the starting point A
	// to a given node on the grid, following the path generated
	// to get there
	unsigned int G;

	// The estimated movement cost to move from that given node
	// to the final destination, point B. a.k.a the Heuristic
	unsigned int H;
	
	// F = G + H
	unsigned int F;

	// The grid it is at
	Grid* grid;

	// Whether it is visited
	bool visited;

	friend bool operator==(const Node& lhs, const Node& rhs);
	
	int ID; // juz a lil smth for us to use operator ==
};

bool operator==(const Node& lhs, const Node& rhs)
{
	return lhs.ID == rhs.ID;
}

#endif