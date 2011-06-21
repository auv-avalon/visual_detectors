#include "buoy_paradise_filter.h"
#include <iostream>

namespace avalon {

//bool feature::Buoy::operator< (const Buoy &a, const Buoy &b)
//{
//    return a.validation<b.validation;
//}

bool BuoyParadiseFilter::compare(feature::Buoy b1, feature::Buoy b2)
{
    return b1.validation>b2.validation;
}

BuoyParadiseFilter::BuoyParadiseFilter() : buoys_buffer_size(20), buoys_buffer_size_min(10), timesteploss(1.75),
                                           startvalidation(100), mindist(150), maxval(50),valthreshhold(7),
                                           multiplicator(1.4)
{
    std::cout << "ParadiseFilter initialisiert:" << std::endl;
    std::cout << "        buoys_buffer_size = " << buoys_buffer_size << std::endl;
    std::cout << "        buoys_buffer_size_min = " << buoys_buffer_size_min << std::endl;
    std::cout << "        timesteploss = " << timesteploss << std::endl;
    std::cout << "        startvalidation = " << startvalidation << std::endl;
    std::cout << "        mindist = " << mindist << std::endl;
    std::cout << "        maxval = " << maxval << std::endl;
}


BuoyParadiseFilter::~BuoyParadiseFilter()
{
}

bool BuoyParadiseFilter::isBuoyFound() const 
{
    if (buoys_buffer.size()>=1) return true;
    return false;
}

void BuoyParadiseFilter::doTimestep(double t)
{
    for(BuoyFeatureVector::iterator it=buoys_buffer.begin();it<buoys_buffer.end();it++)
    {
     //   it->validation-=timesteploss;
     //   if(it->validation<0)it->validation=0;
          it->validation/=t;//buoys_buffer.size();
          if(it->validation<valthreshhold)
              buoys_buffer.erase(it);
    }
}

void BuoyParadiseFilter::setValidations(BuoyFeatureVector& vector)
{
    //berechnen der validation für die buoy
    std::vector<int> index;
    std::vector<double> values;
    for(unsigned int j=0;j<vector.size();j++)
    {
        vector[j].validation=startvalidation;//buoys_buffer_size-buoys_buffer.size();
	int i=0;
        for(BuoyFeatureVector::iterator it=buoys_buffer.begin();it<buoys_buffer.end();it++)
        {
            double x=it->image_x-vector[j].image_x;
            double y=it->image_y-vector[j].image_y;
            int dist=sqrt(x*x+y*y);
            if(dist>mindist)
            {
                continue;
            }

            //berechnen der validation für die buoy
	    /*
             *  =>> sorgt dafür das maximal 1.2 mal die validation der jeweiligen "nachbar"-buoy addiert wird
             */
            double raddiv=it->image_radius-vector[j].image_radius;  //ein faktor der sich aus der Radius-Differenz ergibt
            raddiv=sqrt(raddiv*raddiv);
            raddiv=1-raddiv/it->image_radius;
            if(raddiv<0)raddiv=0;
            double val=maxval-dist*(maxval/mindist); //eine gerade abhängig von der Distanz
            double val2=multiplicator*raddiv*it->validation*(val/maxval)*(val/maxval);
            if(val<0) val=0;
            vector[j].validation+=val2;
            //merken welche bojen noch um welche werte erhöht werden müssen
            //ACHTUNG: hier werden die bojen um einen wert erhöht der von ihrer eigenen val ab hängt
            index.push_back(i);
            values.push_back(val*multiplicator*raddiv);

	    i++;
        }
    }
    //erhöhen der bojen um die entsprechenden werte
    for(unsigned int i=0;i<index.size();i++)
    {
        int j=index[i];
        buoys_buffer[j].validation+=values[i];
    }
    return;
}

/*
 * TODO: Diese Methode implementieren!!!
 */
BuoyFeatureVector BuoyParadiseFilter::process()
{
//    std::cout << buoys_buffer_size<<" "<<timesteploss<<" "<<startvalidation<<" "<<mindist<<" "<<maxval<<" "<<valthreshhold
//                                  <<" "<<multiplicator<<std::endl;
    return buoys_buffer;
}

/*
 * TODO: Diese Methode implementieren!!!
 */
void BuoyParadiseFilter::feed(const BuoyFeatureVector& input_vector) 
{
    BuoyFeatureVector vector=input_vector;

    if(vector.size()==0)  //wenn nicht rein gegeben wird
    {                     //führe einen schwächeren timestep aus
        doTimestep(timesteploss);
        return;
    }
    doTimestep(timesteploss);

    setValidations(vector);

    //einfügen der neuen buoys         TODO: sortiert derzeit falsch herum
    for(unsigned int i=0;i<vector.size();i++)
    {
         buoys_buffer.push_back(vector[i]);
    }
    std::sort(buoys_buffer.begin(), buoys_buffer.end());

    //entfernen alter buoys
    while(buoys_buffer.size()>buoys_buffer_size)
    {
        buoys_buffer.pop_back();
    }
}

void BuoyParadiseFilter::setBufferSize(unsigned int i){buoys_buffer_size=i;}
void BuoyParadiseFilter::setTimesteploss(double d){timesteploss=d;}
void BuoyParadiseFilter::setStartvalidation(double d){startvalidation=d;}
void BuoyParadiseFilter::setMindist(double d){mindist=d;}
void BuoyParadiseFilter::setMaxval(double d){maxval=d;}
void BuoyParadiseFilter::setThreshhold(double d){valthreshhold=d;}
void BuoyParadiseFilter::setMultiplicator(double d){multiplicator=d;}

}
