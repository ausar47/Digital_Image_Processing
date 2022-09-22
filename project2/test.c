#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<Windows.h>

int main()
{
    /*变量声明*/
    FILE *fpBMP,*binarization, *erosion, *dilation, *opening, *closing;//源文件fpBMP,目标文件fpTwoValue
    char filename1[20], filename2[20], filename3[20], filename4[20], filename5[20], filename6[20];

    BITMAPFILEHEADER *fileHeader;//位图文件头
    BITMAPINFOHEADER *infoHeader;//位图信息头
    RGBQUAD *ipRGB;//调色板

    int i,j,k=0;
    unsigned char *a;//存储源图每行像素值
    unsigned char b;//存储每个像素的灰度值或二值
    unsigned char *c;//存储每行像素的二值


    printf("输入图像文件名：");
    scanf("%s", filename1);
    if ((fpBMP = fopen(filename1, "rb")) == NULL)
    {
        printf("打开图片失败");
        exit(0);
    }

    strcpy(filename2, "binarization.bmp");
    strcpy(filename3, "erosion.bmp");
    strcpy(filename4, "dilation.bmp");
    strcpy(filename5, "opening.bmp");
    strcpy(filename6, "closing.bmp");
    if ((binarization = fopen(filename2, "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    if ((erosion = fopen(filename3, "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    if ((dilation = fopen(filename4, "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    if ((opening = fopen(filename5, "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    if ((closing = fopen(filename6, "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    /********************************************************************/

    /*创建位图文件头，信息头，调色板*/
    fileHeader=(BITMAPFILEHEADER *)malloc(sizeof(BITMAPFILEHEADER));
    infoHeader=(BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
    ipRGB=(RGBQUAD *)malloc(2*sizeof(RGBQUAD));

    /*读入源位图文件头和信息头*/
    fread(fileHeader,sizeof(BITMAPFILEHEADER),1,fpBMP);
    fread(infoHeader,sizeof(BITMAPINFOHEADER),1,fpBMP);
    //经过这两条程序把BMP图像的信息头、文件头赋给fileHeader和infoHeader变量，可以根据fileHeader和infoHeader得到图像的各种属性。
//    printf("原始图片每个像素的位数:%d\n" ,infoHeader->biBitCount);
//    printf("原始图片每个像素像素数据偏移:%d\n" ,fileHeader->bfOffBits);
    //修改信息头
    //信息头共有11部分，灰度化时需要修改4部分
    infoHeader->biBitCount=8;//转换成二值图后，颜色深度由24位变为8位
    infoHeader->biSizeImage=((infoHeader->biWidth + 3) / 4) * 4 * infoHeader->biHeight;//每个像素由三字节变为单字节，同时每行像素要四字节对齐
    infoHeader->biClrUsed=2;//颜色索引表数量，二值图为2
    infoHeader->biClrImportant=0;//重要颜色索引为0，表示都重要
    //修改文件头
    //文件头共有5部分，灰度化时需要修改两部分
    fileHeader->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+2*sizeof(RGBQUAD);//数据区偏移量，等于文件头，信息头，索引表的大小之和
    fileHeader->bfSize=fileHeader->bfOffBits+infoHeader->biSizeImage;//文件大小，等于偏移量加上数据区大小
    ipRGB[1].rgbBlue=ipRGB[1].rgbGreen=ipRGB[1].rgbRed=ipRGB[1].rgbReserved=0;//调色板颜色为黑色对应的索引为0
    ipRGB[0].rgbBlue=ipRGB[0].rgbGreen=ipRGB[0].rgbRed=200;//白色对应的索引为150-255

//    printf("修改后的图片每个像素的位数:%d\n" ,infoHeader->biBitCount);
//    printf("修改后的图片每个像素数据偏移:%d\n" ,fileHeader->bfOffBits);

    /********************************************************************/

    //读取BMP图像的信息头、文件头、BMP调色板到新建的图片
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, binarization);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, binarization);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, binarization);

    /*将彩色图转为二值图*/
    a=(unsigned char *)malloc((infoHeader->biWidth*3+3)/4*4);//给变量a申请源图每行像素所占大小的空间,考虑四字节对齐问题
    c=(unsigned char *)malloc((infoHeader->biWidth+3)/4*4);//给变量c申请目标图每行像素所占大小的空间,同样四字节对齐

    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<((infoHeader->biWidth*3+3)/4*4);j++)//遍历每行中每个字节的循环
            {
            fread(a+j,1,1,fpBMP);//将源图每行的每一个字节读入变量a所指向的内存空间
            }
        for(j=0;j<infoHeader->biWidth;j++)//循环像素宽度次,就不会计算读入四字节填充位
            {
            b=(int)(0.114*(float)a[k]+0.587*(float)a[k+1]+0.299*(float)a[k+2]);//a中每三个字节分别代表BGR分量，乘上不同权值转化为灰度值
            if(120>(int)b) //将灰度值转化为二值
                b=1;
            else b=0;
            c[j]=b;   //存储每行的二值
            k+=3;
            }
        fwrite(c,(infoHeader->biWidth+3)/4*4, 1, binarization);//将二值像素四字节填充写入文件,填充位没有初始化,为随机值
        k=0;
        }
    fclose(binarization);
    /********************************************************************/
    if ((binarization = fopen(filename2, "rb")) == NULL)
    {
        printf("打开图片失败");
        exit(0);
    }

    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, erosion);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, erosion);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, erosion);
    fseek(binarization, fileHeader->bfOffBits, 0);
    unsigned char map[infoHeader->biHeight][(infoHeader->biWidth+3)/4*4];
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//遍历每行中每个字节的循环
            {
            fread(map[i]+j,1,1,binarization);//将源图每行的每一个字节读入变量a所指向的内存空间
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
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
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
            if (map[i][j] == 2)
                map[i][j] = 0;
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, erosion);
    fclose(erosion);

    /********************************************************************/
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, dilation);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, dilation);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, dilation);
    fseek(binarization, fileHeader->bfOffBits, 0);
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//遍历每行中每个字节的循环
            {
            fread(map[i]+j,1,1,binarization);//将源图每行的每一个字节读入变量a所指向的内存空间
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
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
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
            if (map[i][j] == 2)
                map[i][j] = 1;
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, dilation);
    fclose(dilation);
    /********************************************************************/
    if ((erosion = fopen(filename3, "rb")) == NULL)
    {
        printf("打开图片失败");
        exit(0);
    }

    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, opening);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, opening);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, opening);
    fseek(erosion, fileHeader->bfOffBits, 0);
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//遍历每行中每个字节的循环
            {
            fread(map[i]+j,1,1,erosion);//将源图每行的每一个字节读入变量a所指向的内存空间
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
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
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
            if (map[i][j] == 2)
                map[i][j] = 1;
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, opening);
    fclose(opening);
    fclose(erosion);
    /********************************************************************/
    if ((dilation = fopen(filename4, "rb")) == NULL)
    {
        printf("打开图片失败");
        exit(0);
    }
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1,closing);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, closing);
    fwrite(ipRGB,2*sizeof(RGBQUAD), 1, closing);
    fseek(dilation, fileHeader->bfOffBits, 0);
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<((infoHeader->biWidth+3)/4*4);j++)//遍历每行中每个字节的循环
            {
            fread(map[i]+j,1,1,dilation);//将源图每行的每一个字节读入变量a所指向的内存空间
            }
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
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
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        {
        for(j=0;j<infoHeader->biWidth;j++)//遍历每行中每个字节的循环
            if (map[i][j] == 2)
                map[i][j] = 0;
        }
    for(i=0;i<infoHeader->biHeight;i++)//遍历图像每行的循环
        fwrite(map[i],(infoHeader->biWidth+3)/4*4, 1, closing);
    fclose(dilation);
    fclose(closing);

    /*释放内存空间，关闭文件*/
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
