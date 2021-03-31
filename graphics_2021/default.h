#pragma once

enum Direction { UP, DOWN, LEFT, RIGHT };
enum Team { PLAYER, ENEMY };
enum Type { BULLET, ITEM};

struct Pos
{
	float x;
	float y;
};

struct Color
{
	float r;
	float g;
	float b;
};

struct Drawable
{
	Pos pos;
	Color color;
};