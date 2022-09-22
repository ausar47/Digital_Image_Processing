#include<stdio.h>
#include<stdlib.h>
typedef unsigned short UINT16;
typedef unsigned long DWORD;
typedef unsigned char UNCHAR;
//存储bmp文件头
#pragma pack(push)
#pragma pack(2) //设置2字节缩进
typedef struct tagBITMAPFILEHEADER{
    UINT16 bfType;         //BM
    DWORD  bfSize;         //size of file
    UINT16 bfReserved1;
    UINT16 bfReserved2;
    DWORD bfOffBits;       //header offset
}BITMAPFILEHEADER;         //14字节
#pragma pack(pop)
//存储位图信息头
typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;          //信息头大小
    DWORD biWidth;         //图像宽度
    DWORD biHeight;        //图像高度
    UINT16 biPlanes;       //图像的面位数
    UINT16 biBitCount;     //每个像素的位数
    DWORD biCompression;   //压缩类型
    DWORD biSizeImage;     //图文件大小
    DWORD biXPelsPerMeter; //水平分辨率
    DWORD biYPelsPerMeter; //垂直分辨率
    DWORD biClrUsed;       //使用的色彩数
    DWORD biClrImportant;  //重要的颜色数
}BITMAPINFOHEADER;         //40字节
//储存rgb图像信息
typedef struct tagrbgIMAGE{
    UNCHAR **r;//指向二维矩阵，表示图像的R信息
    UNCHAR **g;//指向二维矩阵，表示图像的G信息
    UNCHAR **b;//指向二维矩阵，表示图像的B信息
}rgbIMAGE;
typedef rgbIMAGE *rgbIMAGEINFO;
//存储yuv图像信息
typedef struct tagyuvIMAGE{
    UNCHAR **y;//指向二维矩阵，表示图像的Y信息
    char **u;//指向二维矩阵，表示图像的U信息
    char **v; //指向二维矩阵，表示图像的V信息
}yuvIMAGE;
typedef yuvIMAGE *yuvIMAGEINFO;
/**
 *@brief 显示位图文件头信息
 *@param[in] pBmpHead 指向位图文件头结构体
*/
void showBmpHead(BITMAPFILEHEADER* pBmpHead);
/**
 *@brief 显示位图信息头
 *@param[in] pBmpInforHead 指向位图信息头结构体
*/
void showBmpInfoHead(BITMAPINFOHEADER* pBmpInforHead);
/**
 *@brief 读取bmp文件图像数据
 *@param[in] pBmpHead: 指向位图文件头结构体
 *@param[in] pBmpInforHead: 指向位图信息头结构体
 *@param[in] fp: 指向位图文件返回的文件指针
 *@return  一个存储rgb图像信息的结构体
*/
rgbIMAGEINFO bmp2rgb(BITMAPFILEHEADER* pBmpHead, BITMAPINFOHEADER* pBmpInfoHead, FILE* fp);
/**
 *@brief 将rgb图像数据经过变化得到rgb图像数据
 *@param[in] rgb: 存储rgb图像数据的结构体
 *@param     height: 图像的高
 *@param     width: 图像的宽
 *@return  一个存储yuv图像信息的结构体
*/
yuvIMAGEINFO rgb2yuv(rgbIMAGEINFO rgb, int height, int width);
/**
 *@brief 求出一个二维矩阵的最大值或最小值
 *@param[in] a: 表示二维矩阵
 *@param     row: 二维矩阵的行数
 *@param     col: 二维矩阵的列数
 *@param     choice: 1则返回二维矩阵最大值，2则返回二维矩阵最小值
 *@return  返回二维矩阵的最大值或最小值
*/
UNCHAR findmaxorminnum(UNCHAR **a, int row, int col, int choice);
/**
 *@brief 将一个二维矩阵的中的数都限定在[0，255] 之间
 *@param[in] a: 表示二维矩阵
 *@param     row: 二维矩阵的行数
 *@param     col: 二维矩阵的列数
 *@return  返回处理后的二维矩阵
*/
UNCHAR **rearrange(UNCHAR **a,int row,int col);
int main(){
    //open bmp file
    FILE *fpbmp;
    FILE *fpout, *fpout2;
    BITMAPFILEHEADER bmpfileheader;    //指向bmp文件头
    BITMAPINFOHEADER bmpinfoheader;    //指向bmp信息头
    rgbIMAGEINFO rgbPicPoint;          //指向存储rgb图信息的结构体
    rgbIMAGEINFO outrgb;               //指向yuv转到rgb的图信息的结构体
    yuvIMAGEINFO yuvPicPoint;          //指向存储yuv图信息的结构体
    char *filename;
    int row, col;
    int i, j;

    //输入文件绝对路径
    printf("Please enter the filename:");
    filename = (char *) malloc(100 * sizeof(char));
    scanf("%s", filename);
    //只读打开一个二进制文件
    fpbmp = fopen(filename, "rb");
    //如果文件读取失败，返回空指针
    if(!fpbmp){
        printf("Can not open the picture!");
        return 0;
    }
    //让fpbmp指向bmp文件的开始
    fseek(fpbmp, 0, SEEK_SET);
    //读取文件信息
    fread(&bmpfileheader, sizeof(BITMAPFILEHEADER), 1, fpbmp);  //读取文件头信息
    fread(&bmpinfoheader, sizeof(BITMAPINFOHEADER),1, fpbmp);  //读取信息头信息
    showBmpHead(&bmpfileheader);                             //输出文件头信息
    printf("**********************************************\n");
    showBmpInfoHead(&bmpinfoheader);                         //输出信息头信息
    printf("**********************************************\n");
    int width;
    int height;
    if(bmpinfoheader.biBitCount >= 24){
        rgbPicPoint = bmp2rgb(&bmpfileheader, &bmpinfoheader, fpbmp);
        width = bmpinfoheader.biWidth;                         //图像的宽
        height = bmpinfoheader.biHeight;	                   //图像的高
    }
    //将RGB图片转为YUV
    yuvPicPoint = rgb2yuv(rgbPicPoint, height, width);
    //Rearrange gray intensity to lie between [0,255]
    UNCHAR**grayimage = (UNCHAR**) malloc(sizeof(UNCHAR*) * height);
    for(row = 0; row < height; row++){
        grayimage[row] = (UNCHAR*) malloc(sizeof(UNCHAR) * width);
    }
    grayimage = rearrange(yuvPicPoint->y, height, width);
    /*Write a grayscale bmp*/
    //将信息写入文件
    fpout = fopen("gray.bmp", "wb");
    if(!fpout){
        printf("Can not write the file!");
    }
    else{
        fwrite(&bmpfileheader, sizeof(BITMAPFILEHEADER), 1, fpout);      //将文件头写入文件
        fwrite(&bmpinfoheader, sizeof(BITMAPINFOHEADER), 1, fpout);      //将信息头写入文件
        UNCHAR *gray = (UNCHAR*) malloc(sizeof(UNCHAR) * height * width * 3);  //用一个行向量储存图像数据
        //将图像按24位存储，并将原二维数列扩充3倍，放入一维矩阵
        unsigned long k = 0;
        for(row = 0; row < height; row++){
            for(col = 0; col < width; col++){
                gray[k++] = grayimage[row][col];
                gray[k++] = grayimage[row][col];
                gray[k++] = grayimage[row][col];
            }
        }
        //将这个一维数列表示的图像数据写入文件
        fwrite(gray, sizeof(UNCHAR), height * width * 3, fpout);
        //关闭写入的文件
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
    //初始化
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
    //通过公式将yuv转换为rgb
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
    fwrite(&bmpfileheader, sizeof(BITMAPFILEHEADER), 1, fpout2);         //将文件头写入文件
    fwrite(&bmpinfoheader, sizeof(BITMAPINFOHEADER), 1, fpout2);         //将信息头写入文件
    UNCHAR *rgb2 = (UNCHAR*) malloc(sizeof(UNCHAR) * height * width * 3);	  //用一个行向量储存图像数据
    //按照bgr的顺序将3个二维矩阵存储为一个一维矩阵
    unsigned long k = 0;
    for(row = 0; row < height; row++){
        for(col = 0; col < width; col++){
            rgb2[k++] = outrgb->b[row][col];
            rgb2[k++] = outrgb->g[row][col];
            rgb2[k++] = outrgb->r[row][col];
        }
    }
    //将这个一维数列表示的图像数据写入文件
    fwrite(rgb2, sizeof(UNCHAR), height * width * 3, fpout2);
    //关闭文件
    fclose(fpout2);
    printf("Successfully obtained RGB image!\n");
    system("pause");
    return 0;
}

rgbIMAGEINFO bmp2rgb(BITMAPFILEHEADER* pBmpHead, BITMAPINFOHEADER* pBmpInfoHead, FILE* fp){
    int bfoffbits = pBmpHead->bfOffBits;    //从文件头开始到实际图像数据之间的字节的偏移量
    rgbIMAGEINFO rgb = (rgbIMAGEINFO) malloc(sizeof(rgbIMAGE));
    if(!rgb){
        printf("Out of memory!");
        return NULL;
    }
    int row, col;
    fseek(fp, bfoffbits, SEEK_SET);
    int width = pBmpInfoHead->biWidth;     //获得图像的高
    int height = pBmpInfoHead->biHeight;  //获得图像的宽
    //初始化
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
    //读取图像rgb信息
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
    //初始化
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
    //用公式将rgb信息转换为yuv
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
    printf("文件大小:%lu\n", pBmpHead->bfSize);
    printf("保留字:%d\n", pBmpHead->bfReserved1);
    printf("保留字:%d\n", pBmpHead->bfReserved2);
    printf("实际位图数据的偏移字节数:%lu\n", pBmpHead->bfOffBits);
}

void showBmpInfoHead(BITMAPINFOHEADER* pBmpInforHead)
{
    printf("位图信息头:\n");
    printf("结构体的长度:%lu\n", pBmpInforHead->biSize);
    printf("位图宽:%lu\n", pBmpInforHead->biWidth);
    printf("位图高:%lu\n", pBmpInforHead->biHeight);
    printf("biPlanes平面数:%d\n", pBmpInforHead->biPlanes);
    printf("biBitCount采用颜色位数:%d\n", pBmpInforHead->biBitCount);
    printf("压缩方式:%lu\n", pBmpInforHead->biCompression);
    printf("biSizeImage实际位图数据占用的字节数:%lu\n", pBmpInforHead->biSizeImage);
    printf("X方向分辨率:%lu\n", pBmpInforHead->biXPelsPerMeter);
    printf("Y方向分辨率:%lu\n", pBmpInforHead->biYPelsPerMeter);
    printf("使用的颜色数:%lu\n", pBmpInforHead->biClrUsed);
    printf("重要颜色数:%lu\n", pBmpInforHead->biClrImportant);
}