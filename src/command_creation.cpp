#include "command_creation.h"


//using namespace buoydetector;
using namespace avalon;

CommandCreator::CommandCreator() : good_dist(2)
{

}

CommandCreator::CommandCreator(double d) : good_dist(d)
{

}

void CommandCreator::setGoodDist(double d)
{
    good_dist=d;
}


base::AUVPositionCommand CommandCreator::centerBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth, double maxX, double headingFactor)
{
        double heading = 0;
        if(buoy.world_coord(0)!=0)
        {
            heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
            heading*=headingFactor;
        }
        //double z = rbs.position[2];
        base::AUVPositionCommand command;
        command.heading =heading;
        command.x = (buoy.world_coord(0) - good_dist)*0.2;  //distance
	// cap the maximum x speed
	if(command.x > maxX)
	    command.x = maxX;
	if(command.x < -maxX)
	    command.x = -maxX;
        command.y = 0; // no strafing
        command.z = desired_buoy_depth;//buoy.world_coord(2)+z;	//depth
    return command;
}

base::AUVPositionCommand CommandCreator::strafeBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double intensity, double desired_buoy_depth, double headingFactor, double headingModulation)
{
    base::AUVPositionCommand command;
    //double z = rbs.position[2];
    double heading = 0;
    if(buoy.world_coord(0)!=0)
    {
        heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
        heading*=headingFactor;
    }
//TODO:: mark fragen ob ob es ok ist heading in die falsche richtung zunächst auf 0 zu setzen
    if(intensity>0){  //strafe nach links
//        if(heading>0) heading=0;  //hier nicht nach links drehen
        command.x=0;
        command.y=intensity;
        command.heading=heading - headingModulation;
        command.z = desired_buoy_depth;//buoy.world_coord(2)+z;	//depth
    }else{            //strafe nach rechts
//        if(heading<0) heading=0;  //hier nicht nach rechts drehen
        command.x=0;
        command.y=intensity;
        command.heading=heading + headingModulation;
        command.z = desired_buoy_depth;//buoy.world_coord(2)+z;	//depth
    }
    return command;
}

base::AUVPositionCommand CommandCreator::cutBuoy(base::samples::RigidBodyState rbs, double desired_buoy_depth, double h)
{
    //double z = rbs.position[2]+0.4;
    base::AUVPositionCommand command;
    command.heading =0;
    command.x = 0.5;  //distance
	// cap the maximum x speed
    command.y =0; // no strafing
    command.z = desired_buoy_depth+h;//buoy.world_coord(2)+z;	//depth
    return command;
}


base::AUVPositionCommand CommandCreator::cutBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth, double h)
{
    double heading = 0;
    if(buoy.world_coord(0)!=0)
    {
        heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
        heading*=1;
    }
    //double z = rbs.position[2]+0.4;
    base::AUVPositionCommand command;
    command.heading =heading;
    command.x = 0.5;  //distance
	// cap the maximum x speed
    command.y =0; // no strafing
    command.z = desired_buoy_depth+h;//buoy.world_coord(2)+z;	//depth
    return command;
}

base::AUVPositionCommand avalon::CommandCreator::giveInverse(base::AUVPositionCommand c)
{
    base::AUVPositionCommand command = c;
    command.heading=-command.heading;
    command.x=-command.x;
    command.y=-command.y;
    command.z=-command.z;
    
    return command;
}
