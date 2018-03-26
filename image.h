#pragma once

#include <stdio.h>
#include <inttypes.h>

#include "common.h"

#ifdef BOOST
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/split_member.hpp>
#include <fstream>
using namespace boost::archive;
#endif 

class Image {
    public:

	uint8_t * rgb_;
	int32_t x_, y_, n_;
    int32_t id_;

    Image(int32_t x, int32_t y, uint8_t *rgb);
    Image() = default;

#ifdef BOOST
    friend class boost::serialization::access;
#endif

    void interlaced(uint8_t *rgb );


    static void write_to_file(uint8_t *rgb, char * filename, int x, int y, int n); 
   
        
     inline size_t imageSize(void) const {
        #ifdef DEBUG
        if (x_ * y_ * n_== 0)
            printf("ERROR ALLOCATING NO MEMORY\n");
        #endif
        return (n_*x_*y_ * sizeof(uint8_t));
    }
    inline void getPixel(int32_t *x, int32_t *y, PixelP *r) {
		r->r = &rgb_[(0*(y_*x_) + *x*(y_) + *y)]; 
        r->g = &rgb_[(1*(y_*x_) + *x*(y_) + *y)];
        r->b = &rgb_[(2*(y_*x_) + *x*(y_) + *y)];
		
    }
    inline void getPixelFromVector(Vector2 *v, PixelP *r) {
		r->r = &rgb_[(0*(y_*x_) + v->x*(y_) + v->y)]; r->g = &rgb_[(1*(y_*x_) + v->x*(y_) + v->y)];
        r->b = &rgb_[(2*(y_*x_) + v->x*(y_) + v->y)];
		
    }

    private:

#ifdef BOOST
        template <typename Archive>
            void load(Archive &ar, const unsigned int version) {
                ar & id_;
                ar & x_;
                ar & y_;
                ar & n_;
                rgb_ =(unsigned char *) malloc(imageSize());
                ar & boost::serialization::make_array(rgb_, (size_t)imageSize());

            }
        template <typename Archive>
            void save (Archive &ar,  const unsigned int version) const {
                ar & id_;
                ar & x_;
                ar & y_;
                ar & n_;
                ar & boost::serialization::make_array(rgb_, imageSize());

            }

        BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};
