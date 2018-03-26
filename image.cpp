#include "image.h"
#include "stb_image.h"
#include "stb_image_write.h"


void Image::write_to_file(uint8_t *rgb, char * filename, int x, int y, int n) {
    stbi_write_png((char const *)filename, x, y, 3, rgb, 0);
}


Image::Image(int32_t x, int32_t y, uint8_t *rgb) {
    x_ = x;
    y_ = y;
    n_ = 3;
    rgb_ = rgb;

}
void Image::interlaced(uint8_t *rgb) {
    for (int i = 0; i < x_; i++) {
        for (int j = 0; j < y_; j++) {
            PixelP pix;
            getPixel(&i, &j, &pix);
            int pixel_location = (x_ * j + i) * 3;
            rgb[pixel_location] = *pix.r;
            rgb[pixel_location+1] = *pix.g;
            rgb[pixel_location+2] = *pix.b;
        }
    }
}
