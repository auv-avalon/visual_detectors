#ifndef VISUAL_DETECTORS_TYPES_HPP
#define VISUAL_DETECTORS_TYPES_HPP

#include <base/Eigen.hpp>
#include <base/Time.hpp>
#include <base/Float.hpp>

namespace avalon {
namespace feature {

enum BuoyColor{
  WHITE, ORANGE, RED, GREEN, UNKNOWN, NO_BUOY
};
  
struct Buoy
{
        /** x-position of the buoy in image in pixel */
        int image_x;

        /** y-position of the buoy in image in pixel */
        int image_y;

        /** radius of the buoy in in image in pixel */
        int image_radius;
        
        /** propability that this is a real buoy in image */
        double probability;

        /** a variable used by the filter */
        double validation;  //double

        /** timestamp */
        base::Time time;
        
        /** real world coordinates of the buoy */
        base::Vector3d world_coord;     //x=vorne,y=links,z=oben
        
        /**Color of the buoy */
        BuoyColor color;
        
        /**Label of the buoy */
        int label;

        Buoy()
        : image_x(0), image_y(0), image_radius(0), probability(0), validation(0), time(base::Time::now()) {}

        Buoy(int x, int y, int r)
        : image_x(x), image_y(y), image_radius(r), probability(1), validation(0), time(base::Time::now()) {}

        //derzeit falsch herum definiert um das sortieren im filter um zu drehen
        inline friend bool operator< (const Buoy &a, const Buoy &b)
        {
            return a.validation<b.validation;
        }
        inline friend bool operator> (const Buoy &a, const Buoy& b)
        {
            return a.validation>b.validation;
        }

        static bool timeComparison(Buoy const& b0, Buoy const& b1)
        {
            return b0.time > b1.time;
        }
        static bool validityComparison(Buoy const& b0, Buoy const& b1)
        {
            return b0.validation < b1.validation;
        }
        static bool probabilityComparison(Buoy const& b0, Buoy const& b1)
        {
            return b0.probability < b1.probability;
        }
};

struct Gate
{
    /** dummy value */
    int dummy;

    Gate() {}
};

struct WhiteLightSettings{
    double roi_X;
    double roi_Y;
    double roi_width;
    double roi_height;
    int val_Binary_Threshold;
    int sat_Binary_Threshold;
        WhiteLightSettings(double x,double y, double w, double h, int val, int sat)
        : roi_X(x), roi_Y(y), roi_width(w), roi_height(h), val_Binary_Threshold(val), sat_Binary_Threshold(sat) {}
        WhiteLightSettings()
        : roi_X(base::unknown<double>()), roi_Y(base::unknown<double>()), roi_width(base::unknown<double>()), roi_height(base::unknown<double>()), val_Binary_Threshold(base::unknown<int>()), sat_Binary_Threshold(base::unknown<int>()) {}
};

}
}

#endif
