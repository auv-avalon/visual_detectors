#include "buoy_detector.h"
#include <stdio.h>

namespace avalon {

// ---------------------------------------------------------------------------------------

// module internal constant values
const unsigned char cCTHue[] = { 0, 0 };
const unsigned char cCTSat[] = { 255, 0 };
const unsigned char cCTVal[] = { 255, 0 };
const CvScalar circleColor = cvScalar(0, 255, 0);

// ---------------------------------------------------------------------------------------

HSVColorBuoyDetector::HSVColorBuoyDetector() : satMax(0), valMax(0), 
        configLowHue(56), configHighHue(200), configHoughThreshold(100),
        configEdgeThreshold(200)
{
}


HSVColorBuoyDetector::~HSVColorBuoyDetector() {
}

// ---------------------------------------------------------------------------------------

std::vector<feature::Buoy> HSVColorBuoyDetector::detect(IplImage* frame)
{
    // Vector for all buoys
    std::vector<feature::Buoy> result;
    //Original zum HSV-Image umwandeln und dieses glätten.,
    IplImage* imgAsHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
    cvCvtColor(frame, imgAsHSV, CV_RGB2HSV);
    cvSmooth(imgAsHSV, imgAsHSV, CV_GAUSSIAN);

    //Werte zum Durchlaufen und Bearbeiten jeden einzelnen Pixel im HSV-Image
    int height = imgAsHSV->height;
    int width = imgAsHSV->width;
    int rowSize = imgAsHSV->widthStep;
    char *pixelStart = imgAsHSV->imageData;

    //1.Runde:
    //Annahme: Alles, was nicht den Farbtönen der Boje entspricht, kann keine Boje sein.
    //Ziel: Jene Pixel, deren Farbtöne, die nicht denen der Boje entsprechen, schwarz einfärben.
    //Alle anderen behalten ihre Werte. Weiter wird jener Wert mit der höchsten Farbsättigung ermittelt.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            uchar H = *(uchar*) (pixelStart + y * rowSize + x * 3 + 0); // Hue
            uchar S = *(uchar*) (pixelStart + y * rowSize + x * 3 + 1); // Saturation
            uchar V = *(uchar*) (pixelStart + y * rowSize + x * 3 + 2); // Value (Brightness)

            int ctype = filterByHue(H, S, V);

            if (ctype == cBLACK) {
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 0)
                    = cCTHue[ctype];
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 1)
                    = cCTSat[ctype];
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 2)
                    = cCTVal[ctype];
            } else {
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 0) = H;
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 1) = S;
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 2) = V;
            }
        }
    }

    // 2. Runde:
    //Annahme: Boje ist besitzt von den übriggebliebenen "freien" Pixel die höchsten Farbsättigungswerte
    //Ziel: Jene Pixel, deren Sättigungswerte unter einem bestimmten Prozentsatz des zuvor ermittelten
    //höchsten Sättigungswert liegen, werden  schwarz eingefärbt.
    /*
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            uchar H = *(uchar*) (pixelStart + y * rowSize + x * 3 + 0); // Hue
            uchar S = *(uchar*) (pixelStart + y * rowSize + x * 3 + 1); // Saturation
            uchar V = *(uchar*) (pixelStart + y * rowSize + x * 3 + 2); // Value (Brightness)

            int ctype = filterBySaturation(H, S, V);

            if (ctype != -1) {
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 0)
                    = cCTHue[ctype];
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 1)
                    = cCTSat[ctype];
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 2)
                    = cCTVal[ctype];
            } else {
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 0) = H;
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 1) = S;
                *(uchar*) (pixelStart + (y) * rowSize + (x) * 3 + 2) = V;
            }
        }
    }*/

    //Der höchste Sättigungswert wird wieder auf 0 gestellt.
    satMax = 0;
    valMax = 0;

    //Jeder HSV-Kanal bekommt sein eigenes Image.
    IplImage* s_plane = getChannel(HUE, imgAsHSV);
    IplImage* dil = cvCreateImage(cvGetSize(s_plane), 8, 1);

    cvEqualizeHist(s_plane, s_plane);
    
    cvDilate(s_plane, dil, NULL, 2);

    //Über das Image für die Sättigung werden (nachdem es nocheinmal übern Gauss-Filter geglättet wurde)
    //die Kreise ermittelt und in das HSV-Image eingezeichnet.
    CvMemStorage* storage = cvCreateMemStorage(0);

    cvSmooth(dil, dil, CV_GAUSSIAN, 21, 21);
    //cvSmooth(s_plane, s_plane, CV_GAUSSIAN, 13, 13);

    CvSeq* circles = cvHoughCircles(dil, storage, CV_HOUGH_GRADIENT, 2, imgAsHSV->width / 1, configEdgeThreshold, configHoughThreshold);

    cvReleaseImage(&s_plane);
    cvReleaseImage(&dil);

    for(int i = 0; i < circles->total; i++) {
        float* circle = (float*) cvGetSeqElem(circles, i);

        int x = circle[0];
        int y = circle[1];
        int r = circle[2];

        feature::Buoy data(x, y, r);

        result.push_back(data);
    }

    cvReleaseImage(&imgAsHSV);

    return result;
}


inline IplImage* HSVColorBuoyDetector::getChannel(Channel channel, IplImage* hsvframe)
{
    IplImage* h_plane = cvCreateImage(cvGetSize(hsvframe), 8, 1);
    IplImage* s_plane = cvCreateImage(cvGetSize(hsvframe), 8, 1);
    IplImage* v_plane = cvCreateImage(cvGetSize(hsvframe), 8, 1);

    IplImage* output;

    cvCvtPixToPlane(hsvframe, h_plane, s_plane, v_plane, 0);

    switch(channel) {
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


IplImage* HSVColorBuoyDetector::filterHueChannel(IplImage* imgAsHSV)
{    
    uchar *p, *pEnd, *pLine;

    pLine = (uchar*) imgAsHSV->imageData;

    for (int y = 0; y < imgAsHSV->height; y++) {
        for (p = pLine, pEnd = p + (imgAsHSV->width * imgAsHSV->nChannels);
             p < pEnd;
             p += imgAsHSV->nChannels) 
        {

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
/*
base::AUVMotionCommand creatMotionCommand(const feature::Buoy& buoy)
{
    base::AUVMotionCommand command
}
*/
} // namespace avalon

