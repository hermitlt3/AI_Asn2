#ifndef NODE_H_
#define NODE_H_

// Node class for A* pathfinding
struct Node
{
	// Constructor
	Node();
	// Destructor
	~Node();

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
};

#endif