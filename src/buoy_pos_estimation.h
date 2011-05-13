/*
 * Avalon visual_detectors - buoy_pos_estimation.h
 * estimates the position of a buoy detected by the camera
 * 
 * created by: 
 *      Thomas Grassow <deshalb@informatik.uni-bremen.de>
 */


#include <cv.h>
#include <base/samples/frame.h>
#include <frame_helper/FrameHelperTypes.h>
#include <frame_helper/FrameHelper.h>
#include <avalon_base/feature.h>
#include <base/eigen.h>

namespace avalon{
	
    class BuoyPosEstimator{
        private:

        public:

            //calculates the position of the buoy relativ to the center of the AUV
            //and stores it into the buoy itself
            //parameters:
            //buoy        a pointer to a Buoy
            static void estimateAuvKoordinates(feature::Buoy& buoy);

    };

} // namespace avalon
