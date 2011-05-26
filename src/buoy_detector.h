/*
 * Visual Detectors - buoy_detector.h
 * Implementation for detecing a buoy based on color extraction 
 * and hough transformation in HSV space.
 * 
 * created by: 
 *      Fabian Zenker <fzenker@informatik.uni-bremen.de>
 */

#ifndef VISUAL_DETECTORS__BUOY_DETECTOR_H_
#define VISUAL_DETECTORS__BUOY_DETECTOR_H_

#include <avalon_base/feature.h>
#include <vector>
#include <stddef.h>
#include <opencv/cv.h>

namespace avalon {

typedef std::vector<feature::Buoy> BuoyFeatureVector;


/**
 * Buoydetector for searching possible circles in an image
 */
class BuoyDetector {
    enum {
        cRED, cBLACK, NUM_COLOR_TYPES
    };

 public:
     enum Channel {
        HUE, SATURATION, VALUE
     };

    /** 
      * generates a standard buoy detector with a default configuration
      */
     BuoyDetector();

     /**
      * standard destructor for releasing all allocated resources
      */
     ~BuoyDetector();

     /**
      * let this implementation find circles in a given image
      * @param frame expects an image in the classic opencv format
      * @return a list of all possible found buoys 
      */
     BuoyFeatureVector detect(IplImage* frame);

     /**
      * configure the min hue value for buoy extracting in hsv space
      * @param low lower hue value for filterByHue
      */
     void configureLowHue(int low);

     /**
      * configure the max hue value for buoy extracting in hsv space
      * @param low lower hue value for filterByHue
      */
     void configureHighHue(int high);

     // TODO: more configuration methods if necessary

 public: 
     /**
      * helper function to output a specfic HSV channel. Don't forget
      * to release the image with cvReleaseImage(&frame)
      * @param channel type of channel
      * @param frame a given IplImage
      */
     IplImage* getChannel(enum Channel channel, IplImage* hsvframe);
     IplImage* filterHueChannel(IplImage* hsvframe);
 
 private:
     int filterByHue(int H, int S, int V);
     int filterBySaturation(int H, int S, int V);

 private:
     int satMax;
     int valMax;

     int configLowHue;
     int configHighHue;
};

} // namespace avalon

#endif // VISUAL_DETECTORS__BUOY_DETECTOR_H_
