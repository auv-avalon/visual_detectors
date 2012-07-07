#include "command_creation.h"


//using namespace buoydetector;
using namespace avalon;

CommandCreator::CommandCreator() : good_dist(2), tiefenspiel(0.5)
{
	
}

CommandCreator::CommandCreator(double d) : good_dist(d), tiefenspiel(0.5)
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
    double z = buoy.world_coord(2);
	if(z>0.3)	z=0.3;
    if(z<-0.3)	z=-0.3;
    base::AUVPositionCommand command;
    command.heading =heading*headingFactor;
    command.x = (buoy.world_coord(0) - good_dist)*0.2;  //distance
	// cap the maximum x speed
	if(command.x > maxX)
	    command.x = maxX;
	if(command.x < -maxX)
	    command.x = -maxX;
    command.y = 0; // no strafing
    base::Pose p = rbs.getPose();
    z = p.position[2]+z;	//depth
    if(z > desired_buoy_depth+tiefenspiel) z = desired_buoy_depth+tiefenspiel;
    if(z < desired_buoy_depth-tiefenspiel) z = desired_buoy_depth-tiefenspiel;
	command.z = z;    

//	std::cout << "desired_bouy_depth: " << desired_buoy_depth << ", current depth: " << p.position[2] << ", command.z: " << z << std::endl;

	return command;
}

base::AUVPositionCommand CommandCreator::centerBuoyHeadingFixed(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth, double maxX, double target_heading, double headingFactor){
	double heading = 0;
    if(buoy.world_coord(0)!=0)
    {
        heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
    }
	double current_heading = rbs.getYaw();
	double heading_diff = target_heading-current_heading;
	if(heading_diff<-M_PI)
		heading_diff+=2*M_PI;
	if(heading_diff>M_PI)
		heading_diff-=2*M_PI;

    double z = buoy.world_coord(2);
	if(z>0.3)z=0.3;
    if(z<-0.3)z=-0.3;
    base::AUVPositionCommand command;
    command.heading = (heading_diff+heading)/2*headingFactor;
    command.x = (buoy.world_coord(0) - good_dist)*0.2;  //distance
	if(command.x > maxX)
	    command.x = maxX;
	if(command.x < -maxX)
	    command.x = -maxX;
    command.y = buoy.world_coord(1);
    base::Pose p = rbs.getPose();
    z = p.position[2]+z;	//depth
    if(z > desired_buoy_depth+tiefenspiel) z = desired_buoy_depth+tiefenspiel;
    if(z < desired_buoy_depth-tiefenspiel) z = desired_buoy_depth-tiefenspiel;
	command.z = z;
    return command;

}

base::AUVPositionCommand CommandCreator::strafeBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double intensity, double desired_buoy_depth, double headingFactor, double headingModulation)
{
    base::AUVPositionCommand command;
    double z = buoy.world_coord(2);
    if(z>0.3)z=0.3;
    if(z<-0.3)z=-0.3;
    double heading = 0;
    if(buoy.world_coord(0)!=0)
    {
        heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
        heading*=headingFactor;
    }

    if(intensity>0){  //strafe nach links
    	command.x=0;
    	command.y=intensity;
    	command.heading=heading*headingFactor - headingModulation;
    	base::Pose p = rbs.getPose();
    	z = p.position[2]+z;	//depth
   		if(z > desired_buoy_depth+tiefenspiel) z = desired_buoy_depth+tiefenspiel;
   	 	if(z < desired_buoy_depth-tiefenspiel) z = desired_buoy_depth-tiefenspiel;
		command.z = z;
    }else{            //strafe nach rechts
        command.x=0;
        command.y=intensity;
        command.heading=heading + headingModulation;
    	base::Pose p = rbs.getPose();
    	z = p.position[2]+z;	//depth
    	if(z > desired_buoy_depth+tiefenspiel) z = desired_buoy_depth+tiefenspiel;
    	if(z < desired_buoy_depth-tiefenspiel) z = desired_buoy_depth-tiefenspiel;
		command.z = z;
    }
    return command;
}
/*
 * cutten weiter führen ohne die boje noch zu sehen
 */
base::AUVPositionCommand CommandCreator::cutBuoy(base::samples::RigidBodyState rbs, double desired_buoy_depth, double h)
{
    base::AUVPositionCommand command;
    command.heading =0;
    command.x = 0.8;  //distance
	// cap the maximum x speed
    command.y =0; // no strafing
    base::Pose p = rbs.getPose();
    if(p.position[2]<=desired_buoy_depth+h){
    	command.z = desired_buoy_depth+h;	//depth
    }else{
        command.z = p.position[2]+h;
    }
    //command.z -= 0.2;
    return command;
}

/*
 * boje cutten solange sie noch gesehen wird
 */
base::AUVPositionCommand CommandCreator::cutBuoy(feature::Buoy &buoy, base::samples::RigidBodyState rbs, double desired_buoy_depth, double h)
{
    double heading = 0;
    if(buoy.world_coord(0)!=0)
    {
        heading = atan(buoy.world_coord(1) / buoy.world_coord(0));
    }
    base::AUVPositionCommand command;
    command.heading =heading;
    command.x = 0.4;  //distance
	// cap the maximum x speed
    command.y =0; // no strafing
    base::Pose p = rbs.getPose();
//Die Tiefe erhöht sich immer weiter. daher wäre es sinnvoll eine maximale tiefe desired_buoy_depth+h+0.3 fest zu legen
    command.z = p.position[2]+h;
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
