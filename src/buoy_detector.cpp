#include "buoy_detector.h"
#include <stdio.h>
#include "opencv/highgui.h"
#include <Eigen/Core>
namespace avalon {

// ---------------------------------------------------------------------------------------

// module internal constant values
const unsigned char cCTHue[] = { 0, 0 };
const unsigned char cCTSat[] = { 255, 0 };
const unsigned char cCTVal[] = { 255, 0 };
const CvScalar circleColor = cvScalar(0, 255, 0);

// ---------------------------------------------------------------------------------------

HSVColorBuoyDetector::HSVColorBuoyDetector() :
	satMax(0), valMax(0), configLowHue(56), configHighHue(200),
			configHoughThreshold(100), configEdgeThreshold(200) {
}

HSVColorBuoyDetector::~HSVColorBuoyDetector() {
}


// ---------------------------------------------------------------------------------------

void HSVColorBuoyDetector::configureLowHue(int low) {
	if (0 <= low && low <= 255)
		configLowHue = low;
}

void HSVColorBuoyDetector::configureHighHue(int high) {
	if (0 <= high && high <= 255)
		configHighHue = high;
}


std::vector<feature::Buoy> HSVColorBuoyDetector::detect(IplImage* frame,
		double factor) {
	//	// Vector for all buoys
	std::vector < feature::Buoy > result;

	IplImage* dil = cvCreateImage(cvGetSize(frame), 8, 1);

	cvDilate(frame, dil, NULL, 2);

	//	//Über das Image für die Sättigung werden (nachdem es nocheinmal übern Gauss-Filter geglättet wurde)
	//	//die Kreise ermittelt und in das HSV-Image eingezeichnet.
	CvMemStorage* storage = cvCreateMemStorage(0);
	//
	cvSmooth(dil, dil, CV_GAUSSIAN, 21, 21);
	//cvSmooth(s_plane, s_plane, CV_GAUSSIAN, 13, 13);

	CvSeq* circles = cvHoughCircles(dil, storage, CV_HOUGH_GRADIENT, 2,
			frame->width / 3, configEdgeThreshold, configHoughThreshold);

	//	cvReleaseImage(&s_plane);
	cvReleaseImage(&dil);

	for (int i = 0; i < circles->total; i++) {
		float* circle = (float*) cvGetSeqElem(circles, i);

		int x = (int) (circle[0] / factor);
		int y = (int) (circle[1] / factor);
		int r = (int) (circle[2] / factor);

		feature::Buoy data(x, y, r);

		result.push_back(data);
	}

	//	cvReleaseImage(&imgAsHSV);

	return result;
}
int found =0;
int notFound =0;

std::vector<feature::Buoy> HSVColorBuoyDetector::detect(IplImage* frame,
		IplImage* h_plane, IplImage* s_plane, double factor) {
	//	// Vector for all buoys
	std::vector < feature::Buoy > result;

	IplImage* dil = cvCreateImage(cvGetSize(frame), 8, 1);

	cvDilate(frame, dil, NULL, 2);
//	cvCopy(frame, dil);
//
//	int cannyVal = 250;
//	int gaussVal = 2;
//	int erodeVal = 3;
//
//	cv::Mat out;

//	cv::erode(dil, out, cv::Mat(), cv::Point(-1, -1), erodeVal);
/*
	cv::Canny(out, out, cannyVal / 2, cannyVal, 3 );

	cv::GaussianBlur( out, out, cv::Size(gaussVal * 2 - 1, gaussVal * 2 - 1), 2, 2 );
*/


	cvSmooth(dil, dil, CV_GAUSSIAN, 21, 21);

	cv::imshow("or", dil);
	std::vector < cv::Vec3f > circles;

    	cv::HoughCircles(dil, circles, CV_HOUGH_GRADIENT, 2, dil->width / 4,
			configEdgeThreshold, configHoughThreshold, 30, 150);

	cvReleaseImage(&dil);
//	bool wasFound=false;
	for (int i = 0; i < circles.size(); i++) {
		int x = cvRound(circles[i][0] / factor);
		int y = cvRound(circles[i][1] / factor);
		int r = cvRound(circles[i][2] / factor);

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


		if (counter/24.0 >0.8) {


				feature::Buoy data(x, y, r);

				result.push_back(data);
//				wasFound=true;

		}


	}
//	if(wasFound){
//		found++;
//	}
//	else
//	{
//		notFound++;
//	}
//	std::cout << "Found: " << found <<std::endl;
//	std::cout << "Not found: " << notFound <<std::endl;
	return result;
}




void HSVColorBuoyDetector::shadingRGB(IplImage* src, IplImage* dest) {


	int height = src->height;
	int width = src->width;
	int rowSize = src->widthStep;
	char *pixelStart = src->imageData;



	Eigen::MatrixXf a0R(height,width);
	Eigen::MatrixXf a0G(height,width);
	Eigen::MatrixXf a0B(height,width);

	Eigen::MatrixXf a1R(height,width);
	Eigen::MatrixXf a1G(height,width);
	Eigen::MatrixXf a1B(height,width);

	Eigen::MatrixXf slopeR(2,width);
	Eigen::MatrixXf slopeG(2,width);
	Eigen::MatrixXf slopeB(2,width);

	Eigen::MatrixXf y0R(2,width);
	Eigen::MatrixXf y0G(2,width);
	Eigen::MatrixXf y0B(2,width);


	float  mean[3],
			sum[3], sxy[3], sx2, mid, factor[3];
	int a1[3][height][width], min[3], max[3];

	// grab the rgb values

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			a0R(y,x) = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0);
			a0G(y,x) = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1);
			a0B(y,x) = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2);

//			a0[0][y][x] = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0);
//			a0[1][y][x] = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1);
//			a0[2][y][x] = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2);
		}
	}

	//regression 1: compute the regression line of any column
	mid = height / (float) 2;
	for (int x = 0; x < width; x++) {
		for (int z = 0; z < 3; z++) {
			sum[z] = 0;
		}
		for (int y = 0; y < height; y++) {
//			for (int z = 0; z < 3; z++) {
//				sum[z] += (int) a0[z][y][x];
//			}
			sum[0] += (int) a0R(y,x);
			sum[1] += (int) a0G(y,x);
			sum[2] += (int) a0B(y,x);
		}
		for (int z = 0; z < 3; z++) {
			mean[z] = sum[z] / height;
			sxy[z] = 0;
		}
		sx2 = 0;
		for (int y = 0; y < height; y++) {
//			for (int z = 0; z < 3; z++) {
//				sxy[z] += (y - mid) * a0[z][y][x] - mean[z];
//			}
			sxy[0] += (y - mid) * a0R(y,x) - mean[0];
			sxy[1] += (y - mid) * a0G(y,x) - mean[1];
			sxy[2] += (y - mid) * a0B(y,x) - mean[2];

			sx2 += (y - mid) * (y - mid);
		}
//		for (int z = 0; z < 3; z++) {
//			slope[z][1][x] = sxy[z] / sx2;
//			y0[z][1][x] = mean[z] - slope[z][1][x] * mid;
			slopeR(1,x)=sxy[0] / sx2;
			slopeG(1,x)=sxy[1] / sx2;
			slopeB(1,x)=sxy[2] / sx2;
			y0R(1,x)= mean[0] - slopeR(1,x) * mid;
			y0G(1,x)= mean[1] - slopeG(1,x) * mid;
			y0B(1,x)= mean[2] - slopeB(1,x) * mid;
//		}
	}

	//regression 2: compute the regression line of any row
	mid = width / (float) 2;
	for (int y = 0; y < height; y++) {
		for (int z = 0; z < 3; z++) {
			sum[z] = 0;
		}
		for (int x = 0; x < width; x++) {
//			for (int z = 0; z < 3; z++) {
//				sum[z] += slope[z][1][x] * y + y0[z][1][x];
//			}
			sum[0]+=slopeR(1,x)*y+y0R(1,x);
			sum[1]+=slopeG(1,x)*y+y0G(1,x);
			sum[2]+=slopeB(1,x)*y+y0B(1,x);
		}
		for (int z = 0; z < 3; z++) {
			mean[z] = sum[z] / width;
			sxy[z] = 0;
		}
		sx2 = 0;
		for (int x = 0; x < width; x++) {
//			for (int z = 0; z < 3; z++) {
//				sxy[z] += (x - mid) * (slope[z][1][x] * y + y0[z][1][x]
//						- mean[z]);
//			}
			sxy[0] += (x - mid) * (slopeR(1,x) * y + y0R(1,x)
									- mean[0]);
			sxy[1] += (x - mid) * (slopeG(1,x) * y + y0G(1,x)
									- mean[1]);
			sxy[2] += (x - mid) * (slopeB(1,x) * y + y0B(1,x)
									- mean[2]);
			sx2 += (x - mid) * (x - mid);
		}
//		for (int z = 0; z < 3; z++) {
//			slope[z][0][y] = sxy[z] / sx2;
//			y0[z][0][y] = mean[z] - slope[z][0][y] * mid;
//		}
		slopeR(0,y)=sxy[0] / sx2;
		slopeG(0,y)=sxy[1] / sx2;
		slopeB(0,y)=sxy[2] / sx2;

		y0R(0,y)=mean[0] - slopeR(0,y) * mid;
		y0G(0,y)=mean[1] - slopeG(0,y) * mid;
		y0B(0,y)=mean[2] - slopeB(0,y) * mid;
	}

	//shading correction: subtract the flat background image from the
	//original and rearrange the resulting RGB values between 0 to 255
	for (int z = 0; z < 3; z++) {
		min[z] = 255;
		max[z] = 0;
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
//			for (int z = 0; z < 3; z++) {
//				a1[z][y][x] = (int) (a0[z][y][x] - (slope[z][0][y] * x
//						+ y0[z][0][y]));
//				min[z] = (a1[z][y][x] < min[z]) ? a1[z][y][x] : min[z];
//				max[z] = (a1[z][y][x] > max[z]) ? a1[z][y][x] : max[z];
//			}
			a1R(y,x) = (int) (a0R(y,x) - (slopeR(0,y) * x+ y0R(0,y)));
			a1G(y,x) = (int) (a0G(y,x) - (slopeG(0,y) * x+ y0G(0,y)));
			a1B(y,x) = (int) (a0B(y,x) - (slopeB(0,y) * x+ y0B(0,y)));

			min[0] = (a1R(y,x) < min[0]) ? a1R(y,x) : min[0];
			min[1] = (a1G(y,x) < min[1]) ? a1G(y,x) : min[1];
			min[2] = (a1B(y,x) < min[2]) ? a1B(y,x) : min[2];

			max[0] = (a1R(y,x) > max[0]) ? a1R(y,x) : max[0];
			max[1] = (a1G(y,x) > max[1]) ? a1G(y,x) : max[1];
			max[2] = (a1B(y,x) > max[2]) ? a1B(y,x) : max[2];
		}
	}
	for (int z = 0; z < 3; z++) {
		factor[z] = 255 / (float) (max[z] - min[z]);
	}
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
//			for (int z = 0; z < 3; z++) {
//				a0[z][y][x] = (int) ((a1[z][y][x] - min[z]) * factor[z]);
//			}
			a0R(y,x) =(int) ( (a1R(y,x)-min[0])*factor[0]);
			a0G(y,x) =(int) ( (a1G(y,x)-min[1])*factor[1]);
			a0B(y,x) =(int) ( (a1B(y,x)-min[2])*factor[2]);
		}
	}
	rowSize = dest->widthStep;
	pixelStart = dest->imageData;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0) = a0R(y,x);
			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1) = a0G(y,x);
			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2) = a0B(y,x);
		}
	}
}


int HSVColorBuoyDetector::merge(IplImage* dest, IplImage* src1, IplImage* scr2,
		int th1, int th2, int steps, bool negativColor1, bool negativColor2,
		bool rekursion, int past, bool testMode) {
	int negColor1 = negativColor1 ? 255 : 0;
	int posColor1 = negativColor1 ? 0 : 255;
	int negColor2 = negativColor2 ? 255 : 0;
	int posColor2 = negativColor2 ? 0 : 255;
	int height = dest->height;
	int width = dest->width;
	float counter1 = 0;
	float counter2 = 0;
	float counter3 = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int v = ((uchar *) (src1->imageData + y * src1->widthStep))[x];

			int chosenValue = 0;
			if (v >= th1) {
				((uchar *) (dest->imageData + y * dest->widthStep))[x]
						= chosenValue = negColor1;

			} else {
				((uchar *) (dest->imageData + y * dest->widthStep))[x]
						= chosenValue = posColor1;
			}

			v = ((uchar *) (scr2->imageData + y * scr2->widthStep))[x];

			if (v >= th2) {

				if (chosenValue == 0) {
					if (negColor2 == 0) {
						counter2++; //src1 hat schon schwarz gefärbt, src2 würde eigentlich auch schwarz färben
					}
					counter1++;
				} else {
					if (negColor2 == 0) {
						((uchar *) (dest->imageData + y * dest->widthStep))[x]
								= negColor2;
						counter3++; //src1 hat weiß gefärbt, src2 färbt jetzt schwarz
					}
				}

			} else {

				if (chosenValue == 0) {
					if (posColor2 == 0) {
						counter2++;//src1 hat schon schwarz gefärbt, src2 würde eigentlich auch schwarz färben
					}
					counter1++;
				} else {
					if (posColor2 == 0) {
						((uchar *) (dest->imageData + y * dest->widthStep))[x]
								= posColor2;
						counter3++;//src1 hat weiß gefärbt, src2 färbt jetzt schwarz
					}
				}
			}

		}
	}

	if (rekursion) {
		if (counter1 < 10 && (th1 + steps) <= 255) {
			return merge(dest, src1, scr2, (th1 + steps), th2, steps,
					negativColor1, negativColor2, true, past, testMode);

		} else {

			if (counter2 + counter3 <= ((height) * width)
					/ (double) 10 && (past == 0 || (past * 250) / (double) 100
					> counter2 + counter3)) {
				if ((counter2 / counter1) < 0.25 && (th2 + steps) <= 255) {
					return merge(dest, src1, scr2, th1, (th2 + steps), steps,
							negativColor1, negativColor2, true, past, testMode);
				}
			} else {
				if ((th2 - steps) >= 0) {
					return merge(dest, src1, scr2, th1, (th2 - (steps)), steps,
							negativColor1, negativColor2, false, past, testMode);
				}
			}

		}
	}
	if (testMode) {
		IplImage* v_plane = cvCreateImage(cvGetSize(scr2), 8, 1);
		cvThreshold(scr2,v_plane,  th2, 255, CV_THRESH_BINARY);
		cvShowImage("V binary", v_plane);
		cvShowImage("V", scr2);
	}
	return counter2 + counter3;
}


std::vector<feature::Buoy> HSVColorBuoyDetector::detectBuoy(IplImage* img,
		int height, int h_threshold,int s_threshold, int v_threshold, int steps,
		int pastAverageDark, bool testMode) {

		IplImage* copy = cvCreateImage(cvGetSize(img), 8, 3);
		cvCopy(img, copy);
		double factor = 1;//height / (double) (img->height); // The resize factor

		//Split Image to single HSV planes
		cvCvtColor(copy, copy, CV_BGR2HSV); // Image to HSV
		IplImage* h_plane = cvCreateImage(cvGetSize(copy), 8, 1);
		IplImage* s_plane = cvCreateImage(cvGetSize(copy), 8, 1);
		IplImage* v_plane = cvCreateImage(cvGetSize(copy), 8, 1);
		cvCvtPixToPlane(copy, h_plane, s_plane, v_plane, 0);

		//Shading correction
		cvCvtColor(copy, copy, CV_HSV2RGB);
		shadingRGB(copy, copy);

		//Split shaded images to single HSV planes
		cvCvtColor(copy, copy, CV_RGB2HSV);
		IplImage* h_shaded = cvCreateImage(cvGetSize(copy), 8, 1);
		IplImage* s_shaded = cvCreateImage(cvGetSize(copy), 8, 1);
		IplImage* v_shaded = cvCreateImage(cvGetSize(copy), 8, 1);
		cvCvtPixToPlane(copy, h_shaded, s_shaded, v_shaded, 0);

		//Get the buoy
		IplImage* or_plane = cvCreateImage(cvGetSize(copy), 8, 1);
		merge(or_plane, h_plane,v_shaded, h_threshold, v_threshold, steps, false,
				true, true, pastAverageDark, testMode);



		//detect buoys
		std::vector < feature::Buoy > result = detect(or_plane, factor);

		//get binary images
		cvThreshold(h_plane, h_plane, h_threshold, 255, CV_THRESH_BINARY);
		cvThreshold(s_plane, s_plane, s_threshold, 255, CV_THRESH_BINARY);


		cvThreshold(h_shaded, h_shaded, h_threshold, 255, CV_THRESH_BINARY);
		cvThreshold(s_shaded, s_shaded, s_threshold, 255, CV_THRESH_BINARY);

		//Show images
		cvShowImage("H binary (shaded)", h_shaded);
		cvShowImage("S binary (shaded)", s_shaded);
		cvShowImage("H binary", h_plane);
		cvShowImage("S binary", s_plane);
		cvShowImage("Result", or_plane);


		//Release images
		cvReleaseImage(&h_plane);
		cvReleaseImage(&s_plane);
		cvReleaseImage(&v_plane);
		cvReleaseImage(&h_shaded);
		cvReleaseImage(&s_shaded);
		cvReleaseImage(&v_shaded);
		cvReleaseImage(&or_plane);

		return result;

	//		return null;
}

////////SAUC-E/////////////
std::vector<feature::Buoy> HSVColorBuoyDetector::buoyDetection(IplImage* img,
		int height, double h_threshold, double s_threshold) {

	IplImage* copy = cvCreateImage(cvGetSize(img), 8, 3);
	cvCopy(img, copy);
	//	IplImage* copy = getCopy(img, height);// Resize the image to a specific height
	double factor = 1;//height / (double) (img->height); // The resize factor

	//Split Image to single HSV planes
	cvCvtColor(copy, copy, CV_BGR2HSV); // Image to HSV
	IplImage* h_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	IplImage* v_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	cvCvtPixToPlane(copy, h_plane, s_plane, v_plane, 0);

	//Shading correction
	cvCvtColor(copy, copy, CV_HSV2RGB);
	shadingRGB(copy, copy);

	//Split shaded images to single HSV planes
	cvCvtColor(copy, copy, CV_RGB2HSV);
	IplImage* h_shaded = cvCreateImage(cvGetSize(copy), 8, 1);
	IplImage* s_shaded = cvCreateImage(cvGetSize(copy), 8, 1);
	IplImage* v_shaded = cvCreateImage(cvGetSize(copy), 8, 1);
	cvCvtPixToPlane(copy, h_shaded, s_shaded, v_shaded, 0);

	//create binary images
	cvThreshold(h_shaded, h_shaded, h_threshold, 255, CV_THRESH_BINARY);
	cvThreshold(s_plane, s_plane, s_threshold, 255, CV_THRESH_BINARY);

	//"OR" images
	IplImage* or_plane = cvCreateImage(cvGetSize(copy), 8, 1);
	cvOr(h_shaded, s_plane, or_plane);

	//smooth images
	cvSmooth(or_plane, or_plane, CV_MEDIAN, 5, 5);


	//detect buoys
	std::vector < feature::Buoy > result = detect(or_plane, h_shaded, s_plane,
			factor);

//	cv::Mat out;
//	cv::Canny(or_plane, out, (int) configEdgeThreshold / 4,
//			(int) configEdgeThreshold, 3);
//	cv::imshow("cannyOut", out);

	//Show images
	cvShowImage("H binary (shaded)", h_shaded);
	cvShowImage("S binary", s_plane);


	//Release images
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvReleaseImage(&h_shaded);
	cvReleaseImage(&s_shaded);
	cvReleaseImage(&v_shaded);
	cvReleaseImage(&or_plane);

	return result;
}

/////////////////////
} // namespace avalon

