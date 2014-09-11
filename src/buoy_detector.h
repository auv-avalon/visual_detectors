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
      * configure the min hue value for buoy extracting in hsv space
      * @param low lower hue value for filterByHue
      */

     /**
      * configure the max hue value for buoy extracting in hsv space
      * @param low lower hue value for filterByHue
      */

     /**
      * configure the threshold using in houghspace accumulator
      * @param threshold value
      */
     void configureHoughAccumulatorThresholdH(int value) { houghAccumulatorThresholdH = value; }
     void configureHoughAccumulatorThresholdS(int value) { houghAccumulatorThresholdS = value; }
     void configureHoughAccumulatorThresholdV(int value) { houghAccumulatorThresholdV = value; }

     /**
      * configure the threshold using for edge detection
      * @param threshold value
      */
     void configureHoughEdgeThresholdH(int value) { houghEdgeThresholdH = value; }
     void configureHoughEdgeThresholdS(int value) { houghEdgeThresholdS = value; }
     void configureHoughEdgeThresholdV(int value) { houghEdgeThresholdV = value; }
     void configureHoughCircleMin(int value) { houghCircleMin = value; }
     void configureHoughCircleMax(int value) { houghCircleMax = value; }

     void configureHValueMin(int value) { hValueMin = value; }
     void configureHValueMax(int value) { hValueMax = value; }
     void configureSValueMin(int value) { sValueMin = value; }
     void configureSValueMax(int value) { sValueMax = value; }
     void configureVValueMin(int value) { vValueMin = value; }
     void configureVValueMax(int value) { vValueMax = value; }
     void configureHSmooth(int value) { hSmooth = value; }
     void configureSSmooth(int value) { sSmooth = value; }
     void configureVSmooth(int value) { vSmooth = value; }
     //Debug configure
     void configureDebug(bool value) { debug = value;}
     void configureDebugGray(int value) {debug_gray = value;}
     void configureDebugHough(bool h, bool s, bool v) {debug_h = h; debug_s = s; debug_v = v;} 

     void configureUseH(bool value) {useH = value;}
     void configureUseS(bool value) {useS = value;}
     void configureUseV(bool value) {useV = value;}

     /**
     * compute two orthogonal linear regressions in order to convert
     * any arbitrary image into a flat background image without any
     * pictorial content.
     */
     void shadingRGB(IplImage* src, IplImage* dest);
     // TODO: more configuration methods if necessary


     BuoyFeatureVector detect(IplImage* frame, IplImage* h_plane, IplImage* v_plane);



     BuoyFeatureVector buoyDetection(IplImage* img);

    bool findWhiteLight(IplImage* img, feature::Buoy buoy, feature::WhiteLightSettings settings);

	IplImage* getHshaded(); //wichtig
	IplImage* getSshaded();
	IplImage* getVshaded();
	IplImage* getHplane();
	IplImage* getSplane();	//wichtig
	IplImage* getVplane();
	IplImage* getHSVDebug();
	IplImage* getHoughDebug();

	IplImage* getDebugImage();


 private:
     int filterByHue(int H, int S, int V);
     int filterBySaturation(int H, int S, int V);
     int combineAndCount(IplImage *sat,IplImage *val, IplImage *dest );
     bool getWhiteLightState(IplImage *img, feature::WhiteLightSettings settings);
     std::vector<cv::Vec3f> mergeCirclesOfPlanes(std::vector<std::vector<cv::Vec3f> > cirles);

 private:
     //Hough configs
     int houghAccumulatorThresholdH;
     int houghEdgeThresholdH;
     int houghAccumulatorThresholdS;
     int houghEdgeThresholdS;
     int houghAccumulatorThresholdV;
     int houghEdgeThresholdV;
     int houghCircleMin; 
     int houghCircleMax; 

     int hValueMin;
     int hValueMax;
     int sValueMin;
     int sValueMax;
     int vValueMin;
     int vValueMax;
     int hSmooth;
     int sSmooth;
     int vSmooth;
     //Debug
     bool debug;
     int debug_gray;
     bool debug_h;
     bool debug_s;
     bool debug_v;

     bool useH;
     bool useS;
     bool useV;

     //images for debug-output
     //IplImage* h_shaded;
     IplImage* h_plane;
     IplImage* s_plane;
     IplImage* v_plane;
     IplImage* hsv_gray_debug;
     IplImage* hough_debug;
     IplImage* debug_image;
     IplImage* copy;
     IplImage* copy2;
};

} // namespace avalon

#endif // VISUAL_DETECTORS__BUOY_DETECTOR_H_
