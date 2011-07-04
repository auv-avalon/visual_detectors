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
        	if(buoys_buffer[i].size()==0)
        	{
        		b=false;
        		continue;
        	}
            base::Time t = base::Time::now();
            int64_t x = t.microseconds-buoys_buffer[i].front().stamp.microseconds;
            if(x>maxage)
            {
                if(buoys_buffer[i].size()==1) buoys_buffer[i].pop_back();
                else
                {
                    BuoyFeatureVector v;
                    for(unsigned int j=1;j<buoys_buffer[i].size();j++)
                    {
                        v.push_back(buoys_buffer[i][j]);
                    }
                    buoys_buffer[i]=v;
                }
            }else 
            {
                b=false;
            }
        }
    }
    //entfernen leerer BuoyFeatureVectoren
    std::vector<BuoyFeatureVector> v;
    for(unsigned int i=0;i<buoys_buffer.size();i++)
    {
        if(buoys_buffer[i].size()>0) v.push_back(buoys_buffer[i]);
    }
    buoys_buffer=v;
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
             //   std::sort(it->begin(), it->end(), &avalon::feature::Buoy::timeComparison);
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

feature::Buoy BuoyParadiseFilter::radius_filter(BuoyFeatureVector& buoys)
{
    double f2[]={0.7,0.3};
    double f4[]={0.5,0.35,0,0.15};
    feature::Buoy buoy=buoys.back();
    if(buoys.size()>=4)
    {
        double x=0;
        for(unsigned int i=0;i<4;i++)
	{
	    x+=buoys[buoys.size()-(i+1)].image_radius*f4[i];
	}
	buoy.image_radius=x;
    }else
        if(buoys.size()>=2)
	{
            double x=0;
            for(unsigned int i=0;i<2;i++)
	    {
	        x+=buoys[buoys.size()-(i+1)].image_radius*f4[i];
	    }  
	    buoy.image_radius=x;
	}

    return buoy;
}
/*
 * TODO: Diese Methode implementieren!!!
 */
BuoyFeatureVector BuoyParadiseFilter::process()
{
	//return buoys_buffer.back();

    BuoyFeatureVector vector = BuoyFeatureVector();
    for(unsigned int i=0;i<buoys_buffer.size();i++)
    {
        if(buoys_buffer[i].size()>=buoys_buffer_size_min)
        vector.push_back(radius_filter(buoys_buffer[i]));//buoys_buffer[i].back());
    }
    /*
     * TODO: hier wäre noch ein filter auf den Radius sinnvoll
     */

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
    void BuoyParadiseFilter::setBufferSize(unsigned int i){buoys_buffer_size=i;}
    void BuoyParadiseFilter::setMinSize(unsigned int i){buoys_buffer_size_min=i;}
    void BuoyParadiseFilter::setStartval(unsigned int i){startvalidation=i;}
    void BuoyParadiseFilter::setMindist(unsigned int i){mindist=i;}
    void BuoyParadiseFilter::setMaxage(unsigned int i){maxage=(int64_t)(i*10000);}

}
