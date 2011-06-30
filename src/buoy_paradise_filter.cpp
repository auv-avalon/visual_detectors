#include "buoy_paradise_filter.h"
#include <iostream>

/*    // maximum number of buoys to be part of one BuoyFeatureVector
    unsigned int buoys_buffer_size;

    // minimum number of buoys in a BuoyFeatureVector to make it valid
    int buoys_buffer_size_min;

    // validation von der ausgehend die validation der buoys berechnet wird
    double startvalidation;

    // minimum distance between buoys to be part of the same BuoyFeatureVector
    double mindist;
    
    // maximales alter einer Buoy in einem BuoyFeatureVector
    base::Time maxage;    */

namespace avalon {

BuoyParadiseFilter::BuoyParadiseFilter() : buoys_buffer_size(5), buoys_buffer_size_min(3),
                                           startvalidation(100), mindist(10), maxage(1500000)
{
}


BuoyParadiseFilter::~BuoyParadiseFilter()
{
}

void BuoyParadiseFilter::doTimestep()
{
    //entfernen zu alter buoys
    for(unsigned int i=0;i<buoys_buffer.size();i++)
    {
        bool b=true;
        
        while(b)
        {
            base::Time t = base::Time::now();
            int64_t x = t.microseconds-buoys_buffer[i].back().stamp.microseconds;
            if(x>maxage)
            {
                buoys_buffer[i].pop_back();
            }else b=false;
        }
    }
}

void BuoyParadiseFilter::setValidations(BuoyFeatureVector& vector)
{
    for(unsigned int i=0;i<vector.size();i++)
    {
        double x = startvalidation - i*startvalidation/vector.size();
        vector[i].validation = x;
    }
}

void BuoyParadiseFilter::mergeVectors(BuoyFeatureVector& vector)
{
    for(unsigned int i=0;i<vector.size();i++)
    {
        bool b=false;
        for(std::vector<BuoyFeatureVector>::iterator it=buoys_buffer.begin();it<buoys_buffer.end() && b==false;it++)
        {
            double diff_x = vector[i].image_x-it->back().image_x;
            double diff_y = vector[i].image_y-it->back().image_y;
            double dist = sqrt(diff_x*diff_x+diff_y*diff_y);
            if(dist<=mindist)
            {
                it->push_back(vector[i]);
                b=true;
                std::sort(it->begin(), it->end(), &avalon::feature::Buoy::timeComparison);
            }
        }
        //if buoy was not merged to any BuoyFeatureVector it starts a new one
        if(b==false)
        {
            BuoyFeatureVector v=BuoyFeatureVector();
            v.push_back(vector[i]);
            buoys_buffer.push_back(v);
        }
    }
}


/*
 * TODO: Diese Methode implementieren!!!
 */
BuoyFeatureVector BuoyParadiseFilter::process()
{
    BuoyFeatureVector vector = BuoyFeatureVector();
    for(unsigned int i=0;i<buoys_buffer.size();i++)
    {
        vector.push_back(buoys_buffer[i].back());
    }

    return vector;
}


/*
 * 
 */
void BuoyParadiseFilter::feed(const BuoyFeatureVector& input_vector) 
{
    BuoyFeatureVector vector = input_vector;
    setValidations(vector);
    mergeVectors(vector);
    std::sort(buoys_buffer.begin(), buoys_buffer.end(), &avalon::valsumComparison);
    doTimestep();
}

//void BuoyParadiseFilter::setBufferSize(unsigned int i){buoys_buffer_size=i;}

}
