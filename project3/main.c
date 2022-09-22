#include <stdio.h>
#include <Windows.h>
#include <math.h>
BITMAPFILEHEADER fileHeader;// λͼͷ�ļ�
BITMAPINFOHEADER infoHeader;// λͼ��Ϣͷ
FILE* pFile;
int Clr = 0;
WORD bitCount;
LONG pW;     //ͼ�����ݵĿ��
LONG pH;    //ͼ�����ݵĸ߶�
RGBQUAD *QUAD = NULL;
int pSize;  //ͼ�����ݴ�С
int LineSize;  //�����ݴ�С
BYTE* P;   //����ռ䱣��ͼ������

void ReadBmp(char *a);
void Logarithmic(char* b);
void Histogram(char* b);

int main()
{
    char *filename;
    filename = (char*) malloc(100 * sizeof(char));
    printf("�������ļ�·����");
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
        printf("���ļ�ʧ��\n");
        exit(-1);
    }
    // ��ȡͷ��Ϣ
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);   //��ȡ�ļ�ͷ
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);   //��ȡ��Ϣͷ
    bitCount = infoHeader.biBitCount;//ɫ�ʵ�λ��
    if (bitCount == 16)
        exit(-1);
    if (bitCount < 16)
    {
        Clr = infoHeader.biClrUsed ? infoHeader.biClrUsed : 1 << bitCount;
        if (Clr>256)
            Clr = 0;
    }
    // ��ȡ��ɫ��
    int i = 0, j = 0;
    if (Clr > 0)
    {
        QUAD = (RGBQUAD*)malloc(sizeof(RGBQUAD)*Clr);
        fread(QUAD, sizeof(RGBQUAD)*Clr, 1, pFile);
        for (i = 0; i < Clr; i++)
        {
            QUAD[i].rgbRed = QUAD[i].rgbBlue = QUAD[i].rgbGreen =
                    (BYTE)(0.3 * QUAD[i].rgbRed + 0.59 * QUAD[i].rgbGreen + 0.11 * QUAD[i].rgbBlue);  //�Ҷ�ͼ����������y
        }
    }
    pW = infoHeader.biWidth;     //ͼ�����ݵĿ��
    pH = infoHeader.biHeight;    //ͼ�����ݵĸ߶�

    pSize = ((pW * bitCount)>>3)*pH;  //ͼ�����ݴ�С
    LineSize = ((pW * bitCount)>> 3);  //�����ݴ�С
    P = (BYTE*)malloc(pSize);   //����ռ䱣��ͼ������
    fread(P, pSize, 1, pFile);     //����ͼ������
}

void Logarithmic(char* b)
{
    int max=0,index;
    if (Clr == 0)
        for (int i = 0; i < pH; i++)//��
            if (bitCount == 24)
            {
                for (int j = 0; j < pW*3; j = j + 3)// ��
                {
                    int n = i*LineSize + j;
                    index = P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //�ֱ���r,g,b����
                    if(index > max)
                        max = index;
                }
            }
//    printf("%d\n",max);
    for (int i = 0; i < pH; i++)//��
        if (bitCount == 24)
            for (int j = 0; j < pW * 3; j = j + 3)// ��
            {
                int n = i * LineSize + j;
                P[n] = P[n + 1] = P[n + 2] = (BYTE)(255 * log10(P[n] + 1) / log10(max + 1));
            }
    FILE* dFile = fopen(b, "wb");//����Ŀ���ļ�
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //д���ļ�ͷ
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //д���ļ���Ϣͷ
    if (QUAD)
        fwrite(QUAD, sizeof(RGBQUAD)* Clr, 1, dFile);
    //д��ͼ������
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
        for (int i = 0; i < pH; i++)//��
            if (bitCount == 24)
                for (int j = 0; j < pW * 3; j = j + 3)// ��
                {
                    int n = i * LineSize + j;
                    now = P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //�ֱ���r,g,b����
                    grey[P[n]]++;       //�Ҷ�ֱ��ͼ
                    if(now > Max)
                        Max = now;           //�ҵ������С
                    else if(now < Min)
                        Min = now;
                }
    for(int i = 0; i < Max; i++)
        grey[i] = grey[i] / (pH * pW);
//    printf("max=%d,min=%d\n", Max, Min);
    //�ۼ�ֱ��ͼ
    double greyadd[256]={0};
    for(int i = 0; i < 253; i++)
        for(int j = 0; j <= i; j++)
            greyadd[i] += grey[j];
//    printf("%f %f", greyadd[0], greyadd[1]);
//    printf("\n%f\n%f\n%f", grey[0], grey[1], grey[253]);

    //��ֵ
    if (Clr == 0)
        for (int i = 0; i < pH; i++)//��
            if (bitCount == 24)
                for (int j = 0; j < pW * 3; j = j + 3)// ��
                {
                    int n = i * LineSize + j;
                    P[n] = P[n + 1] = P[n + 2] = (BYTE)(Min + (Max - Min) * greyadd[P[n]]);
                }

    FILE* dFile = fopen(b, "wb");//����Ŀ���ļ�
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //д���ļ�ͷ
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //д���ļ���Ϣͷ
    if (QUAD)
        fwrite(QUAD, sizeof(RGBQUAD) * Clr, 1, dFile);
    //д��ͼ������
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