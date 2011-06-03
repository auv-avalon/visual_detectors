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

    CvSeq* circles = cvHoughCircles(dil, storage, CV_HOUGH_GRADIENT, 2, imgAsHSV->width / 3, configEdgeThreshold, configHoughThreshold);

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

void HSVColorBuoyDetector::configureLowHue(int low)
{
    if(0 <= low && low <= 255)
        configLowHue = low;
}

void HSVColorBuoyDetector::configureHighHue(int high)
{
    if(0 <= high && high <= 255)
        configHighHue = high;
}


} // namespace avalon

