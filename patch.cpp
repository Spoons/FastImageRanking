#include <sparsepp/spp.h>
#include "patch.h"

Patch::Patch(int32_t x, int32_t y, uint8_t *rgb, double *heatmap) {
    x_ = x;
    y_ = y;
    n_ = 3;
    rgb_ = rgb;
    heat_map_ = heatmap;
}

bool Patch::operator==(const Patch &o) const {
    return id_ == o.id_ && theta_ == o.theta_; 
}

size_t Patch::Hash() {
    std::size_t seed = 0;
    spp::hash_combine(seed, theta_);
    spp::hash_combine(seed, id_);
    return seed;
}

size_t Patch::Hash(int32_t theta, int32_t id) {
    std::size_t seed = 0;
    spp::hash_combine(seed, theta);
    spp::hash_combine(seed, id);
    return seed;
}
