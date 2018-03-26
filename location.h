#include "common.h"


#ifdef BOOST
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/split_member.hpp>
#include <fstream>
#endif 

struct Location {
	int32_t x, y, theta;
	Patch * image;

    //these two are related
    int8_t * foreground_frame;
    int64_t * foreground_score;

#ifdef BOOST
    friend class boost::serialization::access;

    private:

    template <typename Archive>
        void load(Archive &ar, const unsigned int version) {
            ar & x;
            ar & y;
            ar & theta;
            ar & image;
        }

    template <typename Archive>
        void save(Archive &ar, const unsigned int version) const {
            ar & x;
            ar & y;
            ar & theta;
            ar & image;
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif


};
