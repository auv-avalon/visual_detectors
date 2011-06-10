/*
 * Buoy Estimation Filter - buoy_estimation_filter.h
 * Intelligent Filter trying to estimate a real buoy in 
 * a given feature vector
 * 
 * created by: 
 *     TODO: 
 */

#ifndef VISUAL_DETECTORS__BUOY_PARADISE_FILTER_H_
#define VISUAL_DETECTORS__BUOY_PARADISE_FILTER_H_

#include <stddef.h>
#include <opencv/cv.h>
#include <list>
#include "buoy_interface.h"
#include <avalon_base/feature.h>
#include <stdio.h>
#include <math.h>
#include <vector>

namespace avalon {

//typedef std::vector<feature::Buoy> BuoyFeatureVector;

/**
 * A Buoy estimation filter who is searching the best possible
 * match of a buoy in a given set of feature vectors over the time.
 */
class BuoyParadiseFilter : public BuoyFilter 
{
 
 public:
    /** 
     * generates a standard buoy estimation filter with default configuration
     */
    BuoyParadiseFilter();

    /** 
     * standard destructor for releasing all allocated resources
     */
    ~BuoyParadiseFilter();

    /**
     * has the filter already found a buoy in a given data source (feature vectors)
     * @return true if a real buoy is detected
     */
    bool isBuoyFound() const;

    /**
     * returns the feature of the best possible recognized buoy for a given set of data
     * @return buoy feature of the best possible match
     */
    BuoyFeatureVector process();

    /**
     * add feature vector to this filter in order to estimate the best possible match
     * @param vector a feature vector that is generated by a buoy detector
     */
    void feed(const BuoyFeatureVector& vector);


    /**
     * sets the size of the feature buffer processed by this filter
     * @param size current size of the buffer
     */
     void configureBuoysBufferSize(int size) { 
         buoys_buffer_size = size;
     }

    void doTimestep();

 private:
    BuoyFeatureVector buoys_buffer;

    // maximum number of buoys analysed by this filter
    unsigned int buoys_buffer_size;

    // minimum number of buoys to stay in the buffer
    int buoys_buffer_size_min;

    // loss of validation per timestep
    double timesteploss;

    // validation mit der jede boye startet
    double startvalidation;

    // minimum distance between buoys to be the same
    double mindist;
    // maximale validation die durch nähe hinzu gefügt werden kann
    double maxval;
    
    void setValidations(const BuoyFeatureVector& vector);

    

};

} // namespace avalon

#endif // VISUAL_DETECTORS__BUOY_ESTIMATION_FILTER_H_

