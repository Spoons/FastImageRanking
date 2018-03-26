#include "common.h"
#include "rotate.h"
#include "image.h"
#include "patch.h"

Vector2 imageSizeAfterRotation(int x, int y, double theta) {
    Vector2 size;
    //	theta *= M_PI / 180;

    int32_t w = x;
    int32_t h = y;

    double s = sin(theta);
    double c = cos(theta);

    if (s < 0) s = -s;
    if (c < 0) c = -c;

    size.x = (int_fast32_t)(h*s + w*c);
    size.y = (int_fast32_t)(h*c + w*s);
    return size;
}

Vector2 imageSizeAfterRotation(Image * img, double theta) {
    Vector2 size;/*
                    theta *= M_PI / 180;

                    int32_t w = img->x_;
                    int32_t h = img->y_;

                    double s = sin(theta);
                    double c = cos(theta);

                    if (s < 0) s = -s;
                    if (c < 0) c = -c;

                    size.x = (int_fast32_t)(h*s + w*c);
                    size.y = (int_fast32_t)(h*c + w*s);*/
    int32_t dim = FastMax(img->x_, img->y_);
    size.x = dim;
    size.y = dim;
    return size;
}
void rotateImage(Patch * src, Patch &dest, int32_t theta) {
	Vector2 newsize = imageSizeAfterRotation((Image*)src, theta);
	dest = *src;
	dest.x_ = newsize.x;//newsize.x;
	dest.y_ = newsize.y;//newsize.y;

	uint8_t *rgb = (uint8_t*)malloc(dest.imageSize());
	memset(rgb, 255, dest.imageSize());
	double * heatmap = (double*)malloc(dest.heatMapSize());
	memset(heatmap, 0, dest.heatMapSize());

	dest.rgb_ = rgb;
	dest.heat_map_ = heatmap;
    #ifdef MEX
	mexPrintf("src->x: %i\nsrc->y: %i\ndest->x: %i\nsrc->y: %i\n", src->x_, src->y_, dest.x_, dest.y_);
    #endif
	rotationByBilinearInterpolation(src, &dest, theta);

    uint8_t *rgb_i = (uint8_t*)malloc(dest.imageSize());
    memset(rgb_i, 0, dest.imageSize());

    dest.interlaced(rgb_i);
    Image::write_to_file(rgb_i, "out.png", dest.x_, dest.y_, 3);


}
//scores* scoreTemplates(image *img, Loc * locs);
///@param angle in degree
void rotationByBilinearInterpolation(Patch * src, Patch * dest, int32_t angle) {
    int32_t cx = src->x_/2;
    int32_t cy = src->y_/2;
    int32_t dest_center = dest->x_ / 2;

    int32_t y0;
    for (y0 = 0; y0 < dest->y_; y0++) {
        //y0t and x0t are translated x0 and y0 points with cx,cy as the origin

        for (int32_t x0 = 0; x0 < dest->x_; x0++) {

            //to begin we are going to convert raster to cartesian
            int32_t x = x0 - dest_center;
            int32_t y = dest_center - y0;

            double fDistance = std::sqrt(x*x + y*y);
            double fPolarAngle = 0.0;

            if (x==0) {
                if (y==0) {
                    //center of image no rotation needed

                    PixelP dest_center_pixel, src_center_pixel;

                    dest->getPixel(&x0, &y0, &dest_center_pixel);
                    src->getPixel(&cx, &cy, &src_center_pixel);

                    *dest_center_pixel.r = *src_center_pixel.r;
                    *dest_center_pixel.g = *src_center_pixel.g;
                    *dest_center_pixel.b = *src_center_pixel.b;

                    *dest->getHeatMapRef(&x0, &y0) = 
                        *src->getHeatMapRef(&cx, &cy);

                }
                else if ( y < 0) {
                    fPolarAngle = 1.5 * M_PI;
                } else {
                    fPolarAngle = 0.5 * M_PI;
                }
            } else {
                fPolarAngle = std::atan2((double)y,(double)x);
            }

            //now we perform the rotation

            fPolarAngle += ( angle * M_PI)/180;

            //covert polar back to cartesian
            double x0t = fDistance * std::cos(fPolarAngle);
            double y0t = fDistance * std::sin(fPolarAngle);

            //convert cartesian to raster
            x0t = x0t + (double)cx;
            y0t = (double)cy - y0t;

            //These are vectors for each corner Pixel
            //p12---------p22
            // |		   |
            //p11---------p21
            Vector2 p11v, p12v, p21v, p22v;
            //these are the four nearest Pixels to the original pixel location
            //we will be sampling each corner Pixel around x1 and y1 and finding
            //the bilinear interpolation of the colors
            //im using min and max to clamp Pixels if they go past the border
            //of the image

            int floor_y0t = floor(y0t);
            int floor_x0t = floor(x0t);
            int ceil_y0t = ceil(y0t);
            int ceil_x0t = ceil(x0t);

            if (floor_x0t < 0 || ceil_x0t < 0 || floor_x0t >= src->x_ || ceil_x0t >= src->x_
                || floor_y0t < 0 || ceil_y0t < 0 || floor_y0t >= src->y_ || ceil_y0t >= src->y_) {
                continue;
            }

            p12v.x = (int_fast32_t)floor_x0t;
            p12v.y = (int_fast32_t)floor_y0t;
            p12v.x = FastMax((int_fast32_t)0, p12v.x);
            p12v.y = FastMax((int_fast32_t)0, p12v.y);
            p12v.x = FastMin((int_fast32_t)src->x_, p12v.x);
            p12v.y = FastMin((int_fast32_t)src->y_, p12v.y);

            p22v.x = (int_fast32_t)ceil_x0t;
            p22v.y = p12v.y;
            p22v.x = FastMin((int_fast32_t)src->x_, p22v.x);
            p22v.x = FastMax((int_fast32_t)0, p22v.x);

            p11v.x = p12v.x;
            p11v.y = (int_fast32_t)ceil_y0t;
            p11v.y = FastMin((int_fast32_t)src->y_, p11v.y);
            p11v.y = FastMax((int_fast32_t)0, p11v.y);
            p21v.x = p22v.x;
            p21v.y = p11v.y;

            //now we copy the values of the rgb data at each vector
            //this could easily be reduced to a single 24bit copy...
            PixelP p11, p12, p21, p22;
            src->getPixelFromVector(&p11v, &p11);
            src->getPixelFromVector(&p12v, &p12);
            src->getPixelFromVector(&p21v, &p21);
            src->getPixelFromVector(&p22v, &p22);

            //now i shall calculate the values for the destination Pixels
            double f1, f2;

            //if two neighbor coords are at the same point,
            //force calculation to one point. This prevents divide by zero.
            if (p21v.x == p11v.x) {
                f1 = 1;
                f2 = 0;
            } else {
                f1 = (((double)p21v.x - (double)x0t) / ((double)p21v.x - (double)p11v.x));
                f2 = (((double)x0t - (double)p11v.x) / ((double)p21v.x - (double)p11v.x));
            }

            //calculate the weight between top and bottom Pixel
            Pixel r1, r2;
            r1.r = (uint8_t)(f1 * (double)*p11.r + f2 * (double)*p21.r);
            r1.g = (uint8_t)(f1 * (double)*p11.g + f2 * (double)*p21.g);
            r1.b = (uint8_t)(f1 * (double)*p11.b + f2 * (double)*p21.b);

            r2.r = (uint8_t)(f1 * (double)*p12.r + f2 * (double)*p22.r);
            r2.g = (uint8_t)(f1 * (double)*p12.g + f2 * (double)*p22.g);
            r2.b = (uint8_t)(f1 * (double)*p12.b + f2 * (double)*p22.b);

            double f3, f4;
            //again points are the same
            //prevents division by zero
            if (p12v.y == p11v.y) {
                f3 = 1;
                f4 = 0;
            }
            else {
                f3 = ((double)p12v.y - y0t) / ((double)p12v.y - (double)p11v.y);
                f4 = (y0t - (double)p11v.y) / ((double)p12v.y - (double)p11v.y);
            }
            //calculate the actual value of the Pixel using the weights
            //calculated before
            Pixel pfinal;
            pfinal.r = (uint8_t)((f3 * r1.r) + (f4 * r2.r));
            pfinal.g = (uint8_t)((f3 * r1.g) + (f4 * r2.g));
            pfinal.b = (uint8_t)((f3 * r1.b) + (f4 * r2.b));

            //clamps ecah value to 8 bits
            pfinal.r = FastMin((uint8_t)255, pfinal.r);
            pfinal.r = FastMax((uint8_t)0, pfinal.r);
            pfinal.g = FastMin((uint8_t)255, pfinal.g);
            pfinal.g = FastMax((uint8_t)0, pfinal.g);
            pfinal.b = FastMin((uint8_t)255, pfinal.b);
            pfinal.b = FastMax((uint8_t)0, pfinal.b);

                PixelP pixel_output_location;
                dest->getPixel(&x0,&y0, &pixel_output_location);

                *pixel_output_location.r = pfinal.r;
                *pixel_output_location.g = pfinal.g;
                *pixel_output_location.b = pfinal.b;

                int32_t y0t_i = floor(y0t);
                int32_t x0t_i = floor(x0t);

                *dest->getHeatMapRef(&x0, &y0) = 
                    *src->getHeatMapRef(&x0t_i, &y0t_i);
           // }

        }
    }
}
