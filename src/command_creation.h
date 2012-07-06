#ifndef VISUAL_DETECTORS__COMMAND_CREATION_H_
#define VISUAL_DETECTORS__COMMAND_CREATION_H_

#include <math.h>
#include <iostream>
#include <base/motion_command.h>
#include <base/samples/rigid_body_state.h>
#include "buoy_interface.h"
#include <base/pose.h>


namespace avalon{

enum Direction{
    LEFT, 
    RIGHT
};

class CommandCreator{
    private:
    double good_dist;
	double tiefenspiel;


    public:
    CommandCreator();

    CommandCreator(double d);

    base::AUVPositionCommand centerBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth = -2.4, double maxX = 0.2, double headingFactor = 1.0);
	base::AUVPositionCommand centerBuoyHeadingFixed(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth = -2.4, double maxX = 0.2, double target_heading = 0);
    base::AUVPositionCommand strafeBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double intensity, double desired_buoy_depth = -2.4, double headingFactor = 1.0, double headingModulation = 0.2);
    base::AUVPositionCommand cutBuoy(base::samples::RigidBodyState rbs, double desired_buoy_depth = -2.4, double h = 0.3);
    base::AUVPositionCommand cutBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth = -2.4, double h=0.3);
    base::AUVPositionCommand giveInverse(base::AUVPositionCommand c);
    void setGoodDist(double d);
};

}

#endif // VISUAL_DETECTORS__COMMAND_CREATION_H_
