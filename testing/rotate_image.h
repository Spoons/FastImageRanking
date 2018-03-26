#pragma once

inline Vector2 imageSizeAfterRotation(int x, int y, double theta);

inline Vector2 imageSizeAfterRotation(Image * img, double theta);

void rotateImage(Patch * src, Patch &dest, int32_t theta);

void rotationByBilinearInterpolation(Patch * src, Patch * dest, int32_t angle);
