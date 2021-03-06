#include "buoy_pos_estimation.h"
#include <iostream>

namespace avalon{

    BuoyPosEstimator::BuoyPosEstimator()
    {

    }

    void BuoyPosEstimator::estimateCWKoordinates(feature::Buoy& buoy,frame_helper::FrameHelper fh, base::samples::frame::Frame &frame, float buoyRadius)
    {
	//check if the Frame has fx and fy and set them if not
	if(!frame.hasAttribute("fx"))
		frame.setAttribute<long>("fx",fx);	//möglicherweise sind fx und fy falsch???
	if(!frame.hasAttribute("fy"))
		frame.setAttribute<long>("fy",fy);
	
        //calculate the distance to buoy (x-koordinate)
        buoy.world_coord[0]=fh.calcDistanceToObject(frame,buoy.image_radius,buoyRadius,buoy.image_radius,buoyRadius);//wenn fx u fy an gepasst, hier auch den radius*2
	//calculate the y and z koordinate
        cv::Point2f point2d=fh.calcRelPosToCenter(frame, buoy.image_x,buoy.image_y, buoy.world_coord[0]);
        buoy.world_coord[1]=point2d.x;
        buoy.world_coord[2]=-point2d.y;
        if(buoy.image_x<frame.getWidth()/2 && buoy.world_coord[1]<0) buoy.world_coord[1]*=-1;
        else
        if(buoy.image_x>frame.getWidth()/2 && buoy.world_coord[1]>0) buoy.world_coord[1]*=-1;
    }
    
    void transformKoordinates(feature::Buoy& buoy)
    {
	//transforms by one meter for now
       buoy.world_coord[0]=buoy.world_coord[0]-1;
    }


    base::Vector3d BuoyPosEstimator::estimateAuvKoordinates(feature::Buoy& buoy, base::samples::frame::Frame &frame, float r)
    {
        frame_helper::FrameHelper fh = frame_helper::FrameHelper();
        //estimate world-koordinates relative to camera
        estimateCWKoordinates(buoy,fh,frame, r);
        //transfer the koordinates relative to the AUV
        //TODO:...
	
        return buoy.world_coord;
    }
    
}
