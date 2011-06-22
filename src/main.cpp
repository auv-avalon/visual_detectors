#include <iostream>
#include "buoy_paradise_filter.h"
#include <avalon_base/feature.h>
#include "buoy_interface.h"

using namespace avalon;
//ein automatisch generierter test für basics

void output(const BuoyFeatureVector vector, int step)
{
    std::cout << "========== " << step << ". AUSWERTUNG============" << std::endl;
    for(unsigned int i=0;i<vector.size();i++)
    {
        std::cout << i << ". Buoy - val - x - y - schritt = " << vector[i].validation << " - "
                                       << vector[i].image_x << " -" << vector[i].image_y << " - "
                                       << vector[i].image_radius << std::endl;
    }
}

BuoyFeatureVector test1(int timesteps)
{
    std::cout << "                 TEST 1;" << std::endl;
    BuoyParadiseFilter filter = BuoyParadiseFilter();
    for(int i=0;i<timesteps;i++)
    {
        BuoyFeatureVector vector;
        for(int j=0;j<(i+1)%4;j++)
        {
            int x,y,r;
            x=y=r=i;
            x=y=50*(j+1)*(i+1);
            feature::Buoy buoy = feature::Buoy(x,y,r);
            vector.push_back(buoy);
        }
        filter.feed(vector);
        output(filter.process(),i);
    }
    return filter.process();
}

//ein von hand programmierter test
BuoyFeatureVector test2()
{
    BuoyParadiseFilter filter = BuoyParadiseFilter();
    //erstellen des 1. schrittes
    BuoyFeatureVector s1;
    s1.push_back(feature::Buoy(400,500,1)); //echt
    s1.push_back(feature::Buoy(200,100,1)); //falsch
    //erstellen des 2. schrittes
    BuoyFeatureVector s2;
    s2.push_back(feature::Buoy(370,450,2)); //echt
    //erstellen des 3. schrittes
    BuoyFeatureVector s3;
    s3.push_back(feature::Buoy(360,440,3)); //echt
    //erstellen des 4. schrittes
    BuoyFeatureVector s4;
    s4.push_back(feature::Buoy(360,460,4)); //echt
    //erstellen des 5. schrittes
    BuoyFeatureVector s5;
    s5.push_back(feature::Buoy(190,100,5)); //falsch

    //einspeisen der schritte
    filter.feed(s1);
    output(filter.process(),1);
    filter.feed(s2);
    output(filter.process(),2);
    filter.feed(s3);
    output(filter.process(),3);
    filter.feed(s4);
    output(filter.process(),4);
    filter.feed(s5);
    output(filter.process(),5);

    return filter.process(); 
}

BuoyFeatureVector test3()
{
    BuoyParadiseFilter filter = BuoyParadiseFilter();
    for(int i=0;i<1000;i++)
    {
        BuoyFeatureVector v;
        if(i%60==0)
        {
            v.push_back(feature::Buoy(50,50,i));
        } else v.push_back(feature::Buoy(100,i,i));
        filter.feed(v);
        if(i%100==0) output(filter.process(),i);
    }
    return filter.process();
}

int main(){
    std::cout << "Testprogramm gestartet..." << std::endl;

    //BuoyFeatureVector vector=test1(10);
    //BuoyFeatureVector vector=test2();
    BuoyFeatureVector vector=test3();

}
