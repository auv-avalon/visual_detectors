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
                                           startvalidation(100), mindist(100), maxage((int64_t)1500000)
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
        	if(buoys_buffer.size()==0)
        	{
        		b=false;
        		continue;
        	}
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
	std::cout << "process" << std::endl;
	//return buoys_buffer.back();


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
	std::cout << "feed  " << std::endl;
//	buoys_buffer.push_back(input_vector);
//	return;
	std::cout << "vector  " << std::endl;
    BuoyFeatureVector vector = input_vector;
    std::cout << "setValidations  " << std::endl;
    setValidations(vector);
    std::cout << "mergeVectors  " << std::endl;
    mergeVectors(vector);
    std::cout << "sort  " << std::endl;
    std::sort(buoys_buffer.begin(), buoys_buffer.end(), &avalon::valsumComparison);
    std::cout << "doTimestep  " << std::endl;
    doTimestep();
    std::cout << "end" << std::endl;
}

//void BuoyParadiseFilter::setBufferSize(unsigned int i){buoys_buffer_size=i;}
    void BuoyParadiseFilter::setBufferSize(unsigned int i){buoys_buffer_size=i;}
    void BuoyParadiseFilter::setMinSize(unsigned int i){buoys_buffer_size_min=i;}
    void BuoyParadiseFilter::setStartval(unsigned int i){startvalidation=i;}
    void BuoyParadiseFilter::setMindist(unsigned int i){mindist=i;}
    void BuoyParadiseFilter::setMaxage(unsigned int i){maxage=(int64_t)(i*10000);}

}
