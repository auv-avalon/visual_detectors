#ifndef VISUAL_DETECTORS__BUOY_DETECTOR_H_
#define VISUAL_DETECTORS__BUOY_DETECTOR_H_

#include <avalon_base/buoyposition.h>
#include <vector>
#include <opencv/cv.h>

namespace avalon {

class BuoyDetector {
    enum {
        cRED, cBLACK, NUM_COLOR_TYPES
    };

 public:
     BuoyDetector();
     ~BuoyDetector();

     std::vector<BuoyPos> detect(IplImage* frame);

     int filterByHue(int H, int S, int V);
     int filterBySaturation(int H, int S, int V);

 private:
     int satMax;
     int valMax;
};

} // namespace avalon

#endif // VISUAL_DETECTORS__BUOY_DETECTOR_H_
