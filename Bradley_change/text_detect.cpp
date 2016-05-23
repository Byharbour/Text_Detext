#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "cvaux.h"
#include<iostream>
int inputmatrix[2000000];
long outputmatrix[2000000];
int  pt[2000][1000];
void integral(int* inputmatrix, long* outputmatrix, int width, int height){
	// calculate integral of the first line  
	
	for (int i = 0; i<height; i++){
		outputmatrix[i] = inputmatrix[i];
		if (i>0){
			outputmatrix[i] += outputmatrix[i - 1];
		}
	}
	for (int i = 1; i<width; i++){
		int offset = i*height;
		// first column of each line  
		outputmatrix[offset] = outputmatrix[offset - height] + inputmatrix[offset];
		// other columns   
		for (int j = 1; j < height; j++)

		{
			outputmatrix[offset + j] = outputmatrix[offset + j - 1] + outputmatrix[offset - height + j] + inputmatrix[offset + j] - outputmatrix[offset + j - height - 1];
		}
	}

/*	for (int i = 0; i<height; i++){
		outputMatrix[i] = outputMatrix[i] / (i + 1);
	}
	for (int i = 1; i<width; i++){
		int offset = i*height;
		// first column of each line  
		outputMatrix[offset] = outputMatrix[offset] / (i + 1);
		// other columns   
		for (int j = 1; j < height; j++)

		{
			outputMatrix[offset + j] = outputMatrix[offset + j] / ((i + 1)*(j + 1));
		}
	}
	*/
	//for (int i = 0; i < width*height; i++)
		//std::cout << outputMatrix[i] << " ";
}
void meanvalue(long* outputmatrix, int width, int height,int s)
{
	int k = 0;
	memset(pt, 0, sizeof(pt));
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			pt[j][i] = outputmatrix[k++];
		}
	}

	int row,list;
	int list_start, list_end, row_start, row_end;
	for (int i = 0; i < width*height; i++)
	{
		int cnt = 1;
		row = (i + 1) % height-1;
		if ((i+1)%height == 0)
		{
			list = (i + 1) / height-1;
		}
		else
			list = (i + 1) / height ;

		if (list >= s / 2)
		{
			list_start = list - s / 2;
		}
		else{
			list_start = 0;
		}
		if (list + s / 2 < width){
			list_end = list + s / 2;
		}
		else{
			list_end = width-1;
		}
		if (row >= s / 2)
		{
			row_start = row - s / 2;
		}
		else{
			row_start = 0;
		}
		if (row + s / 2 < height){
			row_end = row + s / 2;
		}
		else{
			row_end = height-1;
		}
		if (row_start != 0 && list_start != 0){
			outputmatrix[i] = pt[row_end][list_end] - pt[row_start - 1][list_end] - pt[row_end][list_start - 1] + pt[row_start - 1][list_start - 1];
		}
		else if (row_start == 0 && list_start != 0)
		{
			outputmatrix[i] = pt[row_end][list_end] - pt[row_end][list_start - 1];
		}
		else if (list_start == 0 && row_start != 0){
			outputmatrix[i] = pt[row_end][list_end] - pt[row_start - 1][list_end];
		}
		else if (list_start == 0 && row_start == 0){
			outputmatrix[i] = pt[row_end][list_end];
		}
		cnt = (row_end - row_start + 1)*(list_end-list_start+1);
		outputmatrix[i] = outputmatrix[i]/cnt;
		//std::cout << outputmatrix[i] << " ";
	}
}
int main()
{
	IplImage* src = NULL;
	cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
	src = cvLoadImage("8.png", -1);
	cvShowImage("src", src);
	IplImage* gray_image = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);//创建图像
	cvCvtColor(src, gray_image, CV_BGR2GRAY);//彩色图像转换为灰度图；
	cvNamedWindow("gray_image", CV_WINDOW_AUTOSIZE);
	cvShowImage("gray_image", gray_image);
	
	IplImage* binary_image = cvCreateImage(cvGetSize(gray_image), IPL_DEPTH_8U, 1);
	cvNamedWindow("binary_image", CV_WINDOW_AUTOSIZE);
	cvThreshold(gray_image, binary_image, 128, 255, CV_THRESH_BINARY);//灰度图转换为二值图,阈值待定。
	cvShowImage("binary_image", binary_image);

	//积分图计算
	int height = gray_image->height;
	int width = gray_image->width;
	memset(inputmatrix, 0, sizeof(inputmatrix));
	memset(outputmatrix, 0, sizeof(outputmatrix));
	for (int i = 0; i < width; i++)	
	{
		for (int j = 0; j < height; j++)
		{
			inputmatrix[i*height + j] = cvGet2D(gray_image, j, i).val[0];
		}
	}

	int size =  width/4;
	integral(inputmatrix, outputmatrix, width, height);
	
	meanvalue(outputmatrix,width,height,size);
	//for (int i = 0; i < width*height; i++)
		//std::cout << outputmatrix[i] << " ";
	int t=-0.15;
	int ptr;
	for (int i = 0; i < width; i++)//保留对比度强的区域，忽略平滑过度区。
	{
		//uchar* ptr = (uchar*)gray_image->imageData + i*gray_image->height;
		for (int j = 0; j < height; j++)
		{			
			ptr = cvGet2D(gray_image, j, i).val[0];
			if (ptr < outputmatrix[i*height + j] * (1 - t))
			{
				ptr = 0;
			}
			else
				ptr = 255;
			cvSet2D(gray_image, j, i, ptr);
			
		}
	}
	
	cvNamedWindow("gray_image1", CV_WINDOW_AUTOSIZE);
	cvShowImage("gray_image1", gray_image);
	
	cvWaitKey(0);
	cvReleaseImage(&src);
	cvReleaseImage(&gray_image);
	cvReleaseImage(&binary_image);
	
	return 0;

}