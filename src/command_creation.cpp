#include "command_creation.h"


//using namespace buoydetector;
using namespace avalon;

CommandCreator::CommandCreator() : good_dist(2)
{

}

CommandCreator::CommandCreator(double d) : good_dist(d)
{

}


base::AUVPositionCommand CommandCreator::centerBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs)
{
        double heading = 0;
        if(buoy.world_coord(0)!=0)
        {
            heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
        }
        double z = rbs.position[2];
        base::AUVPositionCommand command;
        command.heading =heading;
        command.x =buoy.world_coord(0) - good_dist;  //distance
        command.y =0; // no strafing
        command.z =buoy.world_coord(2)+z;	//depth
    return command;
}

base::AUVPositionCommand CommandCreator::strafeBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, Direction dir)
{
    base::AUVPositionCommand command;
    double z = rbs.position[2];
    switch(dir)
    {
    case LEFT:
        command.x=0;
        command.y=0.3;
        command.heading=(atan(buoy.world_coord(0) / command.y)-M_PI/2)/8;
        command.z=buoy.world_coord(2)+z;
    break;
    case RIGHT:
        command.x=0;
        command.y=-0.3;
        command.heading=(M_PI/2-atan(buoy.world_coord(0) / command.y))/8;
        command.z=buoy.world_coord(2)+z;
    break;
    }
    return command;
}
