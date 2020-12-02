#pragma once

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

namespace ColorTransformer {
	/*
	hàm nhận vào 1 ảnh, trả ra histogram của ảnh
	Hàm trả về:
		trả ra 1 mảng chứa mảng các histogram
	*/
	vector<array<int, 256>> Hist(const Mat& sourceImage) 
	{
    //vector chưa từng màu của kênh màu
		vector<array<int, 256>> result; 
    // ảnh gốc rỗng
		if (sourceImage.empty())    
			return move(result);

		int widthStep = sourceImage.step[0];
		int Channel = sourceImage.step[1];
		int rows = sourceImage.rows;
		int cols = sourceImage.cols;

    //gán mỗi kênh màu tối đa 256 giá trị, khởi tạo bằng 0
		for (int i = 0; i < Channel; i++) 
		{
  		result.emplace_back();
		}

		uchar* pData = (uchar*)sourceImage.data;//duyệt ảnh
		for (int i = 0; i < rows; i++) 
		{
			uchar* pRow = pData;
			for (int j = 0; j < cols; j++) 
			{
				for (int z = 0; z < Channel; z++) 
				{
          //ứng với từng kênh màu, tăng giá trị tại vị trí giá trị của kênh màu đó thêm 1 (trong histogram equalization mới chia cho tất cả số điểm ảnh, theo thuật toán trên lớp lý thuyết)
					result[z][pRow[z]] += 1;
				}
				pRow += Channel;
			}
			pData += widthStep;
		}
		return move(result);
	}

	bool DrawHist(const Mat& src, Mat& dest) {
		if (src.empty()) return false;// kiểm tra ảnh gốc
		int n_channel = src.step[1];
		dest = Mat(100*(n_channel + 1), 256, CV_8UC3);
		auto hist = ColorTransformer::Hist(src);
		if (dest.empty())return false;// kiểm tra ảnh kết quả
		uchar* p_src = (uchar*)src.data;
		uchar* p_dest = (uchar*)dest.data;

		auto offset = 100*256*n_channel;
		for (int i = 0; i < 100; i++) 
		{
			for (int j = 0; j < 256; j++)
			{
    		for (int c = 0; c < n_channel; c++) {
    			auto norm = double(hist[c][j]) / double(src.cols * src.rows) * 20.0;
  				*p_dest = p_dest[(c+1) * offset] = norm * 100 >= (100 - i) ? 255 : 0;
  				p_dest++;
    		}
			}
		}
		return true;
	}
	
	/*
	Hàm so sánh hai ảnh
	Tham so :
		image1 : ảnh thứ nhất
		image2 : ảnh thứ hai
	Hàm trả về:
		độ đo sự tương đồng giữa hai ảnh
	*/
	double CompareImage(const Mat& image1, const Mat& image2)
	{
		// Sử dụng độ đo Alternative Chi-Square
		vector<array<int, 256>> imageRGB_1, imageRGB_2;
		int image1_chanel = image1.step[1];
		int image2_chanel = image2.step[1];

		if (image1_chanel != image2_chanel)
			return -1;

		imageRGB_1 = Hist(image1);
		imageRGB_2 = Hist(image2);

		double result = 0;

		for (int i = 0; i < image1_chanel; i++) 
		{
			for (int j = 0; j < 256; j++)
			{
				result += 2 * (imageRGB_1[i][j] - imageRGB_2[i][j]) * (imageRGB_1[i][j] - imageRGB_2[i][j]) / (imageRGB_1[i][j] + imageRGB_2[i][j]);
			}
		}
		return result;
	}

	/*
	Hàm nhận vào một ảnh, thay đổi độ tương phản của ảnh này và lưu kết quả vào ảnh mới
	Tham so :
		sourceImage : ảnh ban đầu
		destinationImage : ảnh kết quả
		c	: giá trị số thực dùng để thay đổi độ tương phản của ảnh
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c)
	{
  	destinationImage = sourceImage.clone();
 
		int height = sourceImage.rows;
		int width = sourceImage.cols;
		int srcChannels = sourceImage.channels();
		int dstChannels = destinationImage.channels();
		destinationImage = cv::Mat(height, width, CV_8UC3); // Tạo ảnh kết quả với kích thước bằng ảnh đầu vào

		for (int x = 0; x < height; x++)
		{
			uchar* pSrcRow = (uchar*)(sourceImage.ptr<uchar>(x));
			uchar* pDstRow = destinationImage.ptr<uchar>(x);
			for (int y = 0; y < width; y++, pSrcRow += srcChannels, pDstRow += dstChannels)
			{
				// Nếu giá trị mức xám vượt quá 255 thì gán bằng 255
				// pSrcRow[0] , pSrcRow[1] , pSrcRow[2] lần lượt là giá trị 3 kênh màu tương ứng B , G , R
				if ((pSrcRow[0] * (uchar)(c)) > 255)
				{
					pDstRow[0] = 255;
				}
				else
				{
					pDstRow[0] = (uchar)((float)(pSrcRow[0]) * c);
				}

				if ((pSrcRow[1] * (uchar)(c)) > 255)
				{
					pDstRow[1] = 255;
				}
				else
				{
					pDstRow[1] = (uchar)((float)(pSrcRow[1]) * c);
				}

				if ((pSrcRow[2] * (uchar)(c)) > 255)
				{
					pDstRow[2] = 255;
				}
				else
				{
					pDstRow[2] = (uchar)((float)(pSrcRow[2]) * c);
				}
			}
		}
		return 0;
	}

	/*
	Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
	Tham so :
		image : ảnh dùng để tính histogram
		histImage : ảnh histogram
	Hàm trả về:
		1: Nếu thành công vẽ được histogram
		0: Nếu không vẽ được histogram
	*/
	int HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
	{
  	destinationImage = sourceImage.clone();

		int height = sourceImage.rows;
		int width = sourceImage.cols;
		int srcChannels = sourceImage.channels();
		int dstChannels = destinationImage.channels();
		destinationImage = cv::Mat(height, width, CV_8UC3); // Tạo ma trận lưu ảnh kết quả với kích thước bằng kích thước ảnh đầu vào

		// Tạo mảng H(2 chiều): bảng tần số phân bố xác suất các mức xám của ảnh đầu vào
		// Với chiều rộng là 256 phần tử chứa các mức xám của ảnh từ 0-255
		// Và chiều dài là 3 tương ứng với 3 kênh màu B , G , R tại mỗi điểm ảnh

		int H[3][256];
		// Tạo mảng T(2 chiều) : Bảng tần số phân bố xác suất các mức xám của ảnh sau khi cân bằng
		// Với chiều rộng là 256 phần tử chứa các mức xám của ảnh từ 0-255
		// Và chiều dài là 3 tương ứng với 3 kênh màu B , G , R tại mỗi điểm ảnh
		int T[3][256];

		// Tạo mảng Histogram(2 chiều) : Bảng giá trị các mức xám của ảnh sau khi cân bằng
		// Với chiều rộng là 256 phần tử chứa các mức xám của ảnh từ 0-255
		// Và chiều dài là 3 tương ứng với 3 kênh màu B , G , R tại mỗi điểm ảnh
		int Histogram[3][256];

		// Cho tất cả các giá trị của ma trận H,T,Histogram bằng 0
		for (int c = 0; c < 3; c++)
		{
			for (int i = 0; i < 256; i++)
			{
				H[c][i] = 0;
				T[c][i] = 0;
				Histogram[c][i] = 0;
			}
		}

		// Tính tần số phân  bố xác suất các mức xám của từng điểm ảnh trong ảnh đầu vào
		for (int x = 0; x < height; x++)
		{
			uchar* pSrcRow = (uchar*)(sourceImage.ptr<uchar>(x));
			for (int y = 0; y < width; y++, pSrcRow += srcChannels)
			{
				// f là giá trị mức xám từ 0-255
				int f0 = (int)(pSrcRow[0]); // kênh màu B
				int f1 = (int)(pSrcRow[1]); // kênh màu G
				int f2 = (int)(pSrcRow[2]); // kênh màu R

				// Khi giá trị pSrcRow bằng giá trị tương ứng của f thì giá trị tương ứng tại mảng H tăng lên 1
				H[0][f0]++; 
				H[1][f1]++;
				H[2][f2]++;
			}
		}

		//Gán giá trị đầu tiên của ma trận T = giá trị đầu tiên của ma trận H
		T[0][0] = H[0][0];
		T[1][0] = H[1][0];
		T[2][0] = H[2][0];

		// Tính tất cả các giá trị của ma trận T (Bảng phân bố tần số xác suất các mức xám của ảnh sau khi cân bằng)
		for (int d = 0; d < 3; d++)
		{
			for (int p = 1; p < 256; p++)
			{
				T[d][p] = H[d][p] + T[d][(p - 1)];
				Histogram[d][p] = ((255 * T[d][p]) / (height * width)); // Chuẩn hóa các giá trị điểm ảnh
			}
		}


		// Ghép lại ảnh sau khi chuẩn hóa các giá trị mức xám theo bảng tần số phân bố xác suất

		for (int x = 0; x < height; x++)
		{
			uchar* pSrcRow = (uchar*)(sourceImage.ptr<uchar>(x));
			uchar* pDstRow = destinationImage.ptr<uchar>(x);
			for (int y = 0; y < width; y++, pSrcRow += srcChannels, pDstRow += dstChannels)
			{
				pDstRow[0] = (uchar)(Histogram[0][pSrcRow[0]]);
				pDstRow[1] = (uchar)(Histogram[1][pSrcRow[1]]);
				pDstRow[2] = (uchar)(Histogram[2][pSrcRow[2]]);
			}
		}

		return 1;
	}

	/*
	Hàm nhận vào một ảnh, thay đổi độ sáng của ảnh này và lưu kết quả vào ảnh mới
	Tham so:
	sourceImage	: ảnh ban đầu
	destinationImage: ảnh kết quả
	b	: giá trị số nguyên dùng để thay đổi độ sáng của ảnh
	Hàm trả về:
	1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
	0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeBrightness(const Mat& sourceImage, Mat& destinationImage, short b)
	{
		if (sourceImage.empty())
			return 0;
		destinationImage = sourceImage.clone();

		int height = sourceImage.rows;
		int width = sourceImage.cols;
		int srcChannels = sourceImage.channels();
		int dstChannels = destinationImage.channels();
		destinationImage = cv::Mat(height, width, CV_8UC3); // Tạo ảnh kết quả với kích thước bằng ảnh đầu vào

		for (int x = 0; x < height; x++)
		{
			uchar* pSrcRow = (uchar*)(sourceImage.ptr<uchar>(x));
			uchar* pDstRow = destinationImage.ptr<uchar>(x);
			for (int y = 0; y < width; y++, pSrcRow += srcChannels, pDstRow += dstChannels)
			{
				// Nếu giá trị mức xám vượt quá 255 thì gán bằng 255
				// pSrcRow[0] , pSrcRow[1] , pSrcRow[2] lần lượt là giá trị 3 kênh màu tương ứng B , G , R
				if ((pSrcRow[0] + (uchar)(b)) > 255)
				{
					pDstRow[0] = 255;
				}
				else
				{
					pDstRow[0] = (uchar)(pSrcRow[0] + b);
				}

				if ((pSrcRow[1] + (uchar)(b)) > 255)
				{
					pDstRow[1] = 255;
				}
				else
				{
					pDstRow[1] = (uchar)((float)(pSrcRow[1]) + b);
				}

				if ((pSrcRow[2] + (uchar)(b)) > 255)
				{
					pDstRow[2] = 255;
				}
				else
				{
					pDstRow[2] = (uchar)((float)(pSrcRow[2]) + b);
				}
			}
		}
		return 1;
	}
}



