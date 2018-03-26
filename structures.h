#pragma once
class Patch;

class PixelP{
    public:
	uint8_t *r, *g, *b;
};

class Pixel{
    public:
	uint8_t r, g, b;
};
class Vector2{
    public:
	int_fast32_t x, y;
};


struct distMatrix{
	int32_t x, y;
	double * mat;
};
