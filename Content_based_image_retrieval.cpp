#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
typedef struct imagedata{
    char *filename;
    float *histogram;
    int distance;
}Imagedata;
void (*histFunc)(Mat*,float*);//64 bin ya da LBP kullanılmasına göre değişecek olan fonksiyon işaretcisi
int imageCount;//öğrenilen resim sayısı
int histSize;//64 yada 256 olan histogram dizisi sayısı
char *directory;//dizin yolu tutan işaretçi
int getFiles(struct imagedata *images);//dizinde bulunan dosyaları images dizisine ekler
void _64binHistogram(Mat *img,float *histogram);//64 bin histogram yöntemine göre histogram dizisini oluşturur
void LBPhistogram(Mat *img,float *histogram);//LBP yöntemine göre histogram dizisini oluşturur.
void findMostSimilarImage(struct imagedata *searching,struct imagedata *data);//gösterilen resme en benzer resmi bulur.
int findDistance(float *hist,float *histogram);//verilen iki histogram arasındaki öklid mesafesini bulur.
int main(int argc, char** argv) {
    int i;//indis değişkeni
    int option;//kullanıcıdan alınan yöntem değişkeni
    struct imagedata *data = (struct imagedata*)malloc(50*sizeof(struct imagedata));//50 boyutlu resim bilgisi içeren yapı tanımı
    struct imagedata searching;//aranan resme ait işaretçi
    char *input;//kullanıcının aramak istediği dosyanın bağıl yolunu tutan işaretçi
    char tempPath[50];//dosyaların okunması için oluşturulan geçici yol değişkeni
    printf("Erişim yöntemi seçin:\n 1- Renk\n 2- Doku\n");
    scanf("%d",&option);
    if(option == 1){
        directory = "Dataset/Color/";//renk olması durumunda 64 bin histogram kullanılacak ve 
        histFunc = _64binHistogram;// histogram boyutu 64 olacaktır..
        histSize = 64;
    }else{
        directory = "Dataset/Texture/";//doku olması durumunda LBP kullanılacak ve 
        histFunc = LBPhistogram;// histogram boyutu 256 olacaktır.
        histSize = 256;
    }
    imageCount = getFiles(data);    
    for(i = 0;i<imageCount;i++) {
        strcpy(tempPath,directory);
        strcat(tempPath,data[i].filename);
        Mat image = imread(tempPath,1);//her resim okunup histogramı hesaplanır.
        data[i].histogram = (float*)calloc(histSize,sizeof(float));
        histFunc(&image,data[i].histogram);
    }
    printf("Bir resim gösterin: ");
    scanf("%ms",&input);//karşılaştırılmmak istenen resim girdi olarak alınır.
    strcpy(tempPath,directory);
    strcat(tempPath,input);
    searching.filename = tempPath;
    searching.histogram = (float*)calloc(histSize,sizeof(float));
    findMostSimilarImage(&searching,data);
    return 0;
}
int getFiles(struct imagedata *images){   
    char *buffer;
    int imagesCount=0;
    char command[50];
    strcpy(command,"ls ");// Linux ve Unix türevi sistemlerde ls komutu dizin içindeki dosyaları listeler.
    strcat(command,directory) ;
    FILE *fp = popen(command,"r");
    fscanf(fp,"%ms",&buffer);
    while(!feof(fp)){
        images[imagesCount++].filename = buffer;
        fscanf(fp,"%ms",&buffer);//Dosyalar okunarak diziye aktarılır..
    }
    return imagesCount;//bulunan dosya sayısı dönderilir.
}
void _64binHistogram(Mat *img,float *histogram){
    int i,j;
    int val;
    for(i=0;i<img->rows;i++){
        for(j=0;j<img->cols;j++){
            val= 0;
            Vec3b pixel = img->at<Vec3b>(i,j);
            val += pixel.val[2]>>6;//Kırmızıdan alınan iki bit eklenir.
            val = val<<2;//iki bit sola kaydırılır.
            val += (pixel.val[1]>>6);//Yeşilden iki bit eklenir.
            val = val<<2;
            val += (pixel.val[0]>>6);//Maviden iki bit eklenir.
            histogram[val]++;//histogram dizisinde oluşan değer artılır.
        }
    }
    return;
}
void LBPhistogram(Mat *img,float *histogram){
    Mat out;
    cvtColor(*img,out,CV_BGR2GRAY);//gri resme çevrilir.
    int i,j,k;
    int val;
    int change;
    int last;
    uchar pixel;
    for(i=1;i<img->rows-1;i++){
        for(j=1;j<img->cols-1;j++){
            val=0;
            change = 0;
            last=2;
            pixel = out.at<uchar>(i,j);
            val += (out.at<uchar>(i-1,j-1) > pixel) << 0;//her pixelin etrafında
            val += (out.at<uchar>(i-1,j  ) > pixel) << 1;//saat yönünde dolanarak
            val += (out.at<uchar>(i-1,j+1) > pixel) << 2;//karşılaştırma yapılır.
            val += (out.at<uchar>(i  ,j+1) > pixel) << 3;//Karşılaştırılan değer
            val += (out.at<uchar>(i+1,j+1) > pixel) << 4;//pikselin değerinde büyükse
            val += (out.at<uchar>(i+1,j  ) > pixel) << 5;//1 yazılır, değilse 0 
            val += (out.at<uchar>(i+1,j-1) > pixel) << 6;//yazılır ve sola kaydırılır.
            val += (out.at<uchar>(i  ,j-1) > pixel) << 7;
            for(k=0;k<8;k++){
                if((val>>k)%2 == 0){//oluşan değerde 1-0, 0-1 geçiş sayısı
                    if(last==1)     //hesaplanır.
                        change++;
                    last=0;
                }else{
                    if(last==0)
                        change++;
                    last=1;
                }                    
            }
            if(change>2)//2'den büyükse sabit bir değere yazılır.
                val = 172;
            histogram[val]++;
        }
    }
    return;
}
void findMostSimilarImage(struct imagedata *searching,struct imagedata *data){
    Mat img = imread(searching->filename,1);//aranan resim yüklenir ve
    histFunc(&img,searching->histogram);    //histogramı hesaplanır.
    int i,j,temp,minindex;
    for(i=0;i<imageCount;i++){
        temp = findDistance(searching->histogram,data[i].histogram);//aranan resmin 
        data[i].distance = temp;// bütün resimlere olan mesafesi hesaplanır.
    }
    int sortIndex[imageCount];//mesafelere göre sıralanmış resimlerin indislerini saklayan dizi.
    for(i=0;i<imageCount;i++){
        sortIndex[i] = i;
    }
    for(i=0;i<imageCount-1;i++){
        minindex = i;
        for(j=i+1;j<imageCount;j++){
            if(data[sortIndex[j]].distance<data[sortIndex[minindex]].distance){
                minindex = j;
            }
        }
        temp = sortIndex[i];//mesafelere göre sıralama işlemi
        sortIndex[i] = sortIndex[minindex];
        sortIndex[minindex] = temp;        
    }
    char file[50];
    char title[50];
    namedWindow("Aranan resim",CV_WINDOW_NORMAL);
    imshow("Aranan resim",img);
    for(i=1;i<6;i++){//0. indiste en benzer resim olarak kendisi bulunacak bu yüzden
        sprintf(file,"%s%s",directory,data[sortIndex[i]].filename);//1. resimden başlanarak gösterilir.
        sprintf(title,"%d. En Benzeyen",i);
        namedWindow(title,CV_WINDOW_NORMAL);
        imshow(title,imread(file,1));
    }
    cvWaitKey(0);
}
int findDistance(float *hist,float *histogram){
    int i;
    int distance=0;
    for(i=0;i<histSize;i++){
        distance+= sqrt(abs(hist[i]-histogram[i]));//iki histogramın mesafesi hesaplanır.
    }
    return distance;
}
