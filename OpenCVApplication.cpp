// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


void maskRedEyes(Mat img, Mat masked)
{
	float r, g, b, minimum, maximum, C;
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++) {
			unsigned char B, G, R;
			Vec3b pixel = img.at< Vec3b>(i, j);
			float H, S, V;
			B = pixel[0];
			G = pixel[1];
			R = pixel[2];

			r = (float)R / 255;
			b = (float)B / 255;
			g = (float)G / 255;

			minimum = min(r, min(g, b));
			maximum = max(r, max(g, b));

			C = maximum - minimum;
			V = maximum;

			if (V != 0)
				S = C / V;
			else
				S = 0;

			if (C != 0) {
				if (maximum == r) {
					H = 60 * (g - b) / C;
				}
				if (maximum == g) {
					H = 120 + 60 * (b - r) / C;
				}
				if (maximum == b) {
					H = 240 + 60 * (r - g) / C;
				}
			}
			else {
				H = 0;
			}
			if (H < 0) {
				H = H + 360;
			}


			if (((H >= 320 && H <= 360) || (H >= 0 && H <= 5)) && R > (B + G))
			{
				masked.at<uchar>(i, j) = 255; 
			}
			else masked.at<uchar>(i, j) = 0; 
		}
}

Mat fixRedEyes(Mat masked, Mat img)
{
	Mat dst = img.clone();

	for (int i = 0; i < masked.rows; i++)
	{
		for (int j = 0; j < masked.cols; j++)
		{
			Vec3b pixel = img.at<Vec3b>(i, j);

			if (masked.at<uchar>(i, j) == 255)
			{
				float blue = pixel[0];
				float green = pixel[1];
				float mean = (blue + green) / 2;

				dst.at<Vec3b>(i, j) = Vec3b(mean, mean, mean);
			}
			else
			{
				dst.at<Vec3b>(i, j) = pixel;
			}
		}
	}

	return dst;
}
int main()
{

	char fname[MAX_PATH];
	Mat img;
	if (openFileDlg(fname))
	{
		
		img = imread(fname, IMREAD_COLOR);
		
	}

	imshow("Initial image", img);

	// Load HAAR cascade
	cv::CascadeClassifier eyes_cascade;

	// A vector of Rect for storing bounding boxes for eyes.
	std::vector<Rect> eyes;
	if (!eyes_cascade.load("C:/Users/Alina Mihut/Desktop/haarcascade_eye.xml")){
		printf("Error loading\n");
		return -1;
		}
	

	// Detect eyes
	eyes_cascade.detectMultiScale(img,eyes, 1.3, 4, 0 | CASCADE_SCALE_IMAGE, Size(100, 100));
	

	Mat redMask(img.rows, img.cols, CV_8UC1);
	maskRedEyes(img, redMask);
	imshow("Red Mask Image", redMask);

	for (size_t i = 0; i < eyes.size(); i++)
	{

			// Extract eye from the image.
			Mat eye = img(eyes[i]);
			//imshow("eye", eye);
			Mat interm = eye.clone();
			maskRedEyes(eye, interm);
			Mat fixedEye = fixRedEyes(interm, eye);
			//imshow("image2", fixedEye);
			
	}

	Mat dst = fixRedEyes(redMask, img);
	
	imshow("Fixed image", dst);

	waitKey();
	return 0;
}