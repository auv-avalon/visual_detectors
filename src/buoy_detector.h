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
     BuoyFeatureVector detect(IplImage* frame, double factor);

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
     IplImage* createHistogram(IplImage* img, int maxRange);

     void imageToSquares(IplImage* img, bool smooth, int squareSize, bool equalizeH, bool equalizeS, bool equalizeV, bool backToRGB);

     void merge(IplImage* dest, IplImage* next, int th, bool negativColor);

     int merge(IplImage* dest, IplImage* src1,  IplImage* scr2, int th1,int th2,bool negativColor1, bool negativColor2,bool rekursion,int past) ;

     void shadingGrey(IplImage* img, int threshold);

     IplImage* getCopy(IplImage* src, int height);
 public: 
     /**
      * helper function to output a specfic HSV channel. Don't forget
      * to release the image with cvReleaseImage(&frame)
      * @param channel type of channel
      * @param frame a given IplImage
      */
     IplImage* getChannel(enum Channel channel, IplImage* hsvframe);

     IplImage* filterHueChannel(IplImage* hsvframe);

     BuoyFeatureVector detectBuoy(IplImage* img, int height, int mergeHValue, int mergeVValue, int pastAverageDark, bool testMode);
 
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
