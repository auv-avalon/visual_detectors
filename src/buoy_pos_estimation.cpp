#include "buoy_pos_estimation.h"

namespace avalon{

    void BuoyPosEstimator::estimateCWKoordinates(feature::Buoy& buoy,frame_helper::FrameHelper fh, base::samples::frame::Frame &frame)
    {
        //calculate the distance to buoy (x-koordinate)
        buoy.world_coord[0]=fh.calcDistanceToObject(frame,buoy.image_radius,buoyRadius,buoy.image_radius,buoyRadius);
	//calculate the y and z koordinate
        cv::Point2f point2d=fh.calcRelPosToCenter(frame, buoy.image_x,buoy.image_y, buoy.world_coord[0]);
        buoy.world_coord[1]=point2d.x;
        buoy.world_coord[2]=point2d.y;
    }


    base::Vector3d BuoyPosEstimator::estimateAuvKoordinates(feature::Buoy& buoy, base::samples::frame::Frame &frame)
    {
        frame_helper::FrameHelper fh = frame_helper::FrameHelper();
        //estimate world-koordinates relative to camera
        estimateCWKoordinates(buoy,fh,frame);
        //transfer the koordinates relative to the AUV
        //TODO:...


        return buoy.world_coord;
    }
    
}