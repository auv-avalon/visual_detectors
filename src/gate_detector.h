/*
 * Visual Detectors - gate_detector.h
 * Implementation for detecing a gate
 * 
 * created by: 
 *      Fabian Zenker <fzenker@informatik.uni-bremen.de>
 */

#ifndef VISUAL_DETECTORS__GATE_DETECTOR_H_
#define VISUAL_DETECTORS__GATE_DETECTOR_H_

#include <avalon_base/feature.h>
#include <stddef.h>
#include <opencv/cv.h>

namespace avalon {

class GateDetector {
 public:
    /**
     * generates a standard gate detector with a default configuration
     */
    GateDetector();

    /** 
     * standard destructor for releasing all allocated resources
     */ 
    ~GateDetector();

    /**
     * find several gates in a given image
     * @param frame expects an image in the classic opencv format
     * @return a list of possible found gates
     */
    std::vector<feature::Gate> detect(IplImage* frame);

 private:
    // member variables
};

} // namespace avalon

#endif // VISUAL_DETECTORS__GATE_DETECTOR_H_
