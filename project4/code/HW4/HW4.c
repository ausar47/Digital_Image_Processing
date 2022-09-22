
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#define pi 3.14
BITMAPFILEHEADER fileHeader;// λͼͷ�ļ�
BITMAPINFOHEADER infoHeader;// λͼ��Ϣͷ
FILE* pFile;
int Clr = 0;
WORD bitCount;
LONG pW;     //ͼ�����ݵĿ��
LONG pH;    //ͼ�����ݵĸ߶�
int cx, cy; //�������Ŀ�ȡ��߶����꣬��תʱ��ԭ��
RGBQUAD* QUAD = NULL;
int size;   //ͼ��������
typedef struct tagIMAGEP  //ÿ������
{
    BYTE blue;
    BYTE green;
    BYTE red;
}P;
P* imgp;

void ReadBmp(char* a);
void translation(int x, int y);
void mirror(int sx, int sy);
void shear(double angle);
void scale(float x, float y);
double xAfterRot(int x, int y, double theta);
double yAfterRot(int x, int y, double theta);
void tXY(int *x, int *y, double theta);
void rotation(double theta);

int main()
{
    char* filename;
    filename = (char*)malloc(100 * sizeof(char));
    printf("�������ļ�·����");
    scanf("%s", filename);
    ReadBmp(filename);
    translation(90, -90);
    mirror(1, -1);
    rotation(45.0 / 180 * pi);
    shear(25.0 / 180 * pi);
    scale(5, 5);
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
    fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), pFile);   //��ȡ�ļ�ͷ
    fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), pFile);   //��ȡ��Ϣͷ
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
    if (pW % 4 == 0)
        pW = pW;
    else
        pW = (pW / 4 + 1) * 4;
    pH = infoHeader.biHeight;    //ͼ�����ݵĸ߶�
    cx = pW / 2;
    cy = pH / 2;
    size = infoHeader.biSizeImage / 3;  //ͼ��������
    imgp = (P*)malloc(sizeof(P) * size);
    fread(imgp, 1, sizeof(P) * size, pFile);     //����ͼ������
}

void translation(int x, int y) 
{
    int moveX, moveY, saveX, saveY;
    //��4ȡ��
    if (x % 4 == 0)
        moveX = x;
    else
        moveX = x / 4 * 4;
    moveY = y;
    saveX = moveX; saveY = moveY;
    if (moveX < 0)
        moveX = -moveX;
    if (moveY < 0)
        moveY = -moveY;
    //������
    int nH = pH + moveY;
    int nW = pW + moveX;
    moveX = saveX; moveY = saveY;
    //��������
    int newSize = nH * nW;
    P *Q = (P*)malloc(sizeof(P) * newSize);
    memset(Q, 0, sizeof(P) * newSize);
    //����ÿ������
    if (moveY < 0)
    {
        moveY = -moveY;
        for (int i = 0; i < nH; i++)
        {
            for (int j = 0; j < nW; j++)
            {
                if (nH - i <= moveY)
                {
                    Q[i * nW + j].red = Q[i * nW + j].blue = Q[i * nW + j].green = 0;
                    continue;
                }
                if (j < moveX || nW - j <= -moveX)
                    Q[i * nW + j].red = Q[i * nW + j].blue = Q[i * nW + j].green = 0;
                else if (moveX >= 0)
                    Q[i * nW + j] = imgp[i * pW + (j - moveX)];
                else if (moveX < 0)
                    Q[i * nW + j] = imgp[i * pW + j];
            }
        }
    }
    else
    {
        for (int i = 0; i < nH; i++)
        {
            for (int j = 0; j < nW; j++)
            {
                if (i < moveY)
                {
                    Q[i * nW + j].red = Q[i * nW + j].blue = Q[i * nW + j].green = 0;
                    continue;
                }
                if (j < moveX || nW - j <= -moveX)
                    Q[i * nW + j].red = Q[i * nW + j].blue = Q[i * nW + j].green = 0;
                else if (moveX >= 0)
                    Q[i * nW + j] = imgp[(i - moveY) * pW + (j - moveX)];
                else if (moveX < 0)
                    Q[i * nW + j] = imgp[(i - moveY) * pW + j];              
            }
        }
    }
        
    //�����µ��ļ�
    FILE* fpo1;
    fpo1 = fopen("trans.bmp", "wb");
    //�µ��ļ�ͷ��Ϣͷ
    BITMAPFILEHEADER nHead = fileHeader;
    BITMAPINFOHEADER newInfo = infoHeader;
    //�µ�ͼ���С
    nHead.bfSize = (DWORD)(fileHeader.bfSize - size * 3 + newSize * 3);
    //�µ�ͼ��߶ȺͿ��
    newInfo.biHeight = (DWORD)nH;
    newInfo.biWidth = (DWORD)nW;
    newInfo.biSizeImage = (DWORD)(newSize * 3);
    fwrite(&nHead, 1, sizeof(BITMAPFILEHEADER), fpo1);
    fwrite(&newInfo, 1, sizeof(BITMAPINFOHEADER), fpo1);
    if (QUAD)
        fwrite(QUAD, 1, sizeof(RGBQUAD) * Clr, fpo1);
    fwrite(Q, 1, sizeof(P) * (newSize), fpo1);
    fclose(fpo1);
    //�ͷſռ�
    free(Q);
}

void mirror(int sx, int sy) {
    P* Q = (P*)malloc(sizeof(P) * size);
    memset(Q, 0, sizeof(P) * size);
    if (sx == 1 && sy == -1)
        for (int i = 0; i < pH; i++) 
            for (int j = 0; j < pW; j++) 
                Q[i * pW + j] = imgp[i * pW + pW - 1 - j];  //horizontal mirror
    else if (sx == -1 && sy == 1)
        for (int i = 0; i < pH; i++)
            for (int j = 0; j < pW; j++)
                Q[i * pW + j] = imgp[(pH - i - 1) * pW + j];    //vertical mirror
    else if (sx == -1 && sy == -1)
        for (int i = 0; i < pH; i++)
            for (int j = 0; j < pW; j++)
                Q[i * pW + j] = imgp[(pH - i - 1) * pW + pW - 1 - j];   //horizontal && vertical mirror
    //�����ļ�
    FILE* fpo1;
    fpo1 = fopen("mirr.bmp", "wb");
    fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fpo1);
    fwrite(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fpo1);
    fwrite(Q, 1, sizeof(P) * size, fpo1);
    fclose(fpo1);
    free(Q);
}

void shear(double angle) 
{
    int nH, nW, newSize;
    P* Q;
    int* tK;

    nW = pW;

    nH = pH + tan(angle) * pW;
    newSize = nW * nH;
    Q = (P*)malloc(sizeof(P) * newSize);
    tK = (int*)malloc(sizeof(int) * newSize);
    //�ж����������Ƿ��Ѿ�����Ϣ�ı������
    for (int i = 0; i < newSize; i++)
        tK[i] = 1;
    //ԭ����
    for (int i = 0; i < pH; i++) 
    {
        for (int j = 0; j < pW; j++) 
        {
            //��任��ĺ������꣬����ֵ��Ӧ������
            int tmpX = j;
            int tmpY = (int)(i + tan(angle) * j);
            Q[tmpY * nW + tmpX] = imgp[i * pW + j];
            //����Ϣ����Ϊ0
            tK[tmpY * nW + tmpX] = 0;
        }
    }
    //ûͼ���λ�ö��ð�ɫ���
    for (int i = 0; i < newSize; i++)
        if (tK[i] == 1)
            Q[i].red = Q[i].green = Q[i].blue = 255;
    //���ļ�
    FILE* fpo1;
    fpo1 = fopen("shear.bmp", "wb");
    BITMAPFILEHEADER nHead = fileHeader;
    BITMAPINFOHEADER newInfo = infoHeader;
    nHead.bfSize = (DWORD)(nHead.bfSize - size * 3 + newSize * 3);
    newInfo.biHeight = (DWORD)nH;
    newInfo.biWidth = (DWORD)nW;
    newInfo.biSizeImage = (DWORD)(newSize * 3);
    fwrite(&nHead, 1, sizeof(BITMAPFILEHEADER), fpo1);
    fwrite(&newInfo, 1, sizeof(BITMAPINFOHEADER), fpo1);
    fwrite(Q, 1, sizeof(P) * (newSize), fpo1);
    fclose(fpo1);
    free(Q);
    free(tK);
}

void scale(float x, float y) 
{
    //�µ�ͼ��߶ȺͿ��
    int nH = (int)(pH * y);
    int tW = (int)(pW * x);
    int nW;
    //��4ȡ��
    if (tW % 4 == 0)
        nW = tW;
    else
        nW = (tW / 4 + 1) * 4;
    //�µ�ͼ���С
    int newSize = nH * nW;
    P* Q = (P*)malloc(sizeof(P) * newSize);
    int* tK = (int*)malloc(sizeof(int) * newSize);
    //�Ȱ�����ͼ����0
    memset(Q, 0, sizeof(P) * newSize);
    memset(tK, 0, sizeof(int) * newSize);
    //ɨ����Ҫ����ͼ��
    for (int i = 0; i < nH; i++) 
    {
        for (int j = 0; j < nW; j++) 
        {
            float tX, tY;
            int intX, intY;
            //��scale֮�����ص��ԭ����
            tX = (j * 1.0 / x);
            tY = (i * 1.0 / y);
            intX = (int)tX;
            intY = (int)tY;

            if (tX >= pW || tY >= pH)
                Q[i * nW + j].red = Q[i * nW + j].green = Q[i * nW + j].blue = 255;
            else if (tX - intX > 0 || tY - intY > 0)    //���
            {
                Q[i * nW + j].red = Q[i * nW + j].green = Q[i * nW + j].blue = 255;
                tK[i * nW + j] = 1;
            }
            else
                Q[i * nW + j] = imgp[intY * pW + intX];
        }
    }
    //ˮƽscale
    for (int i = 0; i < nH; i++) 
    {
        int last = -1;
        for (int j = 0; j < nW; j++) 
        {
            if (tK[i * nW + j] == 0) 
            {
                last = j;
                continue;
            }
            else 
            {
                int k = j;
                //�ҵ���һ����ֵ�ĵط�
                while (k < nW && tK[i * nW + k] == 1)
                    k++;
                if (k >= nW) 
                {			//�����
                    if (last == -1)
                        break;
                    Q[i * nW + j] = Q[i * nW + last];
                }
                else 
                {
                    if (last == -1)	//��ͷ
                        Q[i * nW + j] = Q[i * nW + k];
                    else 
                    {
                        float coe = (j - last) * 1.0 / (k - last);
                        Q[i * nW + j].red = (BYTE)(((int)Q[i * nW + k].red - (int)Q[i * nW + last].red) * coe + (int)Q[i * nW + last].red);
                        Q[i * nW + j].blue = (BYTE)(((int)Q[i * nW + k].blue - (int)Q[i * nW + last].blue) * coe + (int)Q[i * nW + last].blue);
                        Q[i * nW + j].green = (BYTE)(((int)Q[i * nW + k].green - (int)Q[i * nW + last].green) * coe + (int)Q[i * nW + last].green);
                    }
                }
            }
            tK[i * nW + j] = 0;
        }
    }
    //��ֱscale
    for (int i = 0; i < nW; i++) 
    {
        int last = -1;
        for (int j = 0; j < nH; j++) 
        {
            if (tK[j * nW + i] == 0) 
            {
                last = j;
                continue;
            }
            else 
            {
                int k = j;
                //find next position which has value
                while (k < nH && tK[k * nW + i] == 1) 
                {
                    k++;
                }
                if (k >= nH) 
                {			//�����
                    if (last == -1)
                        break;
                    Q[j * nW + i] = Q[last * nW + i];
                }
                else 
                {
                    if (last == -1) 
                    {		//��ͷ
                        Q[j * nW + i] = Q[k * nW + i];
                    }
                    else 
                    {
                        float coe = (j - last) * 1.0 / (k - last);
                        Q[j * nW + i].red = (BYTE)(((int)Q[k * nW + i].red - (int)Q[last * nW + i].red) * coe + (int)Q[last * nW + i].red);
                        Q[j * nW + i].blue = (BYTE)(((int)Q[k * nW + i].blue - (int)Q[last * nW + i].blue) * coe + (int)Q[last * nW + i].blue);
                        Q[j * nW + i].green = (BYTE)(((int)Q[k * nW + i].green - (int)Q[last * nW + i].green) * coe + (int)Q[last * nW + i].green);
                    }
                }
            }
            tK[j * nW + i] = 0;
        }
    }
    //���ļ�
    FILE* fpo1;
    fpo1 = fopen("scale.bmp", "wb");
    BITMAPFILEHEADER nHead = fileHeader;
    BITMAPINFOHEADER newInfo = infoHeader;
    nHead.bfSize = (DWORD)(nHead.bfSize - size * 3 + newSize * 3);
    newInfo.biHeight = (DWORD)nH;
    newInfo.biWidth = (DWORD)nW;
    newInfo.biSizeImage = (DWORD)(newSize * 3);
    fwrite(&nHead, 1, sizeof(BITMAPFILEHEADER), fpo1);
    fwrite(&newInfo, 1, sizeof(BITMAPINFOHEADER), fpo1);
    fwrite(Q, 1, sizeof(P) * (newSize), fpo1);
    fclose(fpo1);
    free(Q);
    free(tK);
}

//���괦��
double xAfterRot(int x, int y, double theta) 
{
    x = x - cx;
    y = y - cy;
    double result = x * cos(theta) - y * sin(theta);
    return result += cx;
}
double yAfterRot(int x, int y, double theta) 
{
    x = x - cx;
    y = y - cy;
    double result = x * sin(theta) + y * cos(theta);
    return result += cy;
}

void tXY(int *x, int *y, double theta) 
{
    double tmpX[4], tmpY[4];
    double minX = 999999, minY = 999999, maxX = 0, maxY = 0;

    tmpX[0] = xAfterRot(0, 0, theta);
    tmpY[0] = yAfterRot(0, 0, theta);
    tmpX[1] = xAfterRot(pW, 0, theta);
    tmpY[1] = yAfterRot(pW, 0, theta);
    tmpX[2] = xAfterRot(pW, pH, theta);
    tmpY[2] = yAfterRot(pW, pH, theta);
    tmpX[3] = xAfterRot(0, pH, theta);
    tmpY[3] = yAfterRot(0, pH, theta);

    for (int i = 0; i < 4; i++) 
    {
        minX = min(tmpX[i], minX);
        maxX = max(tmpX[i], maxX);
        minY = min(tmpY[i], minY);
        maxY = max(tmpY[i], maxY);
    }
    *x = (int)(maxX - minX + 2);
    *y = (int)(maxY - minY + 2);
}

void rotation(double theta) 
{
    int nH;
    int nW;
    tXY(&nW, &nH, theta);
    if (nW % 4 != 0)
        nW = (nW / 4 + 1) * 4;
    int newSize = nH * nW;
    P* Q = (P*)malloc(sizeof(P) * newSize);
    memset(Q, 0, sizeof(P) * newSize);
    int* tK = (int*)malloc(sizeof(int) * newSize);
    int* minIndexX = (int*)malloc(sizeof(int) * nH);
    int* maxIndexX = (int*)malloc(sizeof(int) * nH);
    int* minIndexY = (int*)malloc(sizeof(int) * nW);
    int* maxIndexY = (int*)malloc(sizeof(int) * nW);

    for (int i = 0; i < nH; i++)
        minIndexX[i] = 999999;
    for (int i = 0; i < nW; i++)
        minIndexY[i] = 999999;
    memset(maxIndexX, 0, sizeof(int) * nH);
    memset(maxIndexY, 0, sizeof(int) * nW);

    for (int i = 0; i < newSize; i++)
        tK[i] = 1;

    int moveX = (nW - pW) / 2;
    int moveY = (nH - pH) / 2;

    for (int i = 0; i < pH; i++) 
    {
        for (int j = 0; j < pW; j++) 
        {
            int tmpX = (int)xAfterRot(j, i, theta) + moveX;
            int tmpY = (int)yAfterRot(j, i, theta) + moveY;
            Q[tmpY * nW + tmpX] = imgp[i * pW + j];
            tK[tmpY * nW + tmpX] = 0;
            minIndexX[tmpY] = min(tmpX, minIndexX[tmpY]);
            maxIndexX[tmpY] = max(tmpX, maxIndexX[tmpY]);
            minIndexY[tmpX] = min(tmpY, minIndexY[tmpX]);
            maxIndexY[tmpX] = max(tmpY, maxIndexY[tmpX]);
        }
    }

    //first do the horizontal interpolation
    for (int i = 0; i < nH; i++) 
    {
        int last = -1;
        for (int j = 0; j < nW; j++) 
        {
            if (j < minIndexX[i] || j > maxIndexX[i]) 
            {
                Q[i * nW + j].red = 255;
                Q[i * nW + j].green = 255;
                Q[i * nW + j].blue = 255;
                continue;
            }
            if (tK[i * nW + j] == 0) 
            {
                last = j;
                continue;
            }
            else 
            {
                int k = j;
                //find next position which has value
                while (k < nW && tK[i * nW + k] == 1) 
                    k++;
                if (k >= nW) 
                {			//in the last position of the line
                    if (last == -1)
                        break;
                    Q[i * nW + j] = Q[i * nW + last];
                }
                else 
                {
                    if (last == -1) 		//in the first position of the line
                        Q[i * nW + j] = Q[i * nW + k];
                    else 
                    {				//in the middle
                        float coe = (j - last) * 1.0 / (k - last);
                        Q[i * nW + j].red = (BYTE)(((int)Q[i * nW + k].red - (int)Q[i * nW + last].red) * coe + (int)Q[i * nW + last].red);
                        Q[i * nW + j].blue = (BYTE)(((int)Q[i * nW + k].blue - (int)Q[i * nW + last].blue) * coe + (int)Q[i * nW + last].blue);
                        Q[i * nW + j].green = (BYTE)(((int)Q[i * nW + k].green - (int)Q[i * nW + last].green) * coe + (int)Q[i * nW + last].green);
                    }
                }
            }
            tK[i * nW + j] = 0;
        }
    }
    //then do the vertical interpolation
    for (int i = 0; i < nW; i++) 
    {
        int last = -1;
        for (int j = 0; j < nH; j++) 
        {
            if (j < minIndexY[i] || j > maxIndexY[i]) 
            {
                continue;
            }
            if (tK[j * nW + i] == 0) 
            {
                last = j;
                continue;
            }
            else 
            {
                int k = j;
                //find next position which has value
                while (k < nH && tK[k * nW + i] == 1) 
                {
                    k++;
                }
                if (k >= nH) 
                {			//in the last position of the line
                    if (last == -1)
                        break;
                    Q[j * nW + i] = Q[last * nW + i];
                }
                else 
                {
                    if (last == -1) 
                    {		//in the first position of the line
                        Q[j * nW + i] = Q[k * nW + i];
                    }
                    else 
                    {				//in the middle
                        float coe = (j - last) * 1.0 / (k - last);
                        Q[j * nW + i].red = (BYTE)(((int)Q[k * nW + i].red - (int)Q[last * nW + i].red) * coe + (int)Q[last * nW + i].red);
                        Q[j * nW + i].blue = (BYTE)(((int)Q[k * nW + i].blue - (int)Q[last * nW + i].blue) * coe + (int)Q[last * nW + i].blue);
                        Q[j * nW + i].green = (BYTE)(((int)Q[k * nW + i].green - (int)Q[last * nW + i].green) * coe + (int)Q[last * nW + i].green);
                    }
                }
            }
            tK[j * nW + i] = 0;
        }
    }

    //�������ɵ�����д���ļ�
    FILE* fpo1;
    fpo1 = fopen("rotat.bmp", "wb");
    BITMAPFILEHEADER nHead = fileHeader;
    BITMAPINFOHEADER newInfo = infoHeader;
    nHead.bfSize = (DWORD)(nHead.bfSize - size * 3 + newSize * 3);
    newInfo.biHeight = (DWORD)nH;
    newInfo.biWidth = (DWORD)nW;
    newInfo.biSizeImage = (DWORD)(newSize * 3);
    fwrite(&nHead, 1, sizeof(BITMAPFILEHEADER), fpo1);
    fwrite(&newInfo, 1, sizeof(BITMAPINFOHEADER), fpo1);
    fwrite(Q, 1, sizeof(P) * (newSize), fpo1);
    fclose(fpo1);
    free(Q);
    free(tK);
    free(minIndexX);
    free(maxIndexX);
    free(minIndexY);
    free(maxIndexY);
}