#pragma once

#include <iostream>
#include <vector>
#include <array>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

namespace Converter
{
	/*
	Hàm chuyển đổi không gian màu của ảnh từ RGB sang GrayScale
	sourceImage: ảnh input
	destinationImage: ảnh output	
	Hàm trả về
	0: nếu chuyển thành công
	1: nếu chuyển thất bại (không đọc được ảnh input,...)
	*/
	int RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage){
		// kiem tra input
		if(!sourceImage.data) {
			std::cout << "Error: the image wasn't correctly loaded." << std::endl;
			return 1;
		}
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC1); //Tạo ảnh kết quả từ ảnh gốc
		float red = 0.299;
		float green = 0.587;
		float blue = 0.114;
		// khởi tạo các biến thuộc sourceImage
		int nrow = sourceImage.rows; 
		int ncol = sourceImage.cols;
		uchar* pSource = (uchar*)sourceImage.data;
		int nChannel_S = sourceImage.step[1];
		int widthStep_S = sourceImage.step[0];
		// khởi tạo các biến thuộc destinationImage
		uchar* pDes = (uchar*)destinationImage.data;
		int nChannel_D = destinationImage.step[1];
		int widthStep_D = destinationImage.step[0];
		// Truy xuất đến các phần tử trong mảng
		for (int j = 0; j<nrow; j++, pSource += widthStep_S, pDes += widthStep_D) {
			//lấy con trỏ đầu mỗi dòng
			uchar* pRow = pSource;
			uchar* pRowDes = pDes;
			for (int i = 0; i<ncol; i++, pRow+=nChannel_S, pRowDes+=nChannel_D) {
				// xử lý trên mỗi pixel 
				//RGB[A] to Gray:Y←0.299⋅Red+0.587⋅Green+0.114⋅Blue
				pRowDes[0] = pRow[0]*blue + pRow[1]*green +pRow[2]*red ;
			}   
		}
		return 0;	
	}

		/*
		Hàm chuyển đổi không gian màu của ảnh từ GrayScale sang RGB
		sourceImage: ảnh input
		destinationImage: ảnh output
		Hàm trả về
		0: nếu chuyển thành công
		1: nếu chuyển thất bại (không đọc được ảnh input,...)
		*/
	int GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage){
		// kiem tra input
		if(!sourceImage.data) {
			std::cout << "Error: the image wasn't correctly loaded." << std::endl;
			return 1;
		}
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, CV_8UC3); //Tạo ảnh kết quả từ ảnh gốc
		float red = 0.299;
		float green = 0.587;
		float blue = 0.114;
		//khởi tạo các biến thuộc sourceImage
		int nrow = sourceImage.rows; 
		int ncol = sourceImage.cols;
		uchar* pSource = (uchar*)sourceImage.data;
		int nChannel_S = sourceImage.step[1];
		int widthStep_S = sourceImage.step[0];
		// khởi tạo các biến thuộc  destinationImage
		uchar* pDes = (uchar*)destinationImage.data;
		int nChannel_D = nChannel_S;//theo như thầy nói thì kênh màu của ảnh cần chuyển có cùng số nChanel với ảnh xám
		int widthStep_D = destinationImage.step[0];
		// Truy xuất đến các pixel
		for (int j = 0; j<nrow; j++, pSource += widthStep_S, pDes += widthStep_D) {
			//lấy con trỏ đầu mỗi dòng
			uchar* pRow = pSource;
			uchar* pRowDes = pDes;
			for (int i = 0; i<ncol; i++, pRow+=nChannel_S, pRowDes+=nChannel_D) {
				// xử lý trên mỗi pixel
				//Gray to RGB[A]:R=Y*R,G=Y*G,B=Y*B
				pRowDes[0]= pRow[0];
				pRowDes[1]= pRow[0];
				pRowDes[2]= pRow[0];
			}   
		}
		return 0;
	}
		/*
	Hàm chuyển đổi không gian màu của ảnh từ RGB sang HSV
	sourceImage: ảnh input
	destinationImage: ảnh output
	Hàm trả về
	true: nếu chuyển thành công
	false: nếu chuyển thất bại (không đọc được ảnh input,...)
	*/
	bool RGB2HSV(const Mat& sourceImage, Mat& destinationImage)
	{
		if (sourceImage.empty()) return false; // Kiểm tra ảnh gốc
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type()); //Tạo ảnh kết quả từ ảnh gốc
		if (destinationImage.empty())return false; // Kiểm tra ảnh kết quả
		int nChanel = sourceImage.step[1];
		int nCols = sourceImage.cols;
		int nRows = sourceImage.rows;
		uchar* pDataSource = (uchar*)sourceImage.data;
		uchar* pDataDes = (uchar*)destinationImage.data;
		int widthStep = sourceImage.step[0];
		int widthStepDes = destinationImage.step[0];
		int nChanelDes = destinationImage.step[1];
		for (int i = 0; i < nRows; i++) 
		{
			uchar* pRow = pDataSource;
			uchar* pRowDes = pDataDes;
			for (int j = 0; j < nCols; j++) 
			{
				for (int z = 0; z < nChanel; z++)
				{
					uchar blue = pRow[0];
					uchar green = pRow[1];
					uchar red = pRow[2];
					//Vì RGB thuộc [0..255], nên khi chuyển về HSV cần chuyển sang [0,1] nên chia lần lượt R,G,B cho 255
					float fBlue = float(blue) / 255;
					float fGreen = float(green) / 255;
					float fRed = float(red) / 255;

					float max, min, H, S, V;
					max = min = H = S = V = 0;
					//Tìm max(R,G,B),min(R,G,B)
					if (blue == red && red == green)
					{
						max = min = fBlue;
					}
					else
					{
						if (blue > red)
						{
							if (red > green)
							{
								max = fBlue;
								min = fGreen;
							}
							else
							{
								if (blue > green)
								{
									max = fBlue;
									min = fRed;
								}
								else
								{
									max = fGreen;
									min = fRed;
								}
							}
						}
						else
						{
							if (blue > green)
							{
								max = fRed;
								min = fGreen;
							}
							else
							{
								if (green > red)
								{
									max = fGreen;
									min = fBlue;
								}
								else {
									max = fRed;
									min = fBlue;
								}
							}
						}
					}

					V = max; //gán max = V
					//S= 0 nếu V=0
					if (max == 0)
					{
						S = 0;
					}
					//S=(V-min(R,G,B))/V nếu V !=0
					else
					{
						S = (V - min) / V;
					}

					if (V == min)
					{
						H = 0;
					}
					else
					{
						//Nếu V = R, H = 60*(G-B)/(V-min(R,G,B))
						if (V == fRed)
						{
							H = 60 * (fGreen - fBlue) / (V - min);
						}
						else if (V == fGreen) //Nếu V = G, H=120+60(B-R)/(V-min(R,G,B))
						{
							H = 60 * (2 + (fBlue - fRed) / (V - min));
						}
						else if (V == fBlue) //Nếu V = B, H=240+60(R-G)/(V-min(R,G,B))
						{
							H = 60 * (4 + (fRed - fGreen) / (V - min));
						}
					}
					if (H < 0) {
						H += 360;
					}
					//giá trị H,S,V sau khi chuyển đổi, đưa trở về lại [0...255]
					pRowDes[0] = round(H / 2);
					pRowDes[1] = round(255 * S);
					pRowDes[2] = round(255 * V);
				}
				pRow += nChanel;
				pRowDes += nChanelDes;
			}
			pDataSource += widthStep;
			pDataDes += widthStepDes;
		}
		return true;
	}
	/*
	Hàm chuyển đổi không gian màu của ảnh từ HSV sang RGB
	sourceImage: ảnh input
	destinationImage: ảnh output
	Hàm trả về
	true: nếu chuyển thành công
	false: nếu chuyển thất bại (không đọc được ảnh input,...)
	*/
	bool HSV2RGB(const Mat& sourceImage, Mat& destinationImage)
	{
		if (sourceImage.empty()) return false;// kiểm tra ảnh gốc
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());// tạo ảnh kết quả từ ảnh gốc
		if (destinationImage.empty())return false;// kiểm tra ảnh kết quả
		int nChanel = sourceImage.step[1];
		int nCols = sourceImage.cols;
		int nRows = sourceImage.rows;
		uchar* pDataSource = (uchar*)sourceImage.data;
		uchar* pDataDes = (uchar*)destinationImage.data;
		int widthStep = sourceImage.step[0];
		int widthStepDes = destinationImage.step[0];
		int nChanelDes = destinationImage.step[1];
		for (int i = 0; i < nRows; i++) 
		{
			uchar* pRow = pDataSource;
			uchar* pRowDes = pDataDes;
			for (int j = 0; j < nCols; j++)
			{
				for (int z = 0; z < nChanel; z++)
				{
					// H có miền giá trị từ 0 đến 360°, S,V nằm trong khoảng [0,1]
					double fH = double(pRow[0] * 2);
					double fS = double(pRow[1]) / 255;
					double fV = double(pRow[2]) / 255;
					/*Áp dụng công thức C = V × S, X = C × (1 - |(H / 60°) mod 2 - 1|),m = V - C
					(R',G',B')= (C,X,0), 0° <= H <= 60°
					(R',G',B')= (X,C,0), 60° <= H <= 120°
					(R',G',B')= (0,C,X), 120° <= H <= 180°
					(R',G',B')= (0,X,C), 180° <= H <= 240°
					(R',G',B')= (X,0,C), 240° <= H <= 300°
					(R',G',B')= (C,0,X), 300° <= H <= 360°
					*/
					double fC = fS * fV;
					double fHComma = fH / 60;
					double fX = fC * (1.0 - abs(fmod(fHComma, 2) - 1.0));

					double fR1, fG1, fB1;
					fR1 = fG1 = fB1 = 0;
					if (fHComma <= 1)
					{
						fR1 = fC;
						fG1 = fX;
						fB1 = 0;
					}
					else if (fHComma <= 2)
					{
						fR1 = fX;
						fG1 = fC;
						fB1 = 0;
					}
					else if (fHComma <= 3)
					{
						fR1 = 0;
						fG1 = fC;
						fB1 = fX;
					}
					else if (fHComma <= 4)
					{
						fR1 = 0;
						fG1 = fX;
						fB1 = fC;
					}
					else if (fHComma <= 5)
					{
						fR1 = fX;
						fG1 = 0;
						fB1 = fC;
					}
					else if (fHComma <= 6)
					{
						fR1 = fC;
						fG1 = 0;
						fB1 = fX;
					}

					double m = fV - fC;
					double fR, fG, fB;
					fR = fR1 + m;
					fG = fG1 + m;
					fB = fB1 + m;

					pRowDes[0] = uchar(fB * 255);
					pRowDes[1] = uchar(fG * 255);
					pRowDes[2] = uchar(fR * 255);
				}
				pRow += nChanel;
				pRowDes += nChanelDes;
			}
			pDataSource += widthStep;
			pDataDes += widthStepDes;
		}
		return true;
	}
}



