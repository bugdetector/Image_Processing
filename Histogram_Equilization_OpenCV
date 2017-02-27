#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>

IplImage* histogramEquilazion(IplImage *img);
int main(int argc, char** argv) {
    IplImage *img = cvLoadImage(argv[1],0);//loading image which name is argv[1]
    if(!img){
        printf("Resim yüklenemedi!!");
        return EXIT_FAILURE;
    }
    IplImage *newimg = histogramEquilazion(img);
    
    cvNamedWindow("Asıl",CV_WINDOW_NORMAL);
    cvShowImage("Asıl",img);
    cvNamedWindow("İşlenmiş",CV_WINDOW_NORMAL);
    cvShowImage("İşlenmiş",newimg);
    cvWaitKey(0);
    
    int params[3] = {CV_IMWRITE_PNG_COMPRESSION,100,0};
    char FileName[strlen(argv[1])+6];
    strcat(FileName,"ut-");
    FileName[0]= 'o';
    strcat(FileName,argv[1]);
    cvSaveImage(FileName,newimg, params);
    printf("Yeni resim %s adıyla kaydedildi.\n",FileName);
    return (EXIT_SUCCESS);
}
IplImage* histogramEquilazion(IplImage *img){
    int i,j; //index variables defined
    IplImage *newimg = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);//new image matris defined for store new image variables
    int frequencyCounter[256];//this array contains the number of each pixel intensity
    double probabilities[256];//this array contains the probability and cumulative probability of each pixel intensity
    int height = img->height;//this is height of image
    int width = img->width;//this is width of image
    double totalPixels = width*height;//this is total pixel count in image
    for(i=0;i<256;i++){
        frequencyCounter[i]=0;//firstly moving 0 to all of the indexs of frequencyCounter
    }
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            frequencyCounter[(int) cvGet2D(img,i,j).val[0]]++;//counting the number of each pixel intensity
        }
    }
    for(i=0;i<256;i++){
        probabilities[i] = ((double)frequencyCounter[i])/(totalPixels);//counting probability of each pixel intensity
    }
    for(i=1;i<256;i++){
        probabilities[i] += probabilities[i-1];//counting cumulative probablity
    }
    CvScalar value;
     for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            value.val[0]=probabilities[(int) cvGet2D(img,i,j).val[0]]*255;//since we want to change intensity range 0-255
            cvSet2D(newimg,i,j,value);//multiplying all probabilities with 255, and parse integer for floor rounnding
        }
    }
    return newimg;
}
