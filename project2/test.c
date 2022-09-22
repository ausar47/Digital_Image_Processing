#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<Windows.h>

int main()
{
    /*��������*/
    FILE *fpBMP,*binarization, *erosion, *dilation, *opening, *closing;//Դ�ļ�fpBMP,Ŀ���ļ�fpTwoValue
    char filename1[20], filename2[20], filename3[20], filename4[20], filename5[20], filename6[20];

    BITMAPFILEHEADER *fileHeader;//λͼ�ļ�ͷ
    BITMAPINFOHEADER *infoHeader;//λͼ��Ϣͷ
    RGBQUAD *ipRGB;//��ɫ��

    int i,j,k=0;
    unsigned char *a;//�洢Դͼÿ������ֵ
    unsigned char b;//�洢ÿ�����صĻҶ�ֵ���ֵ
    unsigned char *c;//�洢ÿ�����صĶ�ֵ


    printf("����ͼ���ļ�����");
    scanf("%s", filename1);
    if ((fpBMP = fopen(filename1, "rb")) == NULL)
    {
        printf("��ͼƬʧ��");
        exit(0);
    }

    strcpy(filename2, "binarization.bmp");
    strcpy(filename3, "erosion.bmp");
    strcpy(filename4, "dilation.bmp");
    strcpy(filename5, "opening.bmp");
    strcpy(filename6, "closing.bmp");
    if ((binarization = fopen(filename2, "wb")) == NULL)
    {
        printf("����ͼƬʧ��");
        exit(0);
    }
    if ((erosion = fopen(filename3, "wb")) == NULL)
    {
        printf("����ͼƬʧ��");
        exit(0);
    }
    if ((dilation = fopen(filename4, "wb")) == NULL)
    {
        printf("����ͼƬʧ��");
        exit(0);
    }
    if ((opening = fopen(filename5, "wb")) == NULL)
    {
        printf("����ͼƬʧ��");
        exit(0);
    }
    if ((closing = fopen(filename6, "wb")) == NULL)
    {
        printf("����ͼƬʧ��");
        exit(0);
    }
    /********************************************************************/

    /*����λͼ�ļ�ͷ����Ϣͷ����ɫ��*/
    fileHeader=(BITMAPFILEHEADER *)malloc(sizeof(BITMAPFILEHEADER));
    infoHeader=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
    ipRGB=(RGBQUAD *)malloc(2*sizeof(RGBQUAD));

    /*����Դλͼ�ļ�ͷ����Ϣͷ*/
    fread(fileHeader,sizeof(BITMAPFILEHEADER),1,fpBMP);
    fread(infoHeader,sizeof(BITMAPINFOHEADER),1,fpBMP);
    //���������������BMPͼ�����Ϣͷ���ļ�ͷ����fileHeader��infoHeader���������Ը���fileHeader��infoHeader�õ�ͼ��ĸ������ԡ�
//    printf("ԭʼͼƬÿ�����ص�λ��:%d\n" ,infoHeader->biBitCount);
//    printf("ԭʼͼƬÿ��������������ƫ��:%d\n" ,fileHeader->bfOffBits);
    //�޸���Ϣͷ
    //��Ϣͷ����11���֣��ҶȻ�ʱ��Ҫ�޸�4����
    infoHeader->biBitCount=8;//ת���ɶ�ֵͼ����ɫ�����24λ��Ϊ8λ
    infoHeader->biSizeImage=((infoHeader->biWidth + 3) / 4) * 4 * infoHeader->biHeight;//ÿ�����������ֽڱ�Ϊ���ֽڣ�ͬʱÿ������Ҫ���ֽڶ���
    infoHeader->biClrUsed=2;//��ɫ��������������ֵͼΪ2
    infoHeader->biClrImportant=0;//��Ҫ��ɫ����Ϊ0����ʾ����Ҫ
    //�޸��ļ�ͷ
    //�ļ�ͷ����5���֣��ҶȻ�ʱ��Ҫ�޸�������
    fileHeader->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD);//������ƫ�����������ļ�ͷ����Ϣͷ��������Ĵ�С֮��
    fileHeader->bfSize=fileHeader->bfOffBits+infoHeader->biSizeImage;//�ļ���С������ƫ����������������С
    ipRGB[1].rgbBlue=ipRGB[1].rgbGreen=ipRGB[1].rgbRed=ipRGB[1].rgbReserved=0;//��ɫ����ɫΪ��ɫ��Ӧ������Ϊ0
    ipRGB[0].rgbBlue=ipRGB[0].rgbGreen=ipRGB[0].rgbRed=200;//��ɫ��Ӧ������Ϊ150-255

//    printf("�޸ĺ��ͼƬÿ�����ص�λ��:%d\n" ,infoHeader->biBitCount);
//    printf("�޸ĺ��ͼƬÿ����������ƫ��:%d\n" ,fileHeader->bfOffBits);

    /********************************************************************/

    //��ȡBMPͼ�����Ϣͷ���ļ�ͷ��BMP��ɫ�嵽�½���ͼƬ
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, binarization);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, binarization);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, binarization);

    /*����ɫͼתΪ��ֵͼ*/
    a=(unsigned char *)malloc((infoHeader->biWidth*3+3)/4*4);//������a����Դͼÿ��������ռ��С�Ŀռ�,�������ֽڶ�������
    c=(unsigned char *)malloc((infoHeader->biWidth+3)/4*4);//������c����Ŀ��ͼÿ��������ռ��С�Ŀռ�,ͬ�����ֽڶ���

    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<((infoHeader->biWidth*3+3)/4*4);j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            fread(a+j,1,1,fpBMP);//��Դͼÿ�е�ÿһ���ֽڶ������a��ָ����ڴ�ռ�
            }
        for(j=0;j<infoHeader->biWidth;j++)//ѭ�����ؿ�ȴ�,�Ͳ������������ֽ����λ
            {
            b=(int)(0.114*(float)a[k]+0.587*(float)a[k+1]+0.299*(float)a[k+2]);//a��ÿ�����ֽڷֱ����BGR���������ϲ�ͬȨֵת��Ϊ�Ҷ�ֵ
            if(120>(int)b) //���Ҷ�ֵת��Ϊ��ֵ
                b=1;
            else b=0;
            c[j]=b;   //�洢ÿ�еĶ�ֵ
            k+=3;
            }
        fwrite(c,(infoHeader->biWidth+3)/4*4, 1, binarization);//����ֵ�������ֽ����д���ļ�,���λû�г�ʼ��,Ϊ���ֵ
        k=0;
        }
    fclose(binarization);
    /********************************************************************/
    if ((binarization = fopen(filename2, "rb")) == NULL)
    {
        printf("��ͼƬʧ��");
        exit(0);
    }

    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, erosion);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, erosion);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, erosion);
    fseek(binarization, fileHeader->bfOffBits, 0);
    unsigned char map[infoHeader->biHeight][(infoHeader->biWidth+3)/4*4];
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            fread(map[i]+j,1,1,binarization);//��Դͼÿ�е�ÿһ���ֽڶ������a��ָ����ڴ�ռ�
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            if (j == 0 && i == 0)
            {
                if (map[i][j+1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            else if (j == 0 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 0 || map[i][j+1] == 0)
                    map[i][j] = 2;
            }
            else if (j == 0)
            {
                if (map[i][j+1] == 0 || map[i+1][j] == 0 || map[i-1][j] == 0)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == 0)
            {
                if (map[i][j-1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 0 || map[i][j-1] == 0)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1)
            {
                if (map[i][j-1] == 0 ||  map[i+1][j] == 0 || map[i-1][j] == 0)
                    map[i][j] = 2;
            }
            else if (i == 0)
            {
                if (map[i][j-1] == 0 || map[i][j+1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            else if (i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 0 || map[i][j-1] == 0 || map[i][j+1] == 0)
                    map[i][j] = 2;
            }
            else
            {
                if (map[i-1][j] == 0 || map[i][j-1] == 0 || map[i][j+1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            if (map[i][j] == 2)
                map[i][j] = 0;
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, erosion);
    fclose(erosion);

    /********************************************************************/
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, dilation);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, dilation);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, dilation);
    fseek(binarization, fileHeader->bfOffBits, 0);
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            fread(map[i]+j,1,1,binarization);//��Դͼÿ�е�ÿһ���ֽڶ������a��ָ����ڴ�ռ�
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            if (j == 0 && i == 0)
            {
                if (map[i][j+1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            else if (j == 0 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 1 || map[i][j+1] == 1)
                    map[i][j] = 2;
            }
            else if (j == 0)
            {
                if (map[i][j+1] == 1 || map[i+1][j] == 1 || map[i-1][j] == 1)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == 0)
            {
                if (map[i][j-1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 1 || map[i][j-1] == 1)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1)
            {
                if (map[i][j-1] == 1 ||  map[i+1][j] == 1 || map[i-1][j] == 1)
                    map[i][j] = 2;
            }
            else if (i == 0)
            {
                if (map[i][j-1] == 1 || map[i][j+1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            else if (i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 1 || map[i][j-1] == 1 || map[i][j+1] == 1)
                    map[i][j] = 2;
            }
            else
            {
                if (map[i-1][j] == 1 || map[i][j-1] == 1 || map[i][j+1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            if (map[i][j] == 2)
                map[i][j] = 1;
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, dilation);
    fclose(dilation);
    /********************************************************************/
    if ((erosion = fopen(filename3, "rb")) == NULL)
    {
        printf("��ͼƬʧ��");
        exit(0);
    }

    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, opening);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, opening);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, opening);
    fseek(erosion, fileHeader->bfOffBits, 0);
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            fread(map[i]+j,1,1,erosion);//��Դͼÿ�е�ÿһ���ֽڶ������a��ָ����ڴ�ռ�
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            if (j == 0 && i == 0)
            {
                if (map[i][j+1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            else if (j == 0 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 1 || map[i][j+1] == 1)
                    map[i][j] = 2;
            }
            else if (j == 0)
            {
                if (map[i][j+1] == 1 || map[i+1][j] == 1 || map[i-1][j] == 1)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == 0)
            {
                if (map[i][j-1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 1 || map[i][j-1] == 1)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1)
            {
                if (map[i][j-1] == 1 ||  map[i+1][j] == 1 || map[i-1][j] == 1)
                    map[i][j] = 2;
            }
            else if (i == 0)
            {
                if (map[i][j-1] == 1 || map[i][j+1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            else if (i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 1 || map[i][j-1] == 1 || map[i][j+1] == 1)
                    map[i][j] = 2;
            }
            else
            {
                if (map[i-1][j] == 1 || map[i][j-1] == 1 || map[i][j+1] == 1 || map[i+1][j] == 1)
                    map[i][j] = 2;
            }
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            if (map[i][j] == 2)
                map[i][j] = 1;
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, opening);
    fclose(opening);
    fclose(erosion);
    /********************************************************************/
    if ((dilation = fopen(filename4, "rb")) == NULL)
    {
        printf("��ͼƬʧ��");
        exit(0);
    }
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1,closing);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, closing);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, closing);
    fseek(dilation, fileHeader->bfOffBits, 0);
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            fread(map[i]+j,1,1,dilation);//��Դͼÿ�е�ÿһ���ֽڶ������a��ָ����ڴ�ռ�
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            {
            if (j == 0 && i == 0)
            {
                if (map[i][j+1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            else if (j == 0 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 0 || map[i][j+1] == 0)
                    map[i][j] = 2;
            }
            else if (j == 0)
            {
                if (map[i][j+1] == 0 || map[i+1][j] == 0 || map[i-1][j] == 0)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == 0)
            {
                if (map[i][j-1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1 && i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 0 || map[i][j-1] == 0)
                    map[i][j] = 2;
            }
            else if (j == infoHeader->biWidth - 1)
            {
                if (map[i][j-1] == 0 ||  map[i+1][j] == 0 || map[i-1][j] == 0)
                    map[i][j] = 2;
            }
            else if (i == 0)
            {
                if (map[i][j-1] == 0 || map[i][j+1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            else if (i == infoHeader->biHeight - 1)
            {
                if (map[i-1][j] == 0 || map[i][j-1] == 0 || map[i][j+1] == 0)
                    map[i][j] = 2;
            }
            else
            {
                if (map[i-1][j] == 0 || map[i][j-1] == 0 || map[i][j+1] == 0 || map[i+1][j] == 0)
                    map[i][j] = 2;
            }
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        {
        for(j=0;j<infoHeader->biWidth;j++)//����ÿ����ÿ���ֽڵ�ѭ��
            if (map[i][j] == 2)
                map[i][j] = 0;
        }
    for(i=0;i<infoHeader->biHeight;i++)//����ͼ��ÿ�е�ѭ��
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, closing);
    fclose(dilation);
    fclose(closing);

    /*�ͷ��ڴ�ռ䣬�ر��ļ�*/
    free(fileHeader);
    free(infoHeader);
    free(ipRGB);
    free(a);
    free(c);
    fclose(fpBMP);
    fclose(binarization);
    printf("Done\n");
    return 0;
}
