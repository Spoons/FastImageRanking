#pragma once


class Patch;
class Image;

void rotationByBilinearInterpolation(Patch * src, Patch * dest, int32_t angle);

Vector2 imageSizeAfterRotation(int x, int y, double theta);

Vector2 imageSizeAfterRotation(Image * img, double theta);

void rotateImage(Patch * src, Patch &dest, int32_t theta);

