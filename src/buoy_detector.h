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

     //BuoyFeatureVector detect(IplImage* image, double factor);

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
     void configureHoughThreshold(int threshold) { configHoughThreshold = threshold; }

     /**
      * configure the threshold using for edge detection
      * @param threshold value
      */
     void configureEdgeThreshold(int threshold) { configEdgeThreshold = threshold; }
     
     /**
     * compute two orthogonal linear regressions in order to convert 
     * any arbitrary image into a flat background image without any 
     * pictorial content.
     */
     void shadingRGB(IplImage* src, IplImage* dest);
     // TODO: more configuration methods if necessary

      /**
      * Creates a histogram image for a specific gray image
      * @param 
      * @return 
      */

     int merge(IplImage* dest, IplImage* src1, IplImage* scr2,
		int th1, int th2, int steps, bool negativColor1, bool negativColor2,
		bool rekursion, int past, bool testMode);

     BuoyFeatureVector detect(IplImage* image, double factor);
     BuoyFeatureVector detect(IplImage* frame, IplImage* h_plane, IplImage* s_plane, double factor);

     BuoyFeatureVector detectBuoy(IplImage* img,
		int height, int h_threshold,int s_threshold, int v_threshold, int steps,
		int pastAverageDark, bool testMode);
    
     BuoyFeatureVector buoyDetection(IplImage* img,int height, double h_threshold, double s_threshold);


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
