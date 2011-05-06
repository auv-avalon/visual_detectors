#include "buoy_estimation_filter.h"

namespace avalon {

BuoyEstimationFilter::BuoyEstimationFilter()
{
}


BuoyEstimationFilter::~BuoyEstimationFilter()
{
}


// --------------------------------------------------------------------------------------

bool BuoyEsitmationFilter::hasBuoyFound() const 
{
    return false;
}



feature::Buoy BuoyEstimationFilter::getBestFeature() const 
{
    feature::Buoy buoy;

    return buoy;
}



void BuoyEstimationFilter::feed(const BuoyFeatureVector& vector) 
{
}




} // namespace avalon
