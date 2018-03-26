// score.cpp - contains methods related to scoring templates

#define _CRT_SECURE_NO_WARNINGS
#define __STDC_FORMAT_MACROS


#include "common.h"
#include "patch.h"
#include "rotate.h"
#include "location.h"

#include "score.h"


using spp::sparse_hash_map;

#ifdef BOOST
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#endif


///@param n Number of location entries in loc
///@param loc array of patches and location to search for
///@param score array of n entries to output score of each patch,loc pair

#ifdef DUMP_MEM
long int frame_count = 0;
#endif

void templateScoring(Image * img, Location * loc, int32_t n, int64_t * score) {
    templateScoring(img, loc, n, score, NULL);
}

void templateScoring(Image * img, Location * loc, int32_t n, int64_t * score, int8_t * foreground_input) {
    bool foreground_enabled = false;
    if (foreground_input != NULL)
        foreground_enabled = true;


#ifdef DUMP_MEM
    std::string fp = "dump/frame" + std::to_string(frame_count) + ".data";
    std::ofstream ofs(fp);
    {
        boost::archive::text_oarchive oa{ofs};
            oa << *img;
            oa << *loc;
    }
    frame_count += 1;
#endif



    spp::sparse_hash_map <std::size_t, Patch *> rotated_patches;
    for (int i = 0; i < (int)n; i++) {

        loc[i].image->theta_ = loc[i].theta;
        size_t hash = Patch::Hash(loc[i].theta, loc[i].image->id_);
        auto it = rotated_patches.find(hash);

        if (it == rotated_patches.end()) {
            Patch *patch_w_theta = new Patch(*loc[i].image);
            std::pair<size_t,Patch*> element (hash, patch_w_theta);

            rotated_patches.insert(element);
            loc[i].image = patch_w_theta;

        } else {
            loc[i].image = it->second;
        }
    }


    #pragma omp parallel for shared(rotated_patches)
    for (int i = 0; i < (int)rotated_patches.size(); i++) {
        auto it = rotated_patches.begin();
        advance(it, i);
        Patch *image_ptr = (it->second);
        int32_t theta = image_ptr->theta_;


        if (theta != 0) {

            Patch temp;

            Vector2 new_patch_size = imageSizeAfterRotation((Image*)image_ptr, theta);
            temp = *image_ptr;
            temp.x_ = new_patch_size.x;
            temp.y_ = new_patch_size.y;

            uint8_t *rgb = (uint8_t*)malloc(temp.imageSize());
            memset(rgb, 0, temp.imageSize());
            double * heatmap = (double*)malloc(temp.heatMapSize());
            memset(heatmap, 0, temp.heatMapSize());

            temp.rgb_ = rgb;
            temp.heat_map_ = heatmap;
            rotationByBilinearInterpolation(image_ptr, &temp, theta);

            *image_ptr = temp;

        }

    }

    #pragma omp parallel for 
    for (int i = 0; i < (int)n; i++) {

        //finders the center of each template
        int32_t tempCenterX = (loc+i)->image->x_ / 2;
        int32_t tempCenterY = (loc+i)->image->y_ / 2;

        //the score array represents the total score for each patch
        *(score+i) = 0;


        //looping through each pixel in the template
        for (int32_t x = 0; x < (loc+i)->image->x_; x++) {
            for (int32_t y = 0; y < (loc+i)->image->y_; y++) {


                //lpoint is the pointer to the location where the patch will be scored
                Location * lpoint = loc + i;


                //here we grab the template pixel and heatmap 'pixel'
                PixelP template_pixel;
                lpoint->image->getPixel((int32_t *)&x, (int32_t *)&y, &template_pixel);

                double *distWeight = lpoint->image->getHeatMapRef(&x,&y);

                //calculate the frame pixel to compare to
                //subtract tempCenter to find to base from top
                //left hand side
                int32_t frame_pixel_x = lpoint->x + x - tempCenterX;
                int32_t frame_pixel_y = lpoint->y + y - tempCenterY;


                //this is the local counter for the score
                int64_t scoreLocal = 0;

               //any 0's represent alpha mask
                if (*distWeight == 0.0) {
                    continue;
                }

                //skip if frame pixel is out of bounds and not masked off
                if (frame_pixel_x >= img->x_ || frame_pixel_y >= img->y_ ||
					frame_pixel_x < 0 || frame_pixel_y < 0) {

                    scoreLocal += 255;

                } else {

                    //here we get frame pixel

                    PixelP frame_pixel;
                    img->getPixel(&frame_pixel_x, &frame_pixel_y, &frame_pixel);

                    //sum the difference of the pixel
                    scoreLocal += abs(*frame_pixel.r - *template_pixel.r);
                    scoreLocal += abs(*frame_pixel.g - *template_pixel.g);
                    scoreLocal += abs(*frame_pixel.b - *template_pixel.b);


                    //here we create the the foreground map
                    if(foreground_enabled &&
                            lpoint->foreground_frame!=NULL &&
                            *distWeight > 0.2) {

                        int8_t *foreground_pixel = (lpoint->foreground_frame)+
                            (frame_pixel_x*(img->y_) + frame_pixel_y);

                       // *foreground_pixel = 1;
                       // if we are at this point, we can assume that
                       // foreground pixel will be equal to 1 so lets just
                       // look at the the value in foreground input
                       int8_t *foreground_input_pixel = foreground_input +
                            (frame_pixel_x*(img->y_) + frame_pixel_y);

                       if (*foreground_input_pixel == 1 && *foreground_pixel != 1) {
                           *lpoint->foreground_score += 1;

                           *foreground_pixel = 1;
                       }
                    }

                }
                //multiply score by heat value nad add to global score
                scoreLocal *= *distWeight;
                *(score + i) += scoreLocal;

            }
        }
    }

    //dump all patches and associated memory
    for (auto it : rotated_patches) {
        if (it.second->theta_ != 0) {
            free(it.second->rgb_);
            free(it.second->heat_map_);
        }
        delete it.second;
    }


}
