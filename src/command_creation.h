#include <math.h>
#include <iostream>
#include <base/motion_command.h>
#include <base/samples/rigid_body_state.h>
#include "buoy_interface.h"


namespace avalon{

class CommandCreator{
    private:
    base::samples::RigidBodyState ot;    
    double good_dist;



    public:
    CommandCreator();

    CommandCreator(double d);

    base::AUVPositionCommand createPositionCommand(feature::Buoy &buoy, base::samples::RigidBodyState rbs);
};

}
