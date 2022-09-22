#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
BITMAPFILEHEADER fileHeader;// λͼͷ�ļ�
BITMAPINFOHEADER infoHeader;// λͼ��Ϣͷ
FILE* pFile;
int Clr = 0;
WORD bitCount;
LONG pW;     //ͼ�����ݵĿ��
LONG pH;    //ͼ�����ݵĸ߶�
RGBQUAD* QUAD = NULL;
int pSize;  //ͼ�����ݴ�С
int LineSize;  //�����ݴ�С
BYTE* P;   //����ռ䱣��ͼ������
void ReadBmp(char* a);
void imf(char* b);
void lie(char* b);

int main()
{
	char* filename;
	filename = (char*)malloc(100 * sizeof(char));
	printf("�������ļ�·����");
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
		if (Clr > 256)
			Clr = 0;
	}
	// ��ȡ��ɫ��
	int i = 0, j = 0;
	if (Clr > 0)
	{
		QUAD = (RGBQUAD*)malloc(sizeof(RGBQUAD) * Clr);
		fread(QUAD, sizeof(RGBQUAD) * Clr, 1, pFile);
		for (i = 0; i < Clr; i++)
		{
			QUAD[i].rgbRed = QUAD[i].rgbBlue = QUAD[i].rgbGreen =
				(BYTE)(0.3 * QUAD[i].rgbRed + 0.59 * QUAD[i].rgbGreen + 0.11 * QUAD[i].rgbBlue);  //�Ҷ�ͼ����������y
		}
	}
	pW = infoHeader.biWidth;     //ͼ�����ݵĿ��
	pH = infoHeader.biHeight;    //ͼ�����ݵĸ߶�

	pSize = pW * 3 * pH;  //ͼ�����ݴ�С
	printf("%d %d ", pW, pH);
	printf("%d\n", pSize);
	LineSize = pW * 3;  //�����ݴ�С
	P = (BYTE*)malloc(pSize);   //����ռ䱣��ͼ������
	fread(P, pSize, 1, pFile);     //����ͼ������
}

//���Ծ�ֵ�˲�
void imf(char* b)
{
	//��תgreyͼ��
	if (Clr == 0)
	{
		for (int i = 0; i < pH; i++)//��
		{
			if (bitCount == 24)
			{
				for (int j = 0; j < pW * 3; j = j + 3)// ��
				{
					int n = i * LineSize + j;
					P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //�ֱ���r,g,b����
				}
			}
		}
	}
	//��ֵ�˲�
	for (int i = 1; i < pH - 1; i++)//��
	{
		for (int j = 3; j < (pW - 1) * 3; j = j + 3)// ��
		{
			//int n = i*LineSize + j;
			P[i * LineSize + j] = P[i * LineSize + j + 1] = P[i * LineSize + j + 2] = (BYTE)((P[i * LineSize + j] + P[i * LineSize + j + 3] + P[i * LineSize + j - 3] + P[(i - 1) * LineSize + j] + P[(i - 1) * LineSize + j + 3] + P[(i - 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j + 3] + P[(i + 1) * LineSize + j]) / 9);  //����9�����ص�ľ�ֵ

		}
	}
	FILE* dFile = fopen(b, "wb");//����Ŀ���ļ�

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //д���ļ�ͷ

	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //д���ļ���Ϣͷ
	if (QUAD)
	{
		fwrite(QUAD, sizeof(RGBQUAD) * Clr, 1, dFile);
	}
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

void lie(char* b)
{
	//��תgreyͼ��
	if (Clr == 0)
	{
		for (int i = 0; i < pH; i++)//��
		{
			if (bitCount == 24)
			{
				for (int j = 0; j < pW * 3; j = j + 3)// ��
				{
					int n = i * LineSize + j;
					P[n] = P[n + 1] = P[n + 2] = (BYTE)(0.299 * P[n] + 0.587 * P[n + 1] + 0.114 * P[n + 2]); //�ֱ���r,g,b����

				}
			}

		}
	}
	//������˹
	int w; //��
	for (int i = 1; i < pH - 1; i++)//��
	{
		for (int j = 3; j < (pW - 1) * 3; j = j + 3)// ��
		{
			//int n = i*LineSize + j;
			w = P[i * LineSize + j + 3] + P[i * LineSize + j - 3] + P[(i - 1) * LineSize + j] + P[(i - 1) * LineSize + j + 3] + P[(i - 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j - 3] + P[(i + 1) * LineSize + j + 3] + P[(i + 1) * LineSize + j] - P[i * LineSize + j] * 8;
			w *= 0.1;
			//Ȩ����������

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
	FILE* dFile = fopen(b, "wb");//����Ŀ���ļ�

	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, dFile);   //д���ļ�ͷ

	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, dFile);  //д���ļ���Ϣͷ
	if (QUAD)
	{
		fwrite(QUAD, sizeof(RGBQUAD) * Clr, 1, dFile);
	}
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