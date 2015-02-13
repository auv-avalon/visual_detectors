/*
 * Avalon visual_detectors - buoy_pos_estimation.h
 * estimates the position of a buoy detected by the camera
 * 
 * created by: 
 *      Thomas Grassow <deshalb@informatik.uni-bremen.de>
 */
#ifndef VISUAL_DETECTORS__POS_ESTIMATION_H_
#define VISUAL_DETECTORS__POS_ESTIMATION_H_

#include <base/samples/frame.h>
#include <frame_helper/FrameHelperTypes.h>
#include <frame_helper/FrameHelper.h>
#include "Types.hpp"
#include <base/eigen.h>
#include <vector>

#include <stddef.h>
#include <opencv/cv.h>



namespace avalon{
	
    class BuoyPosEstimator{
        private:
            //der Radius der Boje
            
            //die Brennweite der Kamera
            static constexpr float fx=815;		//TODO: Echte Werte heraus finden
            static constexpr float fy=765;

            //helps estimateAuvKoordinates. calculates the position of the buoy
            //relativ to the camera of the AUV
            //parameters:
            //buoy        a pointer to a Buoy
            void estimateCWKoordinates(avalon::feature::Buoy& buoy,frame_helper::FrameHelper fh, base::samples::frame::Frame &frame, float buoyRadius);
	    
	    //transforms the Koordinates wich are relative to the camera 
	    //relative to the AUV-Center
	    void transformKoordinates(avalon::feature::Buoy& buoy);
        public:

            BuoyPosEstimator();

            //calculates the position of the buoy relativ to the center of the AUV
            //and stores it into the buoy itself
            //parameters:
            //buoy        a pointer to a Buoy
            base::Vector3d estimateAuvKoordinates(avalon::feature::Buoy& buoy,base::samples::frame::Frame &frame, float r);
		
    };
}
#endif
