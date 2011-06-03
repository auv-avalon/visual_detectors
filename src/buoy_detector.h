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

#include <stddef.h>
#include "buoy_interface.h"

namespace avalon {


/**
 * Buoydetector for searching possible circles in an image
 * Transform an image to HSV Space and uses Hue and Saturation
 * Channel in order to recognize some circles
 */
class HSVColorBuoyDetector : public BuoyDetector {
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
     HSVColorBuoyDetector();

     /**
      * standard destructor for releasing all allocated resources
      */
     ~HSVColorBuoyDetector();

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

     /**
      * configure the threshold using in houghspace accumulator
      * @param threshold value
      */
     void configureHoughThreshold(int threshold) { configEdgeThreshold = threshold; }

     /**
      * configure the threshold using for edge detection
      * @param threshold value
      */
     void configureEdgeThreshold(int threshold) { configHoughThreshold = threshold; }
     

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
     int configHoughThreshold;
     int configEdgeThreshold;
};

} // namespace avalon

#endif // VISUAL_DETECTORS__BUOY_DETECTOR_H_
