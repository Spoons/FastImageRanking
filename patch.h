#pragma once

#include "image.h"


#ifdef BOOST
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/split_member.hpp>
#include <fstream>
#endif 

class Patch : public Image {	
    public:
        int32_t theta_;
        double * heat_map_;
        bool * foreground;

		Patch(int32_t x, int32_t y, uint8_t *rgb, double * heatmap);


        Patch() = default;

        bool operator==(const Patch &o) const;
        size_t Hash(void);
        static size_t Hash(int32_t theta, int32_t id);

        inline double * getHeatMapRef(int32_t *x, int32_t *y) {
            return &heat_map_[*x * (y_) + *y];
        }

        inline size_t heatMapSize(void) const {
            #ifdef DEBUG
            if (x_ * y_ == 0)
                printf("ERROR NO HEATMAP MEMORY\n");
            #endif
            return (x_ * y_ * sizeof(double));
        }

#ifdef BOOST
        friend class boost::serialization::access;

    private:

        //need to add the foreground dumping/loading
        template <typename Archive>
            void load(Archive &ar, const unsigned int version) {
                ar & id_;
                ar & x_;
                ar & y_;
                ar & n_;
                rgb_ =(unsigned char *) malloc(imageSize());
                ar & boost::serialization::make_array(rgb_, (size_t)imageSize());
                heat_map_ = (double *)malloc(heatMapSize());
                ar & boost::serialization::make_array(heat_map_, (size_t)heatMapSize());
                ar & theta_;

            }
        template <typename Archive>
            void save (Archive &ar,  const unsigned int version) const {
                ar & id_;
                ar & x_;
                ar & y_;
                ar & n_;
                ar & boost::serialization::make_array(rgb_, imageSize());
                ar & boost::serialization::make_array(heat_map_, heatMapSize());
                ar & theta_;
                
            }

        BOOST_SERIALIZATION_SPLIT_MEMBER()

#endif

 };
        
