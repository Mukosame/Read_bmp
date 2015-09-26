// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "targetver.h"
#include "stdlib.h"
#include "math.h"
#include <iostream>

#define LENGTH_NAME_BMP 30//bmp图片文件名的最大长度

using namespace std;

//变量定义
BITMAPFILEHEADER strHead;
RGBQUAD strPla[256];//256色调色板
BITMAPINFOHEADER strInfo;

//显示位图文件头信息
void showBmpHead(BITMAPFILEHEADER pBmpHead) {
	cout << "位图文件头:" << endl;
	cout << "文件大小:" << pBmpHead.bfSize << endl;
	cout << "保留字_1:" << pBmpHead.bfReserved1 << endl;
	cout << "保留字_2:" << pBmpHead.bfReserved2 << endl;
	cout << "实际位图数据的偏移字节数:" << pBmpHead.bfOffBits << endl << endl;
}

void showBmpInforHead(tagBITMAPINFOHEADER pBmpInforHead) {
	cout << "位图信息头:" << endl;
	cout << "结构体的长度:" << pBmpInforHead.biSize << endl;
	cout << "位图宽:" << pBmpInforHead.biWidth << endl;
	cout << "位图高:" << pBmpInforHead.biHeight << endl;
	cout << "biPlanes平面数:" << pBmpInforHead.biPlanes << endl;
	cout << "biBitCount采用颜色位数:" << pBmpInforHead.biBitCount << endl;
	cout << "压缩方式:" << pBmpInforHead.biCompression << endl;
	cout << "biSizeImage实际位图数据占用的字节数:" << pBmpInforHead.biSizeImage << endl;
	cout << "X方向分辨率:" << pBmpInforHead.biXPelsPerMeter << endl;
	cout << "Y方向分辨率:" << pBmpInforHead.biYPelsPerMeter << endl;
	cout << "使用的颜色数:" << pBmpInforHead.biClrUsed << endl;
	cout << "重要颜色数:" << pBmpInforHead.biClrImportant << endl;
}


int main()
{
	char strFile[LENGTH_NAME_BMP];//原bmp图片的名字
	IMAGEDATA *imagedata = NULL;//动态分配存储原图片的像素信息的二维数组
	IMAGEDATA *imagedataRot = NULL;//动态分配存储反色后的图片的像素信息的二维数组
	int width, height;//图片的宽度和高度

	cout << "Please input the file name:" << endl;
	cin >> strFile;
	FILE *fpi, *fpw;
	errno_t err;
	if ((err = fopen_s(&fpi, strFile, "rb")) != 0)
		printf("The file was not opened\n");
	else
		printf("The file was opened\n");

	if (fpi != NULL) {
		//先读取文件类型
		WORD bfType;
		fread(&bfType, 1, sizeof(WORD), fpi);
		if (0x4d42 != bfType)
		{
			cout << "the file is not a bmp file!" << endl;
			return NULL;
		}
		//读取bmp文件的文件头和信息头
		fread(&strHead, 1, sizeof(tagBITMAPFILEHEADER), fpi);
		//showBmpHead(strHead);//显示文件头
		fread(&strInfo, 1, sizeof(tagBITMAPINFOHEADER), fpi);
		//showBmpInforHead(strInfo);//显示文件信息头

		//读取调色板
		for (unsigned int nCounti = 0;nCounti<strInfo.biClrUsed;nCounti++)
		{
			fread((char *)&(strPla[nCounti].rgbBlue), 1, sizeof(BYTE), fpi);
			fread((char *)&(strPla[nCounti].rgbGreen), 1, sizeof(BYTE), fpi);
			fread((char *)&(strPla[nCounti].rgbRed), 1, sizeof(BYTE), fpi);
			fread((char *)&(strPla[nCounti].rgbReserved), 1, sizeof(BYTE), fpi);
		}

		width = strInfo.biWidth;
		height = strInfo.biHeight;
		//图像每一行的字节数必须是4的整数倍
		width = (width * sizeof(IMAGEDATA) + 3) / 4 * 4;
		imagedata = (IMAGEDATA*)malloc(width * height * sizeof(IMAGEDATA));
		imagedataRot = (IMAGEDATA*)malloc(width * height* sizeof(IMAGEDATA));
		//初始化原始图片的像素数组
		for (int i = 0;i < height;++i)
		{
			for (int j = 0;j < width;++j)
			{
				(*(imagedata + i * width + j)).blue = 0;
				(*(imagedata + i * width + j)).green = 0;
				(*(imagedata + i *  width + j)).red = 0;
			}
		}
		//初始化转换后图片的像素数组
		for (int i = 0;i < 2 * height;++i)
		{
			for (int j = 0;j < 2 * width;++j)
			{
				(*(imagedataRot + i * width + j)).blue = 0;
				(*(imagedataRot + i * width + j)).green = 0;
				(*(imagedataRot + i * width + j)).red = 0;
			}
		}
		//fseek(fpi,54,SEEK_SET);
		//读出图片的像素数据
		fread(imagedata, sizeof(struct tagIMAGEDATA) * width, height, fpi);
		fclose(fpi);
	}
	else
	{
		cout << "file open error!" << endl;
		return NULL;
	}

	//图片反色处理
	for (int i = 0;i < 2 * height;++i)
	{
		for (int j = 0;j < 2 * width;++j)
		{
			(*(imagedataRot + i * width + j)).blue = 255-(*(imagedataRot + i * width + j)).blue;
			(*(imagedataRot + i * width + j)).green = 0;
			(*(imagedataRot + i * width + j)).red = 0;
		}
	}

	//保存新图片
	char * newname = strcat(strFile ,"_rev.bmp");

	if ((fpw = fopen(newname , "wb")) == NULL)
	{
		cout << "create the bmp file error!" << endl;
		return NULL;
	}
	WORD bfType_w = 0x4d42;
	fwrite(&bfType_w, 1, sizeof(WORD), fpw);
	//fpw +=2;
	fwrite(&strHead, 1, sizeof(tagBITMAPFILEHEADER), fpw);
	strInfo.biWidth = 2 * width;
	strInfo.biHeight = 2 * height;
	fwrite(&strInfo, 1, sizeof(tagBITMAPINFOHEADER), fpw);
	//保存调色板数据
	for (unsigned int nCounti = 0;nCounti<strInfo.biClrUsed;nCounti++)
	{
		fwrite(&strPla[nCounti].rgbBlue, 1, sizeof(BYTE), fpw);
		fwrite(&strPla[nCounti].rgbGreen, 1, sizeof(BYTE), fpw);
		fwrite(&strPla[nCounti].rgbRed, 1, sizeof(BYTE), fpw);
		fwrite(&strPla[nCounti].rgbReserved, 1, sizeof(BYTE), fpw);
	}
	//保存像素数据
	for (int i = 0;i < 2 * height;++i)
	{
		for (int j = 0;j < 2 * width;++j)
		{
			fwrite(&((*(imagedataRot + i * 2 * width + j)).blue), 1, sizeof(BYTE), fpw);
			fwrite( &((*(imagedataRot + i * 2 * width + j)).green),1,sizeof(BYTE),fpw);
			fwrite( &((*(imagedataRot + i * 2 * width + j)).red),1,sizeof(BYTE),fpw);
		}
	}

	fclose(fpw);

	//释放内存
	delete[] imagedata;
	delete[] imagedataRot;

    return 0;
}

