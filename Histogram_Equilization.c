#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct Image{
    int *data;
    int width,height;
    char *magicNumber;
    int maxGrayValue;    
};
struct Image readImage(char *fileName);
struct Image histogramEquilazion(struct Image img);
void writeImage(struct Image img,char* saveFileName);

int main(int argc, char** argv) {
    struct Image img = readImage(argv[1]);
    if(img.data == NULL){
        return 1;
    }
    struct Image newimg = histogramEquilazion(img);
    char saveFileName[strlen(argv[1])+4];
    sprintf(saveFileName,"out-%s",argv[1]);
    writeImage(newimg,saveFileName);    
    return (EXIT_SUCCESS);
}
struct Image readImage(char* fileName){
    FILE *filep = fopen(fileName,"r");
    if(filep==NULL){
        printf("Dosya açılamadı.\n");
        return;
    }
    else{
        printf("Çalışıyor.");
    }
    exit(0);
    struct Image img;
    int i,endOfLine,parameterCounter=0;
    char header[3][10];
    while(parameterCounter<3){
        char *headerParameter;
        headerParameter = (char*)malloc(10*sizeof(char));
        endOfLine=0;
        for(i=0;!endOfLine;i++){
            char c = fgetc(filep);
            if(c==10){
                sprintf(header[parameterCounter],"%s",headerParameter);
                parameterCounter++;
                endOfLine=1;
            }
            else if(c=='#'){
                while(fgetc(filep)!=10){}
                endOfLine=1;
            }
            else{
                headerParameter[i] = c;
            }
        }
    }
    img.magicNumber = (char*)malloc(strlen(header[0])*sizeof(char));
    sscanf(header[0],"%s",img.magicNumber);
    sscanf(header[1],"%d%d",&img.width,&img.height);
    sscanf(header[2],"%d",&img.maxGrayValue);
    img.data = (int*)malloc(img.width*img.height*sizeof(int));
    i=0;
    while(!feof(filep)){
        img.data[i]=fgetc(filep);
        i++;
    }
    fclose(filep);
    return img;
}
void writeImage(struct Image img,char* saveFileName){
    FILE *filep = fopen(saveFileName,"w");
    fprintf(filep,"%s\n%d %d\n%d\n",img.magicNumber,img.width,img.height,img.maxGrayValue);
    int i,j;
    for(i=0;i<img.height;i++){
        for(j=0;j<img.width;j++){
            fputc(img.data[i*img.width+j],filep);
        }
    }
    printf("Dosya %s adıyla kaydedildi.\n",saveFileName);
    fclose(filep);
    return;
}
struct Image histogramEquilazion(struct Image img){
    int i,j; //index variables defined
    int totalPixels = img.width*img.height;//this is total pixel count in image
    int *frequencyCounter;//this array contains the number of each pixel intensity
    double *probabilities;//this array contains the probability and cumulative probability of each pixel intensity
    frequencyCounter = (int*)malloc(img.maxGrayValue*sizeof(int));
    probabilities = (double*)malloc(img.maxGrayValue*sizeof(double));
    struct Image newimg;
    newimg.height = img.height;
    newimg.width = img.width;
    newimg.maxGrayValue = img.maxGrayValue;
    newimg.magicNumber = (char*)malloc(strlen(img.magicNumber)*sizeof(int));
    strcpy(newimg.magicNumber,img.magicNumber);
    newimg.data = (int*)malloc(img.width*img.height*sizeof(int));
    
    for(i=0;i<img.maxGrayValue;i++){
        frequencyCounter[i]=0;//firstly moving 0 to all of the indexs of frequencyCounter
    }
    for(i=0;i<img.height;i++){
        for(j=0;j<img.width;j++){
            frequencyCounter[img.data[i*img.width+j]]++;//counting the number of each pixel intensity
        }
    }
    for(i=0;i<img.maxGrayValue;i++){
        probabilities[i] = ((double)frequencyCounter[i])/((double)totalPixels);//counting probability of each pixel intensity
    }
    for(i=1;i<img.maxGrayValue;i++){
        probabilities[i] += probabilities[i-1];//counting cumulative probablity
    }
    for(i=0;i<img.height;i++){
        for(j=0;j<img.width;j++){
            newimg.data[i*img.width+j]= probabilities[img.data[i*img.width+j]]*255;//since we want to change intensity range 0-255
            //multiplying all probabilities with 255, and parse integer for floor rounnding
        }
    }
    return newimg;
}
