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
#include <vector>
#include <opencv/cv.h>



namespace avalon{
	
    class BuoyPosEstimator{
        private:
            //der Radius der Boje
            static const float buoyRadius=0.2;
            //die Brennweite der Kamera
            static const float fx=815;		//TODO: Echte Werte heraus finden
            static const float fy=765;

		//woher kriege ich fx und fy???
		//woher hat alex die pixel-breite auf dem die
	        //wo kriege ich breite und höhe des Bildes her?

            static const float imgWidth=1024;
            static const float imgHeight=640;



            //helps estimateAuvKoordinates. calculates the position of the buoy
            //relativ to the camera of the AUV
            //parameters:
            //buoy        a pointer to a Buoy
            void estimateCWKoordinates(avalon::feature::Buoy& buoy,frame_helper::FrameHelper fh, base::samples::frame::Frame &frame);
	    
	    //transforms the Koordinates wich are relative to the camera 
	    //relative to the AUV-Center
	    void transformKoordinates(avalon::feature::Buoy& buoy);
        public:

            //calculates the position of the buoy relativ to the center of the AUV
            //and stores it into the buoy itself
            //parameters:
            //buoy        a pointer to a Buoy
            base::Vector3d estimateAuvKoordinates(avalon::feature::Buoy& buoy,base::samples::frame::Frame &frame);
		
    };
}
