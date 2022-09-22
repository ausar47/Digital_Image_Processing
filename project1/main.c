#include<stdio.h>
#include<stdlib.h>
typedef unsigned short UINT16;
typedef unsigned long DWORD;
typedef unsigned char UNCHAR;
//�洢bmp�ļ�ͷ
#pragma pack(push)
#pragma pack(2) //����2�ֽ�����
typedef struct tagBITMAPFILEHEADER{
    UINT16 bfType;         //BM
    DWORD  bfSize;         //size of file
    UINT16 bfReserved1;
    UINT16 bfReserved2;
    DWORD bfOffBits;       //header offset
}BITMAPFILEHEADER;         //14�ֽ�
#pragma pack(pop)
//�洢λͼ��Ϣͷ
typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;          //��Ϣͷ��С
    DWORD biWidth;         //ͼ����
    DWORD biHeight;        //ͼ��߶�
    UINT16 biPlanes;       //ͼ�����λ��
    UINT16 biBitCount;     //ÿ�����ص�λ��
    DWORD biCompression;   //ѹ������
    DWORD biSizeImage;     //ͼ�ļ���С
    DWORD biXPelsPerMeter; //ˮƽ�ֱ���
    DWORD biYPelsPerMeter; //��ֱ�ֱ���
    DWORD biClrUsed;       //ʹ�õ�ɫ����
    DWORD biClrImportant;  //��Ҫ����ɫ��
}BITMAPINFOHEADER;         //40�ֽ�
//����rgbͼ����Ϣ
typedef struct tagrbgIMAGE{
    UNCHAR **r;//ָ���ά���󣬱�ʾͼ���R��Ϣ
    UNCHAR **g;//ָ���ά���󣬱�ʾͼ���G��Ϣ
    UNCHAR **b;//ָ���ά���󣬱�ʾͼ���B��Ϣ
}rgbIMAGE;
typedef rgbIMAGE *rgbIMAGEINFO;
//�洢yuvͼ����Ϣ
typedef struct tagyuvIMAGE{
    UNCHAR **y;//ָ���ά���󣬱�ʾͼ���Y��Ϣ
    char **u;//ָ���ά���󣬱�ʾͼ���U��Ϣ
    char **v; //ָ���ά���󣬱�ʾͼ���V��Ϣ
}yuvIMAGE;
typedef yuvIMAGE *yuvIMAGEINFO;
/**
 *@brief ��ʾλͼ�ļ�ͷ��Ϣ
 *@param[in] pBmpHead ָ��λͼ�ļ�ͷ�ṹ��
*/
void showBmpHead(BITMAPFILEHEADER* pBmpHead);
/**
 *@brief ��ʾλͼ��Ϣͷ
 *@param[in] pBmpInforHead ָ��λͼ��Ϣͷ�ṹ��
*/
void showBmpInfoHead(BITMAPINFOHEADER* pBmpInforHead);
/**
 *@brief ��ȡbmp�ļ�ͼ������
 *@param[in] pBmpHead: ָ��λͼ�ļ�ͷ�ṹ��
 *@param[in] pBmpInforHead: ָ��λͼ��Ϣͷ�ṹ��
 *@param[in] fp: ָ��λͼ�ļ����ص��ļ�ָ��
 *@return  һ���洢rgbͼ����Ϣ�Ľṹ��
*/
rgbIMAGEINFO bmp2rgb(BITMAPFILEHEADER* pBmpHead, BITMAPINFOHEADER* pBmpInfoHead, FILE* fp);
/**
 *@brief ��rgbͼ�����ݾ����仯�õ�rgbͼ������
 *@param[in] rgb: �洢rgbͼ�����ݵĽṹ��
 *@param     height: ͼ��ĸ�
 *@param     width: ͼ��Ŀ�
 *@return  һ���洢yuvͼ����Ϣ�Ľṹ��
*/
yuvIMAGEINFO rgb2yuv(rgbIMAGEINFO rgb, int height, int width);
/**
 *@brief ���һ����ά��������ֵ����Сֵ
 *@param[in] a: ��ʾ��ά����
 *@param     row: ��ά���������
 *@param     col: ��ά���������
 *@param     choice: 1�򷵻ض�ά�������ֵ��2�򷵻ض�ά������Сֵ
 *@return  ���ض�ά��������ֵ����Сֵ
*/
UNCHAR findmaxorminnum(UNCHAR **a, int row, int col, int choice);
/**
 *@brief ��һ����ά������е������޶���[0��255] ֮��
 *@param[in] a: ��ʾ��ά����
 *@param     row: ��ά���������
 *@param     col: ��ά���������
 *@return  ���ش����Ķ�ά����
*/
UNCHAR **rearrange(UNCHAR **a,int row,int col);
int main(){
    //open bmp file
    FILE *fpbmp;
    FILE *fpout, *fpout2;
    BITMAPFILEHEADER bmpfileheader;    //ָ��bmp�ļ�ͷ
    BITMAPINFOHEADER bmpinfoheader;    //ָ��bmp��Ϣͷ
    rgbIMAGEINFO rgbPicPoint;          //ָ��洢rgbͼ��Ϣ�Ľṹ��
    rgbIMAGEINFO outrgb;               //ָ��yuvת��rgb��ͼ��Ϣ�Ľṹ��
    yuvIMAGEINFO yuvPicPoint;          //ָ��洢yuvͼ��Ϣ�Ľṹ��
    char *filename;
    int row, col;
    int i, j;

    //�����ļ�����·��
    printf("Please enter the filename:");
    filename = (char *) malloc(100 * sizeof(char));
    scanf("%s", filename);
    //ֻ����һ���������ļ�
    fpbmp = fopen(filename, "rb");
    //����ļ���ȡʧ�ܣ����ؿ�ָ��
    if(!fpbmp){
        printf("Can not open the picture!");
        return 0;
    }
    //��fpbmpָ��bmp�ļ��Ŀ�ʼ
    fseek(fpbmp, 0, SEEK_SET);
    //��ȡ�ļ���Ϣ
    fread(&bmpfileheader, sizeof(BITMAPFILEHEADER), 1, fpbmp);  //��ȡ�ļ�ͷ��Ϣ
    fread(&bmpinfoheader, sizeof(BITMAPINFOHEADER),1, fpbmp);  //��ȡ��Ϣͷ��Ϣ
    showBmpHead(&bmpfileheader);                             //����ļ�ͷ��Ϣ
    printf("**********************************************\n");
    showBmpInfoHead(&bmpinfoheader);                         //�����Ϣͷ��Ϣ
    printf("**********************************************\n");
    int width;
    int height;
    if(bmpinfoheader.biBitCount >= 24){
        rgbPicPoint = bmp2rgb(&bmpfileheader, &bmpinfoheader, fpbmp);
        width = bmpinfoheader.biWidth;                         //ͼ��Ŀ�
        height = bmpinfoheader.biHeight;	                   //ͼ��ĸ�
    }
    //��RGBͼƬתΪYUV
    yuvPicPoint = rgb2yuv(rgbPicPoint, height, width);
    //Rearrange gray intensity to lie between [0,255]
    UNCHAR**grayimage = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        grayimage[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    grayimage = rearrange(yuvPicPoint->y, height, width);
    /*Write a grayscale bmp*/
    //����Ϣд���ļ�
    fpout = fopen("gray.bmp", "wb");
    if(!fpout){
        printf("Can not write the file!");
    }
    else{
        fwrite(&bmpfileheader, sizeof(BITMAPFILEHEADER), 1, fpout);      //���ļ�ͷд���ļ�
        fwrite(&bmpinfoheader, sizeof(BITMAPINFOHEADER), 1, fpout);      //����Ϣͷд���ļ�
        UNCHAR *gray = (UNCHAR*) malloc(sizeof(UNCHAR) * height * width * 3);  //��һ������������ͼ������
        //��ͼ��24λ�洢������ԭ��ά��������3��������һά����
        unsigned long k = 0;
        for(row = 0; row < height; row++){
            for(col = 0; col < width; col++){
                gray[k++] = grayimage[row][col];
                gray[k++] = grayimage[row][col];
                gray[k++] = grayimage[row][col];
            }
        }
        //�����һά���б�ʾ��ͼ������д���ļ�
        fwrite(gray, sizeof(UNCHAR), height * width * 3, fpout);
        //�ر�д����ļ�
        fclose(fpout);
        printf("Successfully obtained grayscale image!\n");
    }
    //Change the luminance value Y
    for(row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            yuvPicPoint->y[row][col] += 5;
        }
    }
    //yuv->rgb
    outrgb = (rgbIMAGEINFO) malloc(sizeof(rgbIMAGE));
    //��ʼ��
    outrgb->b = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        outrgb->b[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    outrgb->g = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        outrgb->g[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    outrgb->r = (UNCHAR **) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        outrgb->r[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    yuvPicPoint->y = rearrange(yuvPicPoint->y, height, width);
    //ͨ����ʽ��yuvת��Ϊrgb
    for(row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            outrgb->r[row][col] = (UNCHAR) (1.0000 * yuvPicPoint->y[row][col] + 1.1398 * yuvPicPoint->v[row][col]);
            outrgb->g[row][col] = (UNCHAR) (0.9996 * yuvPicPoint->y[row][col] - 0.3954 * yuvPicPoint->u[row][col] - 0.5805 * yuvPicPoint->v[row][col]);
            outrgb->b[row][col] = (UNCHAR) (1.0020 * yuvPicPoint->y[row][col] + 2.0361 * yuvPicPoint->u[row][col] - 0.0005 * yuvPicPoint->v[row][col]);
        }
    }
    //write a color bmp
    fpout2 = fopen("lenaps.bmp", "wb");
    if(!fpout2){
        printf("Wrong!Can not open the file!\n");
    }
    fwrite(&bmpfileheader, sizeof(BITMAPFILEHEADER), 1, fpout2);         //���ļ�ͷд���ļ�
    fwrite(&bmpinfoheader, sizeof(BITMAPINFOHEADER), 1, fpout2);         //����Ϣͷд���ļ�
    UNCHAR *rgb2 = (UNCHAR*) malloc(sizeof(UNCHAR) * height * width * 3);	  //��һ������������ͼ������
    //����bgr��˳��3����ά����洢Ϊһ��һά����
    unsigned long k = 0;
    for(row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            rgb2[k++] = outrgb->b[row][col];
            rgb2[k++] = outrgb->g[row][col];
            rgb2[k++] = outrgb->r[row][col];
        }
    }
    //�����һά���б�ʾ��ͼ������д���ļ�
    fwrite(rgb2, sizeof(UNCHAR), height * width * 3, fpout2);
    //�ر��ļ�
    fclose(fpout2);
    printf("Successfully obtained RGB image!\n");
    system("pause");
    return 0;
}

rgbIMAGEINFO bmp2rgb(BITMAPFILEHEADER* pBmpHead, BITMAPINFOHEADER* pBmpInfoHead, FILE* fp){
    int bfoffbits = pBmpHead->bfOffBits;    //���ļ�ͷ��ʼ��ʵ��ͼ������֮����ֽڵ�ƫ����
    rgbIMAGEINFO rgb = (rgbIMAGEINFO) malloc(sizeof(rgbIMAGE));
    if(!rgb){
        printf("Out of memory!");
        return NULL;
    }
    int row, col;
    fseek(fp, bfoffbits, SEEK_SET);
    int width = pBmpInfoHead->biWidth;     //���ͼ��ĸ�
    int height = pBmpInfoHead->biHeight;  //���ͼ��Ŀ�
    //��ʼ��
    rgb->b = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        rgb->b[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    rgb->g = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        rgb->g[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    rgb->r = (UNCHAR **) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        rgb->r[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    //��ȡͼ��rgb��Ϣ
    for(row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            fread(&rgb->b[row][col], sizeof(UNCHAR), 1, fp);
            fread(&rgb->g[row][col], sizeof(UNCHAR), 1, fp);
            fread(&rgb->r[row][col], sizeof(UNCHAR), 1, fp);
        }
    }
    return rgb;
}

yuvIMAGEINFO rgb2yuv(rgbIMAGEINFO rgb, int height, int width){
    yuvIMAGEINFO yuv = (yuvIMAGEINFO) malloc(sizeof(yuvIMAGE));
    if(!yuv){
        printf("Out of memory!");
        return NULL;
    }
    int row, col;
    //��ʼ��
    yuv->y = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        yuv->y[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    yuv->u = (char**) malloc(sizeof(char*) * height);
    for(row = 0; row < height; row++){
        yuv->u[row] = (char*) malloc(sizeof(char) * width);
    }
    yuv->v = (char**) malloc(sizeof(char*) * height);
    for(row = 0; row < height; row++){
        yuv->v[row] = (char*) malloc(sizeof(char) * width);
    }
    //�ù�ʽ��rgb��Ϣת��Ϊyuv
    for(row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            yuv->y[row][col] = (UNCHAR) (0.299 * rgb->r[row][col] + 0.587 * rgb->g[row][col] + 0.144 * rgb->b[row][col]);
            yuv->u[row][col] = (char) (-0.147 * rgb->r[row][col] - 0.289 * rgb->g[row][col] + 0.435 * rgb->b[row][col]);
            yuv->v[row][col] = (char) (0.615 * rgb->r[row][col] - 0.515 * rgb->g[row][col] - 0.100 * rgb->b[row][col]);
        }
    }
    return yuv;
}

UNCHAR findmaxorminnum(UNCHAR **a, int row, int col, int choice){
    UNCHAR max = 0, min = 0;
    int i, j;
    switch(choice){
        case 1:
            for(i = 0; i < row; i++){
                for(j = 0; j < col; j++){
                    if(a[i][j] > max){
                        max = a[i][j];
                    }
                }
            }
            return max;
        case 2:
            for(i = 0; i < row; i++){
                for(j = 0; j < col; j++){
                    if(a[i][j] < min){
                        min = a[i][j];
                    }
                }
            }
            return min;
        default:
            break;
    }
}

UNCHAR **rearrange(UNCHAR **a,int row,int col){
    UNCHAR **rea = (UNCHAR**) malloc(sizeof(UNCHAR*) * row);
    int i,j;
    UNCHAR max, min, scope;
    for(i = 0; i < row; i++){
        rea[i] = (UNCHAR*) malloc(sizeof(UNCHAR) * col);
    }
    max = findmaxorminnum(a, row, col, 1);
    min = findmaxorminnum(a, row, col, 2);
    scope = max - min;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            rea[i][j] = (UNCHAR) (255 * (a[i][j] - min) / scope);
        }
    }
    return rea;
}

void showBmpHead(BITMAPFILEHEADER* pBmpHead)
{
    printf("�ļ���С:%lu\n", pBmpHead->bfSize);
    printf("������:%d\n", pBmpHead->bfReserved1);
    printf("������:%d\n", pBmpHead->bfReserved2);
    printf("ʵ��λͼ���ݵ�ƫ���ֽ���:%lu\n", pBmpHead->bfOffBits);
}

void showBmpInfoHead(BITMAPINFOHEADER* pBmpInforHead)
{
    printf("λͼ��Ϣͷ:\n");
    printf("�ṹ��ĳ���:%lu\n", pBmpInforHead->biSize);
    printf("λͼ��:%lu\n", pBmpInforHead->biWidth);
    printf("λͼ��:%lu\n", pBmpInforHead->biHeight);
    printf("biPlanesƽ����:%d\n", pBmpInforHead->biPlanes);
    printf("biBitCount������ɫλ��:%d\n", pBmpInforHead->biBitCount);
    printf("ѹ����ʽ:%lu\n", pBmpInforHead->biCompression);
    printf("biSizeImageʵ��λͼ����ռ�õ��ֽ���:%lu\n", pBmpInforHead->biSizeImage);
    printf("X����ֱ���:%lu\n", pBmpInforHead->biXPelsPerMeter);
    printf("Y����ֱ���:%lu\n", pBmpInforHead->biYPelsPerMeter);
    printf("ʹ�õ���ɫ��:%lu\n", pBmpInforHead->biClrUsed);
    printf("��Ҫ��ɫ��:%lu\n", pBmpInforHead->biClrImportant);
}