#pragma once
#include "Vector3.h"
#include "Node.h"
#include "NodeManager.h"
#include <map>
#include <set>
#include <vector>

using namespace std;

bool leftFltrightF(Node* lhs, Node* rhs)
{
	return lhs->F < rhs->F;
}


static vector<Vector3> AStarAlgorithm(Node* start, Node* goal) 
{
	/*
	// The set of currently discovered nodes that are already evaluated
	priority_queue<Node*, std::vector<Node*>, LessFcost>openList;
	// Only start node is known
	openList.push(start);

	// The set of nodes already checked
	map<Node*, int> cost_so_far;
	map<Node*, Node*> came_from;
	cost_so_far[start] = 0;
	came_from[start] = nullptr;
	int i = 0;
	Node* current;

	while (!openList.empty())
	{
		current = openList.top();

		if (current == goal || i > 20){
			break;
		}
		i++;
		list<Node*>tempList = NodeManager::GetInstance()->returnNeighbours(current);

		for (std::list<Node*>::iterator it = tempList.begin(); it != tempList.end(); ++it) {
			Node* next = (*it);
			next->visited = true;
			int new_cost = cost_so_far[current] + NodeManager::GetInstance()->returnGcost(current, next);
			// If node is an obstacle
			if (next->grid->type == Grid::WALL) {
				continue;
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
	while (current != start) {

	}*///
	bool(*compare)(Node*, Node*) = leftFltrightF;

	set<Node*, bool(*)(Node*, Node*)>openList(compare);
	set<Node*>closeList;
	openList.insert(start);

	// The set of nodes already checked
	map<Node*, int> gScore;
	map<Node*, int> fScore;
	map<Node*, Node*> came_from;
	gScore[start] = 0;
	fScore[start] = heuristic(start->grid->pos, goal->grid->pos);

	Node* current;
	vector<Vector3> results;
	results.push_back(goal->grid->pos);
	while (!openList.empty())
	{
		current = *(openList.begin());

		if (current == goal)
			break;

		openList.erase(current);
		closeList.insert(current);

		list<Node*>tempList = NodeManager::GetInstance()->returnNeighbours(current);
		for (std::list<Node*>::iterator it = tempList.begin(); it != tempList.end(); ++it) {
			Node* next = (*it);
		
			if (closeList.find(next) != closeList.end()) {
				continue;
			}
			next->visited = true;

			int tentative_gScore = gScore[current] + heuristic(current->grid->pos, next->grid->pos);
			if (openList.find(next) == openList.end()) {
				openList.insert(next);
			}
			else if (tentative_gScore >= gScore[next])
				continue;

			came_from[next] = current;
			gScore[next] = tentative_gScore;
			fScore[next] = gScore[next] + heuristic(next->grid->pos, goal->grid->pos);
		}
	}

	if (current != goal)
		return results;

	while (current != start)
	{
		current->parent = came_from[current];
		if (results.back().x != came_from[current]->grid->pos.x && results.front().y != came_from[current]->grid->pos.y) {
			results.push_back(current->grid->pos);
		}
		current = came_from[current];
	}
	results.push_back(start->grid->pos);

	for (int i = 0; i < results.size(); ++i) {
		cout << results[i] << endl;
	}
	return results;
}
