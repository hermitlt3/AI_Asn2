#pragma once

#include "Vector3.h"
#include "list"

using std::list;

struct Grid {
	enum GRID_TYPE
	{
		EMPTY = 0,
		WALL,
		END,
		START,

		TOTAL_GRID_TYPES
	};
	
	Grid() {
		pos = Vector3(0, 0, 0);
		scale = Vector3(1, 1, 1);
		type = EMPTY;
	}
	Grid(Vector3 pos, Vector3 scale, GRID_TYPE type) {
		this->pos = pos;
		this->scale = scale;
		this->type = type;
	}
	~Grid() {

	}

	Vector3 pos;
	Vector3 scale;
	GRID_TYPE type;
};



