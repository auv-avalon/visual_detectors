#include "buoy_estimation_filter.h"

namespace avalon {

BuoyEstimationFilter::BuoyEstimationFilter() : feature_buffer_size(2), last_location(cvPoint(-1,-1))
{
}


BuoyEstimationFilter::~BuoyEstimationFilter()
{
}


// --------------------------------------------------------------------------------------

bool BuoyEstimationFilter::isBuoyFound() const 
{
//     std::list<BuoyFeatureVector>::const_iterator it;
// 
//     if( features.size() == feature_buffer_size ) {
//         double radius = getAverageRadius();
// 
//         for(it = features.begin(); it != features.end(); it++) {
//             const feature::Buoy buoy = it->front();
//             CvPoint location = cvPoint(buoy.image_x, buoy.image_y);
// 
//             if( !checkLocation(location, radius) )
//                 return false;
//         }
// 
//         return true;
//     }

    return true;
}



BuoyFeatureVector BuoyEstimationFilter::process()
{
    BuoyFeatureVector vector;
    double MAX_DIST = 25.0;

    if(features_history.size() <= 1)
        return vector;

    BuoyFeatureVector::const_iterator current, past;
    BuoyFeatureVector& current_sample = features_history.front();
    BuoyFeatureVector& past_sample = features_history.back();

    for(current = current_sample.begin(); current != current_sample.end(); current++)
    {
        for(past = past_sample.begin(); past != past_sample.end(); past++) {
            int x = current->image_x - past->image_x;
            int y = current->image_y - past->image_y;

            double amount = sqrt( x * x + y * y);

            if(amount < MAX_DIST) {
                vector.push_back(*current);
            }
        }
    }
   
    /*
    for (std::list<BuoyFeatureVector>::const_iterator newBuoy = features.begin(); newBuoy != features.end(); ++newBuoy) {
        int neighbors = 0;
        int x = newBuoy->begin()->image_x;
        int y = newBuoy->begin()->image_y;
        
        for (std::list<BuoyFeatureVector>::const_iterator oldBuoy = features_history.begin(); oldBuoy != features_history.end(); oldBuoy++)
        {
            if (oldBuoy->begin()->image_x < x + MAX_DIST && oldBuoy->begin()->image_x > x - MAX_DIST)
            {
                if (oldBuoy->begin()->image_y < y + MAX_DIST && oldBuoy->begin()->image_y > y - MAX_DIST)
                {
                    neighbors += 1;
                }
            }
        }
    
        //Add Buoy to Vector
        if (neighbors > MIN_NEIGHBORS) {
            feature::Buoy best_buoy = *newBuoy->begin();
            vector.push_back(best_buoy);
        }
    }*/

    //double radius = getAverageRadius(); //KA Was das macht ich benutze das nicht
//     feature::Buoy best_buoy = features.back().front();
//     vector.push_back(best_buoy);
    return vector;
}



void BuoyEstimationFilter::feed(const BuoyFeatureVector& vector) 
{
    if(features_history.size() >= feature_buffer_size) 
        features_history.pop_front();

    features_history.push_back(vector);
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
