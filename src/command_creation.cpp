#include "command_creation.h"


//using namespace buoydetector;
using namespace avalon;

CommandCreator::CommandCreator() : good_dist(2)
{

}

CommandCreator::CommandCreator(double d) : good_dist(d)
{

}


base::AUVPositionCommand CommandCreator::centerBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth)
{
        double heading = 0;
        if(buoy.world_coord(0)!=0)
        {
            heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
            heading*=1;
        }
        double z = rbs.position[2];
        base::AUVPositionCommand command;
        command.heading =heading;
        command.x = (buoy.world_coord(0) - good_dist)*0.2;  //distance
	// cap the maximum x speed
	if(command.x > 0.2)
	    command.x = 0.2;
	if(command.x < -0.2)
	    command.x = -0.2;
        command.y =0; // no strafing
        command.z = desired_buoy_depth;//buoy.world_coord(2)+z;	//depth
    return command;
}

base::AUVPositionCommand CommandCreator::strafeBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double intensity, double desired_buoy_depth)
{
    base::AUVPositionCommand command;
    double z = rbs.position[2];
    double heading = 0;
    if(buoy.world_coord(0)!=0)
    {
        heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
        heading*=1;
    }

    if(intensity>0){  //strafe nach links
        if(heading>0) heading=0;  //hier nicht nach links drehen
        command.x=0;
        command.y=intensity;
        command.heading=heading;
        command.z = desired_buoy_depth;//buoy.world_coord(2)+z;	//depth
    }else{            //strafe nach rechts
        if(heading<0) heading=0;  //hier nicht nach rechts drehen
        command.x=0;
        command.y=intensity;
        command.heading=heading;
        command.z = desired_buoy_depth;//buoy.world_coord(2)+z;	//depth
    }
    return command;
}
