#include "buoy_paradise_filter.h"

namespace avalon {

BuoyParadiseFilter::BuoyParadiseFilter() : buoys_buffer_size(50), buoys_buffer_size_min(25), timesteploss(20.0),
                                           startvalidation(20), mindist(200), maxval(40)
{
}


BuoyParadiseFilter::~BuoyParadiseFilter()
{
}

bool BuoyParadiseFilter::isBuoyFound() const 
{
    if (buoys_buffer.size()>=1) return true;
    return false;
}

void BuoyParadiseFilter::doTimestep()
{
    for(BuoyFeatureVector::iterator it=buoys_buffer.begin();it<buoys_buffer.end();it++)
    {
          /*
           * TODO: hier muss eine gute policy gefunden werden
           */
    //    it->validation-=timesteploss;
          it->validation/=2;
    }
}

void BuoyParadiseFilter::setValidations(const BuoyFeatureVector& input_vector)
{
    //berechnen der validation für die buoy
    BuoyFeatureVector vector=input_vector;
    
    std::vector<int> index;
    std::vector<double> values;
    for(unsigned int j=0;j<vector.size();j++)
    {
        vector[j].validation=startvalidation;

        for(unsigned int i=0; i<buoys_buffer.size(); i++)
        {
            double x=buoys_buffer[i].image_x-vector[j].image_x;
            double y=buoys_buffer[i].image_y-vector[j].image_y;
            int dist=sqrt(x*x+y*y);
            if(dist>mindist)
            {
                continue;
            }

            //berechnen der validation für die buoy
            // ACHTUNG:: diese formel berücksichtigt die radius-unterschiede noch nicht, nur mittelpunkte!!!
	    /*
             *    1.2*buoys_buffer(i).validation*((maxval-dist*(maxval/dist))/maxval)²
             *  =>> sorgt dafür das maximal 1.2 mal die validation der jeweiligen "nachbar"-buoy addiert wird
             */
            double val=maxval-dist*(maxval/mindist); //eine gerade abhängig von der Distanz
            val=1.2*buoys_buffer[i].validation*(val/maxval)*(val/maxval);
            if(val<0) val=0;
            vector[j].validation+=val;
            //merken welche bojen noch um welche werte erhöht werden müssen
            index.push_back(i);
            values.push_back(val);
        }
    }

    for(unsigned int i=0;i<index.size();i++)
    {
        int j=index[i];
        buoys_buffer[j].validation+=values[i];
    }

    //erhöhen der 
    
}

/*
 * TODO: Diese Methode implementieren!!!
 */
BuoyFeatureVector BuoyParadiseFilter::process()
{
    return buoys_buffer;
}

/*
 * TODO: Diese Methode implementieren!!!
 */
void BuoyParadiseFilter::feed(const BuoyFeatureVector& vector) 
{
    doTimestep();
    setValidations(vector);
        /*
         *    TODO: geht das einfügen an die stelle des iterators so?
         *          werden die anderen buoys nach hinten geschoben oder eines überschrieben?
         */
    for(unsigned int i=0;i<vector.size();i++)
    {
        for (BuoyFeatureVector::iterator it=buoys_buffer.begin() ; it < buoys_buffer.end()+1; it++ )
        {
            //falls die neue buoy das kleinste element der Liste ist oder diese leer ist.
            if(it==buoys_buffer.end())
            {
                buoys_buffer.insert(it,vector[i]);
                continue;
            }
            if(it->validation<vector[i].validation)
            {
                buoys_buffer.insert(it,vector[i]);
            }else continue;
            break;
        }
    }  
    //entfernen alter buoys
    while(buoys_buffer.size()>buoys_buffer_size)
    {
        buoys_buffer.pop_back();
    }
}

} // namespace avalon
