#include "buoy_detector.h"
#include <stdio.h>
#include "opencv/highgui.h"

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


std::vector<feature::Buoy> HSVColorBuoyDetector::detect2(IplImage* frame,
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
	CvMemStorage* storage = cvCreateMemStorage(0);

	cvSmooth(dil, dil, CV_GAUSSIAN, 21, 21);
//	cvSmooth(s_plane, s_plane, CV_GAUSSIAN, 13, 13);
	cv::imshow("or", dil);
	std::vector < cv::Vec3f > circles;

    	cv::HoughCircles(dil, circles, CV_HOUGH_GRADIENT, 2, dil->width / 4,
			configEdgeThreshold, configHoughThreshold, 30, 150);

	//	CvSeq* circles = cvHoughCircles(dil, storage, CV_HOUGH_GRADIENT, 2,
	//			frame->width / 3, configEdgeThreshold, configHoughThreshold);

	//	cvReleaseImage(&s_plane);
	cvReleaseImage(&dil);
	//	std::cout << "Circles: "<< circles.size() << std::endl;
	for (int i = 0; i < circles.size(); i++) {
		//		float* circle = (float*) cvGetSeqElem(circles, i);
		int x = cvRound(circles[i][0] / factor);
		int y = cvRound(circles[i][1] / factor);
		int r = cvRound(circles[i][2] / factor);
		/*
		 int x = (int) (circle[0] / factor);
		 int y = (int) (circle[1] / factor);
		 int r = (int) (circle[2] / factor);
		 */
		int h_value =
				((uchar *) (h_plane->imageData + y * h_plane->widthStep))[x];

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

		}
	}

	//	cvReleaseImage(&imgAsHSV);

	return result;
}

inline IplImage* HSVColorBuoyDetector::getChannel(Channel channel,
		IplImage* hsvframe) {
	IplImage* h_plane = cvCreateImage(cvGetSize(hsvframe), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize(hsvframe), 8, 1);
	IplImage* v_plane = cvCreateImage(cvGetSize(hsvframe), 8, 1);

	IplImage* output;

	cvCvtPixToPlane(hsvframe, h_plane, s_plane, v_plane, 0);

	switch (channel) {
	case HUE:
		output = h_plane;
		cvReleaseImage(&s_plane);
		cvReleaseImage(&v_plane);
		break;

	case SATURATION:
		output = s_plane;
		cvReleaseImage(&h_plane);
		cvReleaseImage(&v_plane);
		break;

	case VALUE:
		output = v_plane;
		cvReleaseImage(&h_plane);
		cvReleaseImage(&s_plane);
		break;
	}

	return output;
}

IplImage* HSVColorBuoyDetector::filterHueChannel(IplImage* imgAsHSV) {
	uchar *p, *pEnd, *pLine;

	pLine = (uchar*) imgAsHSV->imageData;

	for (int y = 0; y < imgAsHSV->height; y++) {
		for (p = pLine, pEnd = p + (imgAsHSV->width * imgAsHSV->nChannels); p
				< pEnd; p += imgAsHSV->nChannels) {

			uchar H = *(p + 0); // Hue
			uchar S = *(p + 1); // Saturation
			uchar V = *(p + 2); // Value

			int color = filterByHue(H, S, V);

			*(p + 0) = cCTHue[color];
			*(p + 1) = cCTSat[color];
			*(p + 2) = cCTVal[color];

		}
		pLine += imgAsHSV->widthStep / sizeof(uchar);
	}

	return imgAsHSV;
}

// Wenn der Farbton nicht einen derer der Boje entspricht, wird enum schwarz ausgegeben.
// Ansonsten -1. Weiter wird überprüft, ob dieser Pixel jener mit dem höchsten Sättigungs-Wert ist.
// Wenn ja, so wird dieser als neuer höchster Wert gesetzt.
int HSVColorBuoyDetector::filterByHue(int H, int S, int V) {
	if (H <= configLowHue || H >= configHighHue) {
		satMax = (satMax < S) ? S : satMax;
		valMax = (valMax < S) ? S : valMax;
		return cRED;
	} else {
		return cBLACK;
	}
}

//Jene Pixel, deren Sättigungswerte unter einem bestimmten Prozentsatz des zuvor ermittelten
//höchsten Sättigungswert liegen, werden  schwarz eingefärbt. Alle Anderen rot.
int HSVColorBuoyDetector::filterBySaturation(int H, int S, int V) {
	if ((S > (255 / (double) 100) * 20)
	//||(S > (valMax / (double) 100) * 10)
	) {
		//return cRED;
		return -1;

	} else {
		return cBLACK;
	}
}

void HSVColorBuoyDetector::configureLowHue(int low) {
	if (0 <= low && low <= 255)
		configLowHue = low;
}

void HSVColorBuoyDetector::configureHighHue(int high) {
	if (0 <= high && high <= 255)
		configHighHue = high;
}

void HSVColorBuoyDetector::shadingRGB(IplImage* src, IplImage* dest) {

	int height = src->height;
	int width = src->width;
	int rowSize = src->widthStep;
	char *pixelStart = src->imageData;

	float a0[3][height][width], slope[3][2][width], y0[3][2][width], mean[3],
			sum[3], sxy[3], sx2, mid, factor[3];
	int a1[3][height][width], min[3], max[3];

	// grab the rgb values

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			a0[0][y][x] = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0);
			a0[1][y][x] = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1);
			a0[2][y][x] = *(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2);
		}
	}

	//regression 1: compute the regression line of any column
	mid = height / (float) 2;
	for (int x = 0; x < width; x++) {
		for (int z = 0; z < 3; z++) {
			sum[z] = 0;
		}
		for (int y = 0; y < height; y++) {
			for (int z = 0; z < 3; z++) {
				sum[z] += (int) a0[z][y][x];
			}
		}
		for (int z = 0; z < 3; z++) {
			mean[z] = sum[z] / height;
			sxy[z] = 0;
		}
		sx2 = 0;
		for (int y = 0; y < height; y++) {
			for (int z = 0; z < 3; z++) {
				sxy[z] += (y - mid) * a0[z][y][x] - mean[z];
			}
			sx2 += (y - mid) * (y - mid);
		}
		for (int z = 0; z < 3; z++) {
			slope[z][1][x] = sxy[z] / sx2;
			y0[z][1][x] = mean[z] - slope[z][1][x] * mid;
		}
	}

	//regression 2: compute the regression line of any row
	mid = width / (float) 2;
	for (int y = 0; y < height; y++) {
		for (int z = 0; z < 3; z++) {
			sum[z] = 0;
		}
		for (int x = 0; x < width; x++) {
			for (int z = 0; z < 3; z++) {
				sum[z] += slope[z][1][x] * y + y0[z][1][x];
			}
		}
		for (int z = 0; z < 3; z++) {
			mean[z] = sum[z] / width;
			sxy[z] = 0;
		}
		sx2 = 0;
		for (int x = 0; x < width; x++) {
			for (int z = 0; z < 3; z++) {
				sxy[z] += (x - mid) * (slope[z][1][x] * y + y0[z][1][x]
						- mean[z]);
			}
			sx2 += (x - mid) * (x - mid);
		}
		for (int z = 0; z < 3; z++) {
			slope[z][0][y] = sxy[z] / sx2;
			y0[z][0][y] = mean[z] - slope[z][0][y] * mid;
		}
	}

	//shading correction: subtract the flat background image from the
	//original and rearrange the resulting RGB values between 0 to 255
	for (int z = 0; z < 3; z++) {
		min[z] = 255;
		max[z] = 0;
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int z = 0; z < 3; z++) {
				a1[z][y][x] = (int) (a0[z][y][x] - (slope[z][0][y] * x
						+ y0[z][0][y]));
				min[z] = (a1[z][y][x] < min[z]) ? a1[z][y][x] : min[z];
				max[z] = (a1[z][y][x] > max[z]) ? a1[z][y][x] : max[z];
			}
		}
	}
	for (int z = 0; z < 3; z++) {
		factor[z] = 255 / (float) (max[z] - min[z]);
	}
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			for (int z = 0; z < 3; z++) {
				a0[z][y][x] = (int) ((a1[z][y][x] - min[z]) * factor[z]);
			}
		}
	}
	rowSize = dest->widthStep;
	pixelStart = dest->imageData;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 0) = a0[0][y][x];
			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 1) = a0[1][y][x];
			*(uchar *) (pixelStart + (y) * rowSize + (x) * 3 + 2) = a0[2][y][x];
		}
	}
}

IplImage* HSVColorBuoyDetector::createHistogram(IplImage* img, int maxRange) {
	int numBins = maxRange + 1;
	float range[] = { 0, maxRange };
	float *ranges[] = { range };
	IplImage* histImage = cvCreateImage(cvSize(320, 200), 8, 1);
	CvHistogram* hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
	cvCalcHist(&img, hist, 0, NULL);
	float histMax = 0;
	//grab the min and max values and their indeces
	cvGetMinMaxHistValue(hist, 0, &histMax, 0, 0);
	//scale the bin values so that they will fit in the image representation
	cvScale(hist->bins, hist->bins, ((double) histImage->height) / histMax, 0);

	//set all histogram values to 255
	cvSet(histImage, cvScalarAll(maxRange), 0);
	//create a factor for scaling along the width
	int hist_size = 255;
	int bin_w = cvRound((double) histImage->width / hist_size);

	float mean;
	for (int i = 0; i < hist_size; i++) {
		//draw the histogram data onto the histogram image
		cvRectangle(
				histImage,
				cvPoint(i * bin_w, histImage->height),
				cvPoint((i + 1) * bin_w,
						histImage->height - cvRound(cvGetReal1D(hist->bins, i))),
				cvScalarAll(0), -1, 8, 0);
		//get the value at the current histogram bucket
		float* bins = cvGetHistValue_1D(hist, i);
		//increment the mean value
		mean += bins[0];
	}
	//finish mean calculation
	mean /= hist_size;
	float variance;
	//go back through now that mean has been calculated in order to calculate variance
	for (int i = 0; i < hist_size; i++) {
		float* bins = cvGetHistValue_1D(hist, i);
		variance += pow((bins[0] - mean), 2);
	}

	return histImage;

}

void HSVColorBuoyDetector::merge(IplImage* dest, IplImage* next, int th,
		bool negativColor) {
	int negColor = negativColor ? 255 : 0;
	int posColor = negativColor ? 0 : 255;
	int height = dest->height;
	int width = dest->width;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			int v = ((uchar *) (next->imageData + y * next->widthStep))[x];

			if (v >= th) {
				((uchar *) (dest->imageData + y * dest->widthStep))[x]
						= negColor;
			} else {
				((uchar *) (dest->imageData + y * dest->widthStep))[x]
						= posColor;
			}
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
	bool found = false;
	int yToStart = 0;
	for (int y = 0; y < height; y++) {
		int pixInX = 0;

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
			//			if(!found&&chosenValue==0){
			//				pixInX++;
			//				found=pixInX>10?true:found;
			//				yToStart=y;
			//			}

			//			if (found) {
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
			//			}
		}
	}

	if (rekursion) {
		if (counter1 < 10 && (th1 + steps) <= 255) {
			return merge(dest, src1, scr2, (th1 + steps), th2, steps,
					negativColor1, negativColor2, true, past, testMode);

		} else {

			if (counter2 + counter3 <= ((height - yToStart) * width)
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
		std::cout << "Th2 ist " << th2 << std::endl;
		//std::cout << "Steps sind " << steps << std::endl;
		//		std::cout << "Übereinstimmungen: " << (counter2 / counter1)
		//	  				<< std::endl;
		IplImage* v_plane = cvCreateImage(cvGetSize(scr2), 8, 1);
		merge(v_plane, scr2, th2, true);
		cvShowImage("V binary", v_plane);
		cvShowImage("V", scr2);
	}
	return counter2 + counter3;
}

void HSVColorBuoyDetector::imageToSquares(IplImage* img, bool smooth,
		int squareSize, bool equalizeH, bool equalizeS, bool equalizeV,
		bool backToRGB) {
	IplImage* imgAsHSV = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	cvCvtColor(img, imgAsHSV, CV_RGB2HSV);
	if (smooth) {
		cvSmooth(imgAsHSV, imgAsHSV, CV_GAUSSIAN);
	}
	IplImage* h_plane = cvCreateImage(cvGetSize(imgAsHSV), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize(imgAsHSV), 8, 1);
	IplImage* v_plane = cvCreateImage(cvGetSize(imgAsHSV), 8, 1);

	cvCvtPixToPlane(imgAsHSV, h_plane, s_plane, v_plane, 0);
	if (equalizeH) {
		cvEqualizeHist(h_plane, h_plane);
	}
	if (equalizeS) {
		cvEqualizeHist(s_plane, s_plane);
	}
	if (equalizeV) {
		cvEqualizeHist(v_plane, v_plane);
	}
	int height = img->height;
	int width = img->width;
	int widthSteps = width / squareSize;
	int heightSteps = height / squareSize;

	int dif = squareSize * squareSize;
	int myArrayH[widthSteps][heightSteps];
	int myArrayS[widthSteps][heightSteps];
	int myArrayV[widthSteps][heightSteps];

	for (int i = 0; i < widthSteps; i++) {
		for (int k = 0; k < heightSteps; k++) {
			int sumH = 0;
			int sumS = 0;
			int sumV = 0;
			for (int x = i * squareSize; x < (i + 1) * squareSize; x++) {

				for (int y = k * squareSize; y < (k + 1) * squareSize; y++) {
					sumH += ((uchar *) (h_plane->imageData + y
							* h_plane->widthStep))[x];
					sumS += ((uchar *) (s_plane->imageData + y
							* s_plane->widthStep))[x];
					sumV += ((uchar *) (v_plane->imageData + y
							* v_plane->widthStep))[x];
				}
			}
			for (int x = i * squareSize; x < (i + 1) * squareSize; x++) {

				for (int y = k * squareSize; y < (k + 1) * squareSize; y++) {
					((uchar *) (h_plane->imageData + y * h_plane->widthStep))[x]
							= int(sumH / dif);
					((uchar *) (s_plane->imageData + y * s_plane->widthStep))[x]
							= int(sumS / dif);
					((uchar *) (v_plane->imageData + y * v_plane->widthStep))[x]
							= int(sumV / dif);
				}
			}
			myArrayH[i][k] = int(sumH / dif);
			myArrayS[i][k] = int(sumS / dif);
			myArrayV[i][k] = int(sumV / dif);

		}
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			*(uchar*) (img->imageData + (y) * img->widthStep + (x) * 3 + 0)
					= myArrayH[x / squareSize][y / squareSize];
			*(uchar*) (img->imageData + (y) * img->widthStep + (x) * 3 + 1)
					= myArrayS[x / squareSize][y / squareSize];
			*(uchar*) (img->imageData + (y) * img->widthStep + (x) * 3 + 2)
					= myArrayV[x / squareSize][y / squareSize];
		}
	}
	if (backToRGB) {
		cvCvtColor(img, img, CV_HSV2RGB);
	}
	cvReleaseImage(&imgAsHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
}

IplImage* HSVColorBuoyDetector::getCopy(IplImage* src, int newHeight) {
	int curHeight = src -> height;
	double factor = newHeight / (double) curHeight;
	IplImage *dst = cvCreateImage(
			cvSize((int) ((src->width) * factor),
					(int) ((src->height) * factor)), 8, 3);
	cvResize(src, dst, 1);
	return dst;
}

void HSVColorBuoyDetector::shadingGrey(IplImage* img, int threshold) {

}

std::vector<feature::Buoy> HSVColorBuoyDetector::detectBuoy(IplImage* img,
		int height, int mergeHValue, int mergeVValue, int steps,
		int pastAverageDark, bool testMode) {

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

		//Get the buoy
		IplImage* or_plane = cvCreateImage(cvGetSize(copy), 8, 1);
		merge(or_plane, h_plane,v_shaded, mergeHValue, mergeVValue, steps, false,
				true, true, pastAverageDark, testMode);
//		//create binary images
//		cvThreshold(h_shaded, h_shaded, h_threshold, 255, CV_THRESH_BINARY);
//		cvThreshold(s_plane, s_plane, s_threshold, 255, CV_THRESH_BINARY);

//		//"OR" images

//		cvOr(h_shaded, s_plane, or_plane);

//		//smooth images
//		cvSmooth(or_plane, or_plane, CV_MEDIAN, 5, 5);

		//detect buoys
		std::vector < feature::Buoy > result = detect2(or_plane, factor);

	//	cv::Mat out;
	//	cv::Canny(or_plane, out, (int) configEdgeThreshold / 4,
	//			(int) configEdgeThreshold, 3);
	//	cv::imshow("cannyOut", out);

		//Show images
		cvShowImage("H binary (shaded)", h_shaded);
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
//	cvSmooth(or_plane, or_plane, CV_MEDIAN, 5, 5);


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

