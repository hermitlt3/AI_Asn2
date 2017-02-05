#pragma once

#include <list>
#include "SingletonTemplate.h"
#include "Node.h"

#define GRID_ROWS	10
#define GRID_COLS   4
#define GRID_SIZE	10

using std::list;


static float heuristic(Vector3 a, Vector3 b)
{
	return abs(a.x - b.x) + abs(a.y - b.y);
}

struct Node;
class NodeManager : public Singleton<NodeManager>
{
	friend Singleton<NodeManager>;

public:
	void Init(const int width, const int height, Node* node) {
		theNode[width][height] = node;
	}

	list<Node*> returnNeighbours(Node* node) {
		int widthIndex = returnWidthIndex(node);
		int heightIndex = returnHeightIndex(node);

		list<Node*> results;

		// LEFT NEIGHBOUR
		if (widthIndex > 0) {
			if (theNode[widthIndex - 1][heightIndex]->grid->type != Grid::WALL)
				results.push_back(theNode[widthIndex - 1][heightIndex]);
		}
		if (widthIndex < GRID_COLS - 1) {
			if (theNode[widthIndex + 1][heightIndex]->grid->type != Grid::WALL)
				results.push_back(theNode[widthIndex + 1][heightIndex]);
		}
		if (heightIndex > 0) {
			if (theNode[widthIndex ][heightIndex - 1]->grid->type != Grid::WALL)
				results.push_back(theNode[widthIndex][heightIndex - 1]);
		}
		if (heightIndex < GRID_ROWS - 1) {
			if (theNode[widthIndex][heightIndex + 1]->grid->type != Grid::WALL)
				results.push_back(theNode[widthIndex][heightIndex + 1]);
		}

		return results;
	};

	int returnWidthIndex(Node* node) {
		return (node->grid->pos.x - (GRID_SIZE >> 1)) / GRID_SIZE;
	}

	int returnHeightIndex(Node* node) {
		return (node->grid->pos.y - (GRID_SIZE >> 1)) / GRID_SIZE;
	}

	int returnGcost(Node* start, Node* curr) {
		int startWIndex = returnWidthIndex(start);
		int startHIndex = returnHeightIndex(start);
		int currWIndex = returnWidthIndex(curr);
		int currHIndex = returnHeightIndex(curr);

		int a = abs(currWIndex - startWIndex) + abs(currHIndex - startHIndex);
		return a;
	}

	int returnHcost(Node* curr, Node* end) {
		return (int)heuristic(end->grid->pos, curr->grid->pos);
	}

	void CalculateFGHCost(Node* start, Node* goal) {
		for (size_t i = 0; i < GRID_ROWS; ++i) {
			for (size_t j = 0; j < GRID_COLS; ++j) {
				if (theNode[j][i]->grid->type != Grid::START || theNode[j][i]->grid->type != Grid::END)
				{
					theNode[j][i]->G = returnGcost(start, theNode[j][i]);
					theNode[j][i]->H = returnHcost(theNode[j][i], goal);
					theNode[j][i]->F = theNode[j][i]->G + theNode[j][i]->H;
				}
			}
		}
	}
//private:
	Node* theNode[GRID_COLS][GRID_ROWS];
};


