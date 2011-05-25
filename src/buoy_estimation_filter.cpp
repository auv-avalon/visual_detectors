#include "buoy_estimation_filter.h"

namespace avalon {

BuoyEstimationFilter::BuoyEstimationFilter() : feature_buffer_size(2), last_location(cvPoint(-1,-1))
{
}


BuoyEstimationFilter::~BuoyEstimationFilter()
{
}


// --------------------------------------------------------------------------------------

bool BuoyEstimationFilter::hasBuoyFound() const 
{
    std::list<BuoyFeatureVector>::const_iterator it;

    if( features.size() == feature_buffer_size ) {
        double radius = getAverageRadius();

        for(it = features.begin(); it != features.end(); it++) {
            const feature::Buoy buoy = it->front();
            CvPoint location = cvPoint(buoy.image_x, buoy.image_y);

            if( !checkLocation(location, radius) )
                return false;
        }

        return true;
    }

    return false;
}



feature::Buoy BuoyEstimationFilter::getBestFeature() const 
{
    feature::Buoy last_buoy = features.back().front();

    double radius = getAverageRadius();

    last_buoy.image_radius = radius;

    return last_buoy;
}



void BuoyEstimationFilter::feed(const BuoyFeatureVector& vector) 
{
    if(features.size() > feature_buffer_size) 
        features.pop_front();

    last_location = cvPoint(vector.front().image_x, vector.front().image_y);

    features.push_back(vector);
}


// --------------------------------------------------------------------------------------


bool BuoyEstimationFilter::checkLocation(CvPoint location, int radius) const 
{
     if(last_location.x != -1){
        int difX = abs(last_location.x - location.x);
        int difY = abs(last_location.y - location.y);

        return difX <= radius / 1.25 && difY <= radius / 1.25;
    }
    
    return false;
}


double BuoyEstimationFilter::getAverageRadius() const 
{
    int new_radius = 0;

    if(features.size() < feature_buffer_size)
        return -1;

    std::list<BuoyFeatureVector>::const_iterator it;
    
    for(it = features.begin(); it != features.end(); it++) {
        new_radius += it->front().image_radius;
    }

    return new_radius / static_cast<double>(feature_buffer_size);
}


} // namespace avalon
