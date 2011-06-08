/*
 * Visual Detectors - buoy_interface.h
 * General interfaces for all buoy detector and filter implementations
 * 
 * created by: 
 *      Christoph Mueller <christoph.mueller@dfki.de>
 */



#ifndef VISUAL_DETECTORS__BUOY_FILTER_H_
#define VISUAL_DETECTORS__BUOY_FILTER_H_

#include <vector>
#include <avalon_base/feature.h>
#include <opencv/cv.h>

namespace avalon {

typedef std::vector<feature::Buoy> BuoyFeatureVector;

/**
 * Abstract base class for a buoy filter in order to reduce a possible
 * set of buoys.
 */
class BuoyFilter {
 public:
    /**
     * returns a new buoy feature vector with a reduced false positives / negatives rate
     * @return improved buoy feature vector
     */
    virtual BuoyFeatureVector process() = 0;

    /** 
     * add feature vector to his filter in order to reduce false positives / negatives
     * @return a new set of possible buoys
     */
    virtual void feed(const BuoyFeatureVector& vector) = 0;
};

/**
 * Abstract base class for a buoy detector
 */
class BuoyDetector {
 public:
     virtual BuoyFeatureVector detect(IplImage* image) = 0;

     inline static void draw(IplImage* image, const feature::Buoy& buoy) {
         cvCircle(image, cvPoint(buoy.image_x, buoy.image_y), buoy.image_radius, 
                 CV_RGB(255, 0, 0));
     }
};


} // namespace avalon


#endif // VISUAL_DETECTORS__BUOY_FILTER_H_
