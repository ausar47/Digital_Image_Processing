#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
BITMAPFILEHEADER fileHeader;// 位图头文件
BITMAPINFOHEADER infoHeader;// 位图信息头
FILE* pFile;
int Clr = 0;
WORD bitCount;
LONG pW;     //图像数据的宽度
LONG pH;    //图像数据的高度
RGBQUAD* QUAD = NULL;
int pSize;  //图像数据大小
int LineSize;  //行数据大小
BYTE* P;   //申请空间保存图像数据
void ReadBmp(char* a);
void imf(char* b);
void lie(char* b);

int main()
{
	char* filename;
	filename = (char*)malloc(100 * sizeof(char));
	printf("请输入文件路径：");
	scanf("%s", filename);
	ReadBmp(filename);
	imf("imfa.bmp");
	ReadBmp(filename);
	lie("liea.bmp");
	free(filename);
	return 0;
}

void ReadBmp(char* a)
{
	pFile = fopen(a, "rb");
	if (pFile == NULL)
	{
		printf("打开文件失败\n");
		exit(-1);
	}
	// 读取头信息
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);   //读取文件头
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);   //读取信息头
	bitCount = infoHeader.biBitCount;//色彩的位数
	if (bitCount == 16)
		exit(-1);
	if (bitCount < 16)
	{
		Clr = infoHeader.biClrUsed ? infoHeader.biClrUsed : 1 << bitCount;
		if (Clr > 256)
			Clr = 0;
	}
	// 读取调色板
	int i = 0, j = 0;
	if (Clr > 0)
	{
		QUAD = (RGBQUAD*)malloc(sizeof(RGBQUAD) * Clr);
		fread(QUAD, sizeof(RGBQUAD) * Clr, 1, pFile);
		for (i = 0; i < Clr; i++)
		{
			QUAD[i].rgbRed = QUAD[i].rgbBlue = QUAD[i].rgbGreen =
				(BYTE)(0.3 * QUAD[i].rgbRed + 0.59 * QUAD[i].rgbGreen + 0.11 * QUAD[i].rgbBlue);  //灰度图像三个都是y
		}
	}
	pW = infoHeader.biWidth;     //图像数据的宽度
	pH = infoHeader.biHeight;    //图像数据的高度

	pSize = pW * 3 * pH;  //图像数据大小
	printf("%d %d ", pW, pH);
	printf("%d\n", pSize);
	LineSize = pW * 3;  //行数据大小
	P = (BYTE*)malloc(pSize);   //申请空间保存图像数据
	fread(P, pSize, 1, pFile);     //存入图像数据
}

//线性均值滤波
void imf(char* b)
{
	//先转grey图像
	if (Clr == 0)
	{
		for (int i = 0; i < pH; i++)//行
		{
			if (bitCount == 24)
			{
				for (int j = 0; j < pW * 3; j = j + 3)// 列
				{
					int n = i * LineSize + j;
					P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //分别是r,g,b分量
				}
			}
		}
	}
	//均值滤波
	for (int i = 1; i < pH - 1; i++)//行
	{
		for (int j = 3; j < (pW - 1) * 3; j = j + 3)// 列
		{
			//int n = i*LineSize + j;
			P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)((P[i * LineSize + j] + P[i * LineSize + j + 3] + P[i * LineSize + j - 3] + P[(i - 1) * LineSize + j] + P[(i - 1) * LineSize + j + 3] + P[(i - 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j + 3] + P[(i + 1) * LineSize + j]) / 9);  //等于9个像素点的均值

		}
	}
	FILE* dFile = fopen(b, "wb");//创建目标文件

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //写入文件头

	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //写入文件信息头
	if (QUAD)
	{
		fwrite(QUAD, sizeof(RGBQUAD) * Clr, 1, dFile);
	}
	//写入图像数据
	fwrite(P, pSize, 1, dFile);
	fclose(dFile);

	if (QUAD)
	{
		free(QUAD);
		QUAD = NULL;
	}
	if (P)
	{
		free(P);
		P = NULL;
	}
}

void lie(char* b)
{
	//先转grey图像
	if (Clr == 0)
	{
		for (int i = 0; i < pH; i++)//行
		{
			if (bitCount == 24)
			{
				for (int j = 0; j < pW * 3; j = j + 3)// 列
				{
					int n = i * LineSize + j;
					P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //分别是r,g,b分量

				}
			}

		}
	}
	//拉普拉斯
	int w; //和
	for (int i = 1; i < pH - 1; i++)//行
	{
		for (int j = 3; j < (pW - 1) * 3; j = j + 3)// 列
		{
			//int n = i*LineSize + j;
			w = P[i * LineSize + j + 3] + P[i * LineSize + j - 3] + P[(i - 1) * LineSize + j] + P[(i - 1) * LineSize + j + 3] + P[(i - 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j + 3] + P[(i + 1) * LineSize + j] - P[i * LineSize + j] * 8;
			w *= 0.1;
			//权重正负分类

			if (w >= 0) {
				//
				if (P[i * LineSize + j] + w > 255)
					P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)(255);
				else if (P[i * LineSize + j] + w < 0)
					P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)(0);
				else
					P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)(P[i * LineSize + j] + w);
			}

			else {
				if (P[i * LineSize + j] - w > 255)
					P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)(255);
				else if (P[i * LineSize + j] - w < 0)
					P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)(0);
				else
					P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)(P[i * LineSize + j] - w);
			}
		}
	}
	FILE* dFile = fopen(b, "wb");//创建目标文件

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //写入文件头

	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //写入文件信息头
	if (QUAD)
	{
		fwrite(QUAD, sizeof(RGBQUAD) * Clr, 1, dFile);
	}
	//写入图像数据
	fwrite(P, pSize, 1, dFile);
	fclose(dFile);

	if (QUAD)
	{
		free(QUAD);
		QUAD = NULL;
	}
	if (P)
	{
		free(P);
		P = NULL;
	}
}