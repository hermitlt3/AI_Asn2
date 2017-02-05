#pragma once
#include "Vector3.h"
#include "Node.h"
#include "NodeManager.h"
#include <list>
#include <queue>
#include <map>


using namespace std;

struct LessFcost {
	bool operator()(Node* lhs, Node* rhs)
	{
		return lhs->F > rhs->F;
	}
};


static void AStarAlgorithm(Node* start, Node* goal) 
{
	// The set of currently discovered nodes that are already evaluated
	priority_queue<Node*, std::vector<Node*>, LessFcost>openList;
	// Only start node is known
	openList.push(start);

	// The set of nodes already checked
	map<Node*, int> cost_so_far;
	map<Node*, Node*> came_from;
	cost_so_far[start] = 0;
	came_from[start] = nullptr;
	
	Node* current;

	while (!openList.empty())
	{
		current = openList.top();

		if (current == goal){
			break;
		}
		
		list<Node*>tempList = NodeManager::GetInstance()->returnNeighbours(current);

		for (std::list<Node*>::iterator it = tempList.begin(); it != tempList.end(); ++it) {
			Node* next = (*it);
			
			int new_cost = cost_so_far[current] + NodeManager::GetInstance()->returnGcost(current, next);
			// If node is an obstacle
			if (next->grid->type != Grid::EMPTY) {
				//continue;
			}
			// Ignore neighbour

			// If node is calculated
			if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;

				//next->F = new_cost + heuristic(goal->grid->pos, next->grid->pos);
				std::make_heap(const_cast<Node**>(&openList.top()), const_cast<Node**>(&openList.top()) + openList.size(), LessFcost());

				openList.push(next);
				came_from[next] = current;
			}
		}
	}
	/*while (current != start) {

	}*/
}
