#include "buoy_detector.h"
//#include <stdio.h>
#include <Eigen/Core>
//#include <iostream>
namespace avalon {

// ---------------------------------------------------------------------------------------

// module internal constant values
const unsigned char cCTHue[] = { 0, 0 };
const unsigned char cCTSat[] = { 255, 0 };
const unsigned char cCTVal[] = { 255, 0 };
const CvScalar circleColor = cvScalar(0, 255, 0);


// ---------------------------------------------------------------------------------------

HSVColorBuoyDetector::HSVColorBuoyDetector() :
			houghAccumulatorThresholdH(100), houghAccumulatorThresholdS(100), houghAccumulatorThresholdV(100), houghEdgeThresholdH(200), houghEdgeThresholdS(200), houghEdgeThresholdV(200) {
      h_plane = NULL;
      s_plane = NULL;
      v_plane = NULL;
      hsv_gray_debug = NULL;
      hough_debug = NULL;
      //h_shaded = NULL;
      debug_image = NULL;
      copy = NULL;
}

HSVColorBuoyDetector::~HSVColorBuoyDetector() {
	//Release images
      if(s_plane)
	cvReleaseImage(&s_plane);
      if(h_plane)
	cvReleaseImage(&h_plane);
      if(debug_image)
	cvReleaseImage(&debug_image);
      if(copy)
	cvReleaseImage(&copy);
}

// ---------------------------------------------------------------------------------------



void HSVColorBuoyDetector::shadingRGB(IplImage* src, IplImage* dest) {

	int height = src->height;
	int width = src->width;
	int rowSize = src->widthStep;
	char *pixelStart = src->imageData;

	Eigen::MatrixXf a0R(height, width);
	Eigen::MatrixXf a0G(height, width);
	Eigen::MatrixXf a0B(height, width);

	Eigen::MatrixXf a1R(height, width);
	Eigen::MatrixXf a1G(height, width);
	Eigen::MatrixXf a1B(height, width);

	Eigen::MatrixXf slopeR(2, width);
	Eigen::MatrixXf slopeG(2, width);
	Eigen::MatrixXf slopeB(2, width);

	Eigen::MatrixXf y0R(2, width);
	Eigen::MatrixXf y0G(2, width);
	Eigen::MatrixXf y0B(2, width);

	float mean[3], sum[3], sxy[3], sx2, mid, factor[3];
	int  min[3], max[3];

	// grab the rgb values

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			a0R(y, x) = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0);
			a0G(y, x) = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1);
			a0B(y, x) = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2);
		}
	}

	//regression 1: compute the regression line of any column
	mid = height / (float) 2;
	for (int x = 0; x < width; x++) {
		for (int z = 0; z < 3; z++) {
			sum[z] = 0;
		}
		for (int y = 0; y < height; y++) {
			sum[0] += (int) a0R(y, x);
			sum[1] += (int) a0G(y, x);
			sum[2] += (int) a0B(y, x);
		}
		for (int z = 0; z < 3; z++) {
			mean[z] = sum[z] / height;
			sxy[z] = 0;
		}
		sx2 = 0;
		for (int y = 0; y < height; y++) {
			sxy[0] += (y - mid) * a0R(y, x) - mean[0];
			sxy[1] += (y - mid) * a0G(y, x) - mean[1];
			sxy[2] += (y - mid) * a0B(y, x) - mean[2];

			sx2 += (y - mid) * (y - mid);
		}

		slopeR(1, x) = sxy[0] / sx2;
		slopeG(1, x) = sxy[1] / sx2;
		slopeB(1, x) = sxy[2] / sx2;
		y0R(1, x) = mean[0] - slopeR(1, x) * mid;
		y0G(1, x) = mean[1] - slopeG(1, x) * mid;
		y0B(1, x) = mean[2] - slopeB(1, x) * mid;
	}

	//regression 2: compute the regression line of any row
	mid = width / (float) 2;
	for (int y = 0; y < height; y++) {
		for (int z = 0; z < 3; z++) {
			sum[z] = 0;
		}
		for (int x = 0; x < width; x++) {
			sum[0] += slopeR(1, x) * y + y0R(1, x);
			sum[1] += slopeG(1, x) * y + y0G(1, x);
			sum[2] += slopeB(1, x) * y + y0B(1, x);
		}
		for (int z = 0; z < 3; z++) {
			mean[z] = sum[z] / width;
			sxy[z] = 0;
		}
		sx2 = 0;
		for (int x = 0; x < width; x++) {
			sxy[0] += (x - mid) * (slopeR(1, x) * y + y0R(1, x) - mean[0]);
			sxy[1] += (x - mid) * (slopeG(1, x) * y + y0G(1, x) - mean[1]);
			sxy[2] += (x - mid) * (slopeB(1, x) * y + y0B(1, x) - mean[2]);
			sx2 += (x - mid) * (x - mid);
		}
		slopeR(0, y) = sxy[0] / sx2;
		slopeG(0, y) = sxy[1] / sx2;
		slopeB(0, y) = sxy[2] / sx2;

		y0R(0, y) = mean[0] - slopeR(0, y) * mid;
		y0G(0, y) = mean[1] - slopeG(0, y) * mid;
		y0B(0, y) = mean[2] - slopeB(0, y) * mid;
	}

	//shading correction: subtract the flat background image from the
	//original and rearrange the resulting RGB values between 0 to 255
	for (int z = 0; z < 3; z++) {
		min[z] = 255;
		max[z] = 0;
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			a1R(y, x) = (int) (a0R(y, x) - (slopeR(0, y) * x + y0R(0, y)));
			a1G(y, x) = (int) (a0G(y, x) - (slopeG(0, y) * x + y0G(0, y)));
			a1B(y, x) = (int) (a0B(y, x) - (slopeB(0, y) * x + y0B(0, y)));

			min[0] = (a1R(y, x) < min[0]) ? a1R(y, x) : min[0];
			min[1] = (a1G(y, x) < min[1]) ? a1G(y, x) : min[1];
			min[2] = (a1B(y, x) < min[2]) ? a1B(y, x) : min[2];

			max[0] = (a1R(y, x) > max[0]) ? a1R(y, x) : max[0];
			max[1] = (a1G(y, x) > max[1]) ? a1G(y, x) : max[1];
			max[2] = (a1B(y, x) > max[2]) ? a1B(y, x) : max[2];
		}
	}
	for (int z = 0; z < 3; z++) {
		factor[z] = 255 / (float) (max[z] - min[z]);
	}
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			a0R(y, x) = (int) ((a1R(y, x) - min[0]) * factor[0]);
			a0G(y, x) = (int) ((a1G(y, x) - min[1]) * factor[1]);
			a0B(y, x) = (int) ((a1B(y, x) - min[2]) * factor[2]);
		}
	}
	rowSize = dest->widthStep;
	pixelStart = dest->imageData;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0) = a0R(y, x);
			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1) = a0G(y, x);
			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2) = a0B(y, x);
		}
	}
}



////////SAUC-E/////////////
std::vector<feature::Buoy> HSVColorBuoyDetector::buoyDetection(IplImage* img) {
        //std::cout << "creating copy" << std::endl;
        if(!copy)
        {
	  copy = cvCreateImage(cvGetSize(img), 8, 3);
        }  
	cvCopy(img, copy);

        //only for initialization
        //std::cout << " creating images" << std::endl;
        //std::cout << "creating debug" << std::endl;
        if(!debug_image){
	  debug_image = cvCreateImage(cvGetSize(img), 8, 3);
        }
	//cvCopy(img, debug_image);
	//cvCvtColor(debug_image, debug_image, CV_BGR2RGB); // Image to HSV

        //std::cout << "creating hsv gray" << std::endl;
        if(!hsv_gray_debug){
	  hsv_gray_debug = cvCreateImage(cvGetSize(img), 8, 1);
        }
        //std::cout << "creating hough debug" << std::endl;
        if(!hough_debug){
	  hough_debug = cvCreateImage(cvGetSize(img), 8, 3);
        }
	cvCopy(img, hough_debug);
	
        
        
        cvCvtColor(hough_debug, hough_debug, CV_BGR2RGB); // Image to HSV
	//Split Image to single HSV planes
        if(hValueMax < hValueMin){
            // Image to HSV, interprete red as blue to get better resuls at CVSmoth
	    cvCvtColor(copy, copy, CV_BGR2HSV); 
            
            //Convert hValues from RGB-Based to BGR-based 
            hValueMin = 255 - hValueMin - 85;
            if (hValueMin < 0){
                hValueMin += 256;
            }
            hValueMax = 255 - hValueMax - 85;
            if (hValueMax < 0){
                hValueMax += 256;
            }

        } else{
	    cvCvtColor(copy, copy, CV_RGB2HSV); // Image to HSV
        }

        if (hValueMin > hValueMax){
            int tmp = hValueMin;
            hValueMin = hValueMax;
            hValueMax = tmp;
        }

        //std::cout << "hValueMin: " << hValueMin;
        //std::cout << "           hValueMax: " << hValueMax << std::endl;

        //std::cout << "Creating H Plane" << std::endl;
        if(!h_plane)
	  h_plane = cvCreateImage(cvGetSize(copy), 8, 1);
        //std::cout << "Creating S Plane" << std::endl;
        if(!s_plane)
	  s_plane = cvCreateImage(cvGetSize(copy), 8, 1);
        //std::cout << "Creating V Plane" << std::endl;
        if(!v_plane)
	  v_plane = cvCreateImage(cvGetSize(copy), 8, 1);
        //std::cout << "seperate planes" << std::endl;
        //std::cout << "h" << std::endl;
	cvCvtPixToPlane(copy, h_plane, 0, 0, 0);
        //std::cout << "s" << std::endl;
	cvCvtPixToPlane(copy, 0, s_plane, 0, 0);
        //std::cout << "v" << std::endl;
	cvCvtPixToPlane(copy, 0, 0, v_plane, 0);
        
// Führt zu Speicherfehleri
        //std::cout << "hSmooth : " << hSmooth << std::endl;
        cvSmooth(h_plane, h_plane, CV_MEDIAN, hSmooth);
	cvSmooth(s_plane, s_plane, CV_MEDIAN, sSmooth);
	cvSmooth(v_plane, v_plane, CV_MEDIAN, vSmooth);

        //std::cout << "gray debug" << debug_gray << std::endl;
        if (debug_gray == 0 && debug){
            cvCopy(h_plane, hsv_gray_debug);
        } else if (debug_gray == 1 && debug){
            cvCopy(s_plane, hsv_gray_debug);
        } else if (debug_gray == 2 && debug){
            cvCopy(v_plane, hsv_gray_debug);
        } else if (debug) {
            std::cerr << "Wrong Debug Gray Param" << std::endl;
        }

        //std::cout  << "creating binarys" << std::endl;	
        cvThreshold(h_plane, h_plane, hValueMax, 255, CV_THRESH_TOZERO_INV);
	cvThreshold(h_plane, h_plane, hValueMin, 255, CV_THRESH_BINARY);
	cvThreshold(s_plane, s_plane, sValueMax, 255, CV_THRESH_TOZERO_INV);
	cvThreshold(s_plane, s_plane, sValueMin, 255, CV_THRESH_BINARY);
	cvThreshold(v_plane, v_plane, vValueMax, 255, CV_THRESH_TOZERO_INV);
	cvThreshold(v_plane, v_plane, vValueMin, 255, CV_THRESH_BINARY);
        
	//cvCircle(debug_image, cvPoint(50, 50), 30,
	//		cvScalar(0, 0, 255), 2);
	//Shading correction
	//cvCvtColor(copy, copy, CV_HSV2RGB);
	//shadingRGB(copy, copy);

	//Split shaded images to single HSV planes
	//cvCvtColor(copy, copy, CV_RGB2HSV);

	//create binary images

	//
        //cv::Mat v_mat(v_plane);
        //cv::Mat s_mat(s_plane);
        //cv::Mat and_mat = min(s_mat, v_mat);// ;v_mat & s_mat;
        //std::cout << "1" << std::endl;
        //copy2 = and_mat;
        //cvMin(s_plane, v_plane, hough_debug);
        //std::cout << "2" << std::endl;
        //hough_debug = &copy2;        
        //std::cout << "3" << std::endl;
        
        //debug_image = &((IplImage)and_mat);
        
        //debug_image = cvCloneImage(&(IplImage)and_mat);
        
	//smooth images
        //std::cout << "smooth images" << std::endl;

	//detect buoys
	std::vector < feature::Buoy > result ;
	result = detect(s_plane, h_plane, v_plane);

	return result;
}

std::vector<feature::Buoy> HSVColorBuoyDetector::detect(IplImage* s_plane,
		IplImage* h_plane, IplImage* v_plane) {

	std::vector < feature::Buoy > result;
        //std::cout << "Houghes" << std::endl;
        //std::cout << "H Hough" << std::endl;
        cv::Mat dil_h;
        dil_h = cv::Mat(h_plane,true);
        std::vector < cv::Vec3f > circles_h;
        cv::HoughCircles(dil_h, circles_h, CV_HOUGH_GRADIENT, 2, dil_h.cols / 4, houghEdgeThresholdH, houghAccumulatorThresholdH, houghCircleMin, houghCircleMax);
        if (debug_h){
            for (int i=0; i<circles_h.size(); i++){
                int x = cvRound(circles_h[i][0]);
                int y = cvRound(circles_h[i][1]);
                int r = cvRound(circles_h[i][2]);
                cvCircle(hough_debug, cvPoint(x, y), r,
                        cvScalar(255, 0, 0), 2);
            }
        }
        //std::cout << "S Hough" << std::endl;
        cv::Mat dil_s;
	dil_s = cv::Mat(s_plane,true);
	std::vector < cv::Vec3f > circles_s;
        cv::HoughCircles(dil_s, circles_s, CV_HOUGH_GRADIENT, 2, dil_s.cols / 4, houghEdgeThresholdS, houghAccumulatorThresholdS, houghCircleMin, houghCircleMax);
        if (debug_s){
            for (int i=0; i<circles_s.size(); i++){
                int x = cvRound(circles_s[i][0]);
                int y = cvRound(circles_s[i][1]);
                int r = cvRound(circles_s[i][2]);
                cvCircle(hough_debug, cvPoint(x, y), r,
                        cvScalar(0, 255, 0), 2);
            }
        }
        //std::cout << "V Hough" << std::endl;
        cv::Mat dil_v;
	dil_v = cv::Mat(v_plane,true);
	std::vector < cv::Vec3f > circles_v;
	cv::HoughCircles(dil_v, circles_v, CV_HOUGH_GRADIENT, 2, dil_v.cols / 4, houghEdgeThresholdV, houghAccumulatorThresholdV, houghCircleMin, houghCircleMax);
        if (debug_v){
            for (int i=0; i<circles_v.size(); i++){
                int x = cvRound(circles_v[i][0]);
                int y = cvRound(circles_v[i][1]);
                int r = cvRound(circles_v[i][2]);
                cvCircle(hough_debug, cvPoint(x, y), r,
                        cvScalar(0, 0, 255), 2);
            }
        }

        std::vector<std::vector<cv::Vec3f> > to_merge;
        to_merge.push_back(circles_h);
        to_merge.push_back(circles_s);
        to_merge.push_back(circles_v);
       
        std::vector<cv::Vec3f> circles = mergeCirclesOfPlanes(to_merge);
        //std::cout << "BUOYS FOUND: " << circles.size() << std::endl;
        
        for (int i=0; i<circles.size(); i++){
            int x = cvRound(circles[i][0]);
            int y = cvRound(circles[i][1]);
            int r = cvRound(circles[i][2]);
            cvCircle(hough_debug, cvPoint(x, y), r, cvScalar(255, 255, 0), 2);
        }

	for (int i = 0; i < circles.size(); i++) {
		int x = cvRound(circles[i][0]);
		int y = cvRound(circles[i][1]);
		int r = cvRound(circles[i][2]);

                if(x < 2)
                   x = 2;
                if(x > s_plane->width - 2)
                   x = s_plane->width - 2;
                if(y < 2)
                   y = 2;
                if(y > s_plane->height - 2)
                   y = s_plane->height - 2;
		int counter = 0;
		for (int j = x - 2; j <= x + 2; j++) {
			for (int k = y - 2; k <= y + 2; k++) {
				int h_value = ((uchar *) (h_plane->imageData + k
						* h_plane->widthStep))[j];
				if (h_value == 255) {

					int s_value = ((uchar *) (s_plane->imageData + k
							* s_plane->widthStep))[j];
					if (h_value == s_value) {
						counter++;
					}
				}

			}
		}

		if (counter / 24.0 > 0.8) {

			feature::Buoy data(x, y, r);

			result.push_back(data);
		}

	}


	return result;
}

std::vector<cv::Vec3f> HSVColorBuoyDetector::mergeCirclesOfPlanes(std::vector<std::vector< cv::Vec3f> > circles){
    if(circles.size() == 1){
        return circles.at(0);
    } else {
        std::vector<cv::Vec3f> current = circles.back();
        circles.pop_back();
        std::vector<cv::Vec3f> merge = mergeCirclesOfPlanes(circles);
        std::vector<cv::Vec3f> ret;

        for(unsigned i = 0; i < merge.size(); i++){
            for(unsigned n = 0; n < current.size(); n++){
                cv::Vec3f distance = merge.at(i) - current.at(n);
                distance[2] = 0; //remove the radius
                if(norm(distance) < (merge.at(i)[2] + current.at(n)[2])){
                    ret.push_back(((merge.at(i) * (int)circles.size()) + current.at(n)) * (1.0/(double)(circles.size()+1))); 
                }
            }
        }
        return ret;
    }

}



//White Light Detection

bool HSVColorBuoyDetector::findWhiteLight(IplImage* img, feature::Buoy buoy, feature::WhiteLightSettings settings)
{

    double roi_width = settings.roi_width;
    double roi_height = settings.roi_height;
    bool result = false;
    int width = (int)(roi_width*buoy.image_radius);
    int height = (int)(roi_height*buoy.image_radius);
    int p1_x = buoy.image_x - width/2 - (settings.roi_X * buoy.image_radius);
    int p1_y = buoy.image_y- buoy.image_radius -height - (settings.roi_Y * buoy.image_radius) ;
    int p3_x = p1_x + width;


    if(p1_x<1){
        p1_x =1;
    }
    CvSize size=cvGetSize(img);
  

    if(p1_y < 10){
	height = height - (10- p1_y);
	if(height<5){
	return false;
	}
        p1_y = 10;

    }

    if(p3_x > size.width-1){
        width=size.width-1-p1_x;
    }

    CvRect rect = cvRect(p1_x,p1_y, width, height);

    if(rect.y > 0 && rect.x > 0 && p1_x + width < size.width && rect.y +height < size.height){
    	cvSetImageROI(img, rect);
    	result = getWhiteLightState(img, settings);
    	cvResetImageROI(img);
    }

    return result;
}

int HSVColorBuoyDetector::combineAndCount(IplImage *sat,IplImage *val, IplImage *dest )
{

	CvSize size=cvGetSize(sat);

	uchar *dataSat  = (uchar *)sat->imageData;
	uchar *dataVal  = (uchar *)val->imageData;
	uchar *dataDest  = (uchar *)dest->imageData;
	int step = sat->widthStep;

    int counter =0;
	for(int i=0;i< size.height;i++)
	{
		for(int j=0;j< size.width;j++)
		{
			uchar curSat = dataSat[i*step+j];
			uchar curVal = dataVal[i*step+j];


			//if(curSat==0 &&curVal==255)
			if(curSat==255)
			{
				((uchar *)(dataDest+i*step))[j]=255;
				counter++;
			}
			else
			{
				((uchar *)(dataDest+i*step))[j]=0;

			}
		}

	}
	return counter;
}

int whiteLightCounter = 0;
int blackLightCounter = 0;
bool lastState = false;
bool HSVColorBuoyDetector::getWhiteLightState(IplImage *img, feature::WhiteLightSettings settings){


    IplImage* copy = cvCreateImage(cvGetSize(img), 8, 3);
	cvCopy(img, copy);

	//Split Image to single HSV planes
	cvCvtColor(copy, copy, CV_BGR2HSV); // Image to HSV
	//IplImage* h_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	//IplImage* v_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	IplImage* dest = cvCreateImage(cvGetSize(copy), 8, 1);
	cvCvtPixToPlane(copy, 0, s_plane, 0, 0);

   // cvThreshold(v_plane, v_plane, settings.val_Binary_Threshold, 255, CV_THRESH_BINARY);
    cvThreshold(s_plane, s_plane, settings.sat_Binary_Threshold, 255, CV_THRESH_BINARY);
	//only for initialization

    cvSmooth(s_plane, s_plane, CV_MEDIAN, 5, 5);
    int counter =combineAndCount(s_plane,0,dest);

    //Debug-GUI
    //debug_image = cvCreateImage(cvGetSize(getSplane()), 8, 1);
//	cvResize(s_plane, debug_image);

	//cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	//cvReleaseImage(&v_plane);
	cvReleaseImage(&dest);
	cvReleaseImage(&copy);

if(counter>20){
    whiteLightCounter++;
    blackLightCounter=0;

    }else
    {
        blackLightCounter++;
        whiteLightCounter=0;
    }


    if(whiteLightCounter>=8){
        lastState =true;
    }
    if(blackLightCounter>=8 ){
        lastState =false;
    }
        return lastState;
    }


// Ports:
IplImage* HSVColorBuoyDetector::getHplane(){
	return h_plane;		//wichtig
}
IplImage* HSVColorBuoyDetector::getSplane(){
	return s_plane;			//wichtig
}
IplImage* HSVColorBuoyDetector::getVplane(){
	return v_plane;			//wichtig
}
IplImage* HSVColorBuoyDetector::getHSVDebug(){
	return hsv_gray_debug;			//wichtig
}
IplImage* HSVColorBuoyDetector::getHoughDebug(){
	return hough_debug;			//wichtig
}
IplImage* HSVColorBuoyDetector::getDebugImage(){
	return debug_image;
}
/////////////////////
} // namespace avalon

