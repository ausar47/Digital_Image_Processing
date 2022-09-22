#include <stdio.h>
#include <Windows.h>
#include <math.h>
BITMAPFILEHEADER fileHeader;// 位图头文件
BITMAPINFOHEADER infoHeader;// 位图信息头
FILE* pFile;
int Clr = 0;
WORD bitCount;
LONG pW;     //图像数据的宽度
LONG pH;    //图像数据的高度
RGBQUAD *QUAD = NULL;
int pSize;  //图像数据大小
int LineSize;  //行数据大小
BYTE* P;   //申请空间保存图像数据

void ReadBmp(char *a);
void Logarithmic(char* b);
void Histogram(char* b);

int main()
{
    char *filename;
    filename = (char*) malloc(100 * sizeof(char));
    printf("请输入文件路径：");
    scanf("%s", filename);
    ReadBmp(filename);
    Logarithmic("lgc.bmp");
    ReadBmp(filename);
    Histogram("Hc1.bmp");
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
        if (Clr>256)
            Clr = 0;
    }
    // 读取调色板
    int i = 0, j = 0;
    if (Clr > 0)
    {
        QUAD = (RGBQUAD*)malloc(sizeof(RGBQUAD)*Clr);
        fread(QUAD, sizeof(RGBQUAD)*Clr, 1, pFile);
        for (i = 0; i < Clr; i++)
        {
            QUAD[i].rgbRed = QUAD[i].rgbBlue = QUAD[i].rgbGreen =
                    (BYTE)(0.3 * QUAD[i].rgbRed + 0.59 * QUAD[i].rgbGreen + 0.11 * QUAD[i].rgbBlue);  //灰度图像三个都是y
        }
    }
    pW = infoHeader.biWidth;     //图像数据的宽度
    pH = infoHeader.biHeight;    //图像数据的高度

    pSize = ((pW * bitCount)>>3)*pH;  //图像数据大小
    LineSize = ((pW * bitCount)>> 3);  //行数据大小
    P = (BYTE*)malloc(pSize);   //申请空间保存图像数据
    fread(P, pSize, 1, pFile);     //存入图像数据
}

void Logarithmic(char* b)
{
    int max=0,index;
    if (Clr == 0)
        for (int i = 0; i < pH; i++)//行
            if (bitCount == 24)
            {
                for (int j = 0; j < pW*3; j = j + 3)// 列
                {
                    int n = i*LineSize + j;
                    index = P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //分别是r,g,b分量
                    if(index > max)
                        max = index;
                }
            }
//    printf("%d\n",max);
    for (int i = 0; i < pH; i++)//行
        if (bitCount == 24)
            for (int j = 0; j < pW * 3; j = j + 3)// 列
            {
                int n = i * LineSize + j;
                P[n] = P[n + 1] = P[n + 2] = (BYTE)(255 * log10(P[n] + 1) / log10(max + 1));
            }
    FILE* dFile = fopen(b, "wb");//创建目标文件
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //写入文件头
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //写入文件信息头
    if (QUAD)
        fwrite(QUAD, sizeof(RGBQUAD)* Clr, 1, dFile);
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

void Histogram(char* b)
{
    int Min = 120;
    int Max = 120;
    int now;
    double grey[256]={0};
    if (Clr == 0)
        for (int i = 0; i < pH; i++)//行
            if (bitCount == 24)
                for (int j = 0; j < pW * 3; j = j + 3)// 列
                {
                    int n = i * LineSize + j;
                    now = P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //分别是r,g,b分量
                    grey[P[n]]++;       //灰度直方图
                    if(now > Max)
                        Max = now;           //找到最大最小
                    else if(now < Min)
                        Min = now;
                }
    for(int i = 0; i < Max; i++)
        grey[i] = grey[i] / (pH * pW);
//    printf("max=%d,min=%d\n", Max, Min);
    //累计直方图
    double greyadd[256]={0};
    for(int i = 0; i < 253; i++)
        for(int j = 0; j <= i; j++)
            greyadd[i] += grey[j];
//    printf("%f %f", greyadd[0], greyadd[1]);
//    printf("\n%f\n%f\n%f", grey[0], grey[1], grey[253]);

    //新值
    if (Clr == 0)
        for (int i = 0; i < pH; i++)//行
            if (bitCount == 24)
                for (int j = 0; j < pW * 3; j = j + 3)// 列
                {
                    int n = i * LineSize + j;
                    P[n] = P[n + 1] = P[n + 2] = (BYTE)(Min + (Max - Min) * greyadd[P[n]]);
                }

    FILE* dFile = fopen(b, "wb");//创建目标文件
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //写入文件头
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //写入文件信息头
    if (QUAD)
        fwrite(QUAD, sizeof(RGBQUAD) * Clr, 1, dFile);
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