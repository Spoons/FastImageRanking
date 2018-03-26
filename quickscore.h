#pragma once
class Location;

void mStructToImage(const mxArray *p, Patch ** img, int32_t *arrayLength);
void mArrayToLocs(const mxArray *p, Location ** loc, int32_t *length);
void printResults (int64_t * results, int32_t n);
