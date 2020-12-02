#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColorTransformer.h"
#include "Converter.h"

using namespace cv;
using namespace std;

void ShowImages(Mat src, Mat dest) {
	namedWindow("Source Image");
	imshow("Source Image", src);
	namedWindow("Destination Image");
	imshow("Destination Image", dest);
	while (waitKey(0) != 27);
}

int main(int argc, char** argv)
{
	//<ProgramName.exe> <Command> <InputPath> <CommandArguments>
	string command = string(argv[1]);
	string path = string(argv[2]);
	Mat source = imread(path, cv::IMREAD_COLOR);
	Mat dest;
	int arg_used = 3;
	if (command.find("rgb2hsv") != -1)
	{
		if (Converter::RGB2HSV(source, dest)) {
  		ShowImages(source, dest);
		} else cout << "Conversion failed";
	}
	else if (command.find("hsv2rgb") != -1)
	{
		if (Converter::HSV2RGB(source, dest)) {
  		ShowImages(source, dest);
		} else cout << "Conversion failed";
	}
	else if (command.find("rgb2gray") != -1)
	{
		if (!Converter::RGB2GrayScale(source, dest)) {
  		ShowImages(source, dest);
		} else cout << "Conversion failed";
	}
	else if (command.find("gray2rgb") != -1)
	{
		if (!Converter::GrayScale2RGB(source, dest)) {
  		ShowImages(source, dest);
		} else cout << "Conversion failed";
	}
	else if (command.find("compare") != -1) 
	{
		string DesPath = string(argv[3]);
		dest = imread(DesPath, cv::IMREAD_COLOR);
		cout << "Compare result: " << ColorTransformer::CompareImage(source, dest);
	}
	else if (command.find("drawhist") != -1)
	{
  	ColorTransformer::DrawHist(source, dest);
  	ShowImages(source, dest);
	}
	else if (command.find("brightness") != -1)
	{
  	ColorTransformer::ChangeBrightness(source, dest, atof(argv[arg_used++]));
  	ShowImages(source, dest);
	}
	else if (command.find("contrast") != -1)
	{
  	ColorTransformer::ChangeContrast(source, dest, atof(argv[arg_used++]));
  	ShowImages(source, dest);
	}
	else if (command.find("equalhist") != -1)
	{
  	ColorTransformer::HistogramEqualization(source, dest);
  	ShowImages(source, dest);
	}
	else if (command.find("hist") != -1)
	{
  	auto hist = ColorTransformer::Hist(source);
  	for(int c = 0; c < hist.size(); c++) {
    	cout << "Channel " << c << ':' << endl;
    	for(int v = 0; v < 256; v++) {
      	cout << hist[c][v] << ' ';
    	}
    	cout << endl << endl;
  	}
	}
	if (argc > arg_used) imwrite(argv[arg_used++], dest);
	return 0;
}
