#include <math.h>
#include <iostream>
#include <base/motion_command.h>
#include <base/samples/rigid_body_state.h>
#include "buoy_interface.h"


namespace avalon{

enum Direction{
    LEFT, 
    RIGHT
};

class CommandCreator{
    private:
    double good_dist;


    public:
    CommandCreator();

    CommandCreator(double d);

    base::AUVPositionCommand centerBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth = -2.5);
    base::AUVPositionCommand strafeBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, Direction dir, double desired_buoy_depth = -2.5);
};

}
