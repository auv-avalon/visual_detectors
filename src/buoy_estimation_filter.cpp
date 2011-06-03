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



BuoyFeatureVector BuoyEstimationFilter::process()
{
    BuoyFeatureVector vector;
    int MAX_DIST = 100;
    int MIN_NEIGHBORS = 1;
    

    int x = features.back().front().image_x;
    int y = features.back().front().image_y;
    int neighbors = 0;

    const feature::Buoy *best_buoy; //Return Bouy
    
    for (std::list<BuoyFeatureVector>::const_iterator it = features.begin(); it != features.end(); it++ )
    {
      if(it->begin()->image_x < x + MAX_DIST && it->begin()->image_x > x - MAX_DIST)
      {
        if(it->begin()->image_y < y + MAX_DIST && it->begin()->image_y > y - MAX_DIST)
        {
          neighbors += 1;
        }
      }
//         if (it->begin()->image_x > x && it->begin()->image_y > y)
//         {
//             best_buoy =  &it->front();
//             printf("o%d", it->front().image_x);
//         }
    }

    if (neighbors > MIN_NEIGHBORS){
      best_buoy = &features.back().front();
    }

//     std::cout << "test";
    
    //feature::Buoy best_buoy = features.back().front();

    double radius = getAverageRadius(); //KA Was das macht ich benutze das nicht

    //last_buoy.image_radius = radius;
    vector.push_back(*best_buoy);

    return vector;
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
