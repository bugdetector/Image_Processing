#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <math.h>
using namespace std;
using namespace cv;
int cols;
int rows;
typedef struct{
    int r;
    int g;
    int b;
    int pixels;
    int sumr;
    int sumg;
    int sumb;
}color;
void reassignAll(int **label,int before,int after,int row,int col,int lasti, int lastj);
int ConnectedComponentLabeling(const Mat outputimage, int **kmat);
Mat kmeans(Mat input, int k,int **kmat);
//this function does k measn clustering and returns result in a new Mat
float euclideanDist(color first,color second);
//this funtiction computes euclidian distance and returns result as float
int findMin(float *arr,int size);
//founds minimum value in an float array and returns it's index
int main(int argc, char** argv) {
    Mat image = imread(argv[1],1);
    if(image.empty()){
        printf("Dosya yüklenemedi!!\n");
        return -1;
    }
    int k,i;
    printf("K sayısını giriniz: ");
    scanf("%d",&k);
    int **kmat = new int*[image.rows+1];//kmat stores label number for each pixel
    for(i=0;i<image.rows+1;i++){
        kmat[i] = new int[image.cols+1];
    }
    Mat KmeanImage = kmeans(image,k,kmat);
    namedWindow("Asıl resim",CV_WINDOW_AUTOSIZE);
    imshow("Asıl resim",image);
    namedWindow("k resim",CV_WINDOW_AUTOSIZE);
    imshow("k resim",KmeanImage);
    Mat connectedLabeled = KmeanImage.clone();
    int objcnt = ConnectedComponentLabeling(connectedLabeled,kmat);
    printf("\nBağımsız bölüntü sayısı:%d\n",objcnt);
    namedWindow("Bölütlenmiş resim",CV_WINDOW_AUTOSIZE);
    imshow("Bölütlenmiş resim",connectedLabeled);
    for(i=0;i<image.rows+1;i++){
       free(kmat[i]);
    }
    free(kmat);
    waitKey(0);
    return 0;
}
Mat kmeans(Mat input, int knum,int **kmat){
    float *kdist;//using for store and  finding minimum of euclidian distances
    int i,j,k,l,m;//index counters
    color *kconvercs;//stores average color variables of each k values   
    int index;//usisng for store the index of minimum distance k's
    color temp;//temporary color
    int changeCount;//using for understand if k's are converged
    Mat output = input.clone();
    kconvercs = (color*)malloc(knum*sizeof(color));//storing clusters's average color
    kdist = (float*)malloc(knum*sizeof(float));//kdist must be float
    srand(time(NULL));
    for(int k=0;k<knum;k++){
        i = rand()%input.rows;//choosing indexes randomly
        j = rand()%input.cols;//so i guarentee that choosing a valid color
        kconvercs[k].r = input.at<Vec3b>(i,j).val[2];
        kconvercs[k].g = input.at<Vec3b>(i,j).val[1];
        kconvercs[k].b = input.at<Vec3b>(i,j).val[0];
        kconvercs[k].pixels = 0;
    }
    changeCount=1;
    while(changeCount){//if there is no change this is stops
        changeCount=0;
        for(i=0;i<input.rows;i++){
            for(j=0;j<input.cols;j++){
                temp.r = input.at<Vec3b>(i,j).val[2];
                temp.g = input.at<Vec3b>(i,j).val[1];
                temp.b = input.at<Vec3b>(i,j).val[0];
                for(k=0;k<knum;k++){//computing distances for each k value
                    kdist[k] = euclideanDist(kconvercs[k],temp);
                }//to that pixel
                index = findMin(kdist,knum);//finding minimum distanced values
                if(kmat[i][j]!=index){      //index
                    changeCount++;//counting changes
                    kmat[i][j]=index;//labelling pixel
                }                
            }
        }
        for(k=0;k<knum;k++){
            kconvercs[k].sumr = 0;
            kconvercs[k].sumg = 0;
            kconvercs[k].sumb = 0;
            kconvercs[k].pixels = 0;
        }
        for(i=0;i<input.rows;i++){
            for(j=0;j<input.cols;j++){//computing sum of each color valur for
                //k, and counting pixels for each k
                    kconvercs[kmat[i][j]].sumr += input.at<Vec3b>(i,j).val[2];
                    kconvercs[kmat[i][j]].sumg += input.at<Vec3b>(i,j).val[1];
                    kconvercs[kmat[i][j]].sumb += input.at<Vec3b>(i,j).val[0];
                    kconvercs[kmat[i][j]].pixels++;
            }                    
        }
        for(k=0;k<knum;k++){//computing average
            kconvercs[k].r = kconvercs[k].sumr/kconvercs[k].pixels;
            kconvercs[k].g = kconvercs[k].sumg/kconvercs[k].pixels;
            kconvercs[k].b = kconvercs[k].sumb/kconvercs[k].pixels;
        }
              
    }
    for(i=0;i<input.rows;i++){
            for(j=0;j<input.cols;j++){
                output.at<Vec3b>(i,j).val[2] = kconvercs[kmat[i][j]].r;
                output.at<Vec3b>(i,j).val[1] = kconvercs[kmat[i][j]].g;
                output.at<Vec3b>(i,j).val[0] = kconvercs[kmat[i][j]].b;
        }//moving k color values to output image
    }
    return output;
}
float euclideanDist(color first,color second){
    float result;
    int r = second.r-first.r;
    int g = second.g-first.g;
    int b = second.b-first.b;
    result = sqrt(r*r+g*g+b*b);
    return result;
}
int findMin(float *arr,int size){
    int minindex=0,i;
    for(i=0;i<size;i++){
        if(arr[i]<=arr[minindex]){
            minindex = i;
        }
    }
    return minindex;
}
int ConnectedComponentLabeling(Mat outputimage, int **kmat)
{/*It is labeling Connected Components and returns result in outputimage*/
    int i,j,k,l;
    int **label = new int*[outputimage.rows+2];//kmat stores label number for each pixel
    for(i=0;i<outputimage.rows+2;i++){
        label[i] = new int[outputimage.cols+2];
    };
    int label_count = 0,waste_count=0; // starts at 2 because 0,1 are used already
    int templabel;    
    for(i=0;i<outputimage.rows;i++){
        for(j=0;j<outputimage.cols;j++){
            label[i][j]=0;
        }
    }
    for( i=1; i < outputimage.rows-1; i++) {
        for(j=1; j < outputimage.cols-1; j++) {
            templabel=0;
            for(k=-1;k<1;k++){
                for(l=-1;l<2;l++){
                    if(kmat[i+k][j+l]==kmat[i][j]){
                        if(label[i+k][j+l]!=0 && label[i+k][j+l]!=templabel ){
                            if(templabel!=0){
                                reassignAll(label,label[i+k][j+l],templabel,outputimage.rows,outputimage.cols,i,j);
                                //deleting all second labels and assigning them to first
                                waste_count++;
                            }else
                                templabel=label[i+k][j+l];
                        }
                    }
                }
            }
            if(templabel==0){
                label_count++;
                label[i][j] = label_count;
            }
            else{
                label[i][j] = templabel;
            }
        }
    }
    color labelColor[label_count];
    for(i=0;i<label_count;i++){
        labelColor[i].r=rand()%256;
        labelColor[i].g=rand()%256;
        labelColor[i].b=rand()%256;
    }
    for(i=0;i<outputimage.rows;i++){
        for(j=0;j<outputimage.cols;j++){
            outputimage.at<Vec3b>(i,j).val[2] = labelColor[label[i][j]].r;
            outputimage.at<Vec3b>(i,j).val[1] = labelColor[label[i][j]].g;
            outputimage.at<Vec3b>(i,j).val[0] = labelColor[label[i][j]].b;
        }
    }    
    return (label_count-waste_count);
}
void reassignAll(int **label,int before,int after,int row,int col,int lasti,int lastj){
    /*it is reassgning all values which is before to after value in label matrix*/
    int i,j;
    for(i=0;i<row;i++){
        for(j=0;j<col;j++){
            if(label[i][j]==before){
                label[i][j]=after;
            }
            if(i==lasti&&j==lastj){
                i=row;
                j=col;
            }
        }
    }
    return;
}
