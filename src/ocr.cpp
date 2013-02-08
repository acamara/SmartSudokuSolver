#include "Ocr.h"
#include <stdio.h>

Ocr::Ocr(){
    num_iterations = 1;
    num_digits = 0;

    trainData =  cvCreateMat(classes * train_samples,imagesize, CV_32FC1);
    trainClasses = cvCreateMat(classes * train_samples, 1, CV_32FC1);

    train(trainData, trainClasses);

    knn = new KNearest(trainData, trainClasses);

    //runselftest();
}

Ocr::~Ocr(){
    delete knn;
}

Mat Ocr::preprocess_image(Mat inImage,int sizex, int sizey){
    Mat outImage, grayImage, blurredImage, thresholdImage, contourImage, regionOfInterest;

    vector<vector<Point> > contours;

    cvtColor(inImage, grayImage, COLOR_BGR2GRAY);

    GaussianBlur(grayImage, blurredImage, Size(5, 5), 2, 2);
    adaptiveThreshold(blurredImage, thresholdImage, 255, 1, 1, 11, 2);

    thresholdImage.copyTo(contourImage);

    findContours(contourImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    int idx = 0;
    size_t area = 0;

    for (size_t i = 0; i < contours.size(); i++){
        if (area < contours[i].size() ){
            idx = i;
            area = contours[i].size();
        }
    }

    Rect rec = boundingRect(contours[idx]);

    regionOfInterest = thresholdImage(rec);

    resize(regionOfInterest, outImage, Size(sizex, sizey));
    return outImage;
}

Mat Ocr::preprocess_image_digit(Mat grayimage, int sizex, int sizey, bool& is_number){
    Mat outImage, blurredImage, thresholdImage, contourImage, regionOfInterest;
    vector<vector<Point> > contours;

    GaussianBlur(grayimage, blurredImage, Size(5, 5), 2, 2);
    adaptiveThreshold(blurredImage, thresholdImage, 255, 1, 1, 11, 2);

    thresholdImage.copyTo(contourImage);

    Mat output(thresholdImage, cv::Range(4, 46), cv::Range(4, 46));
    int count_non=countNonZero(output);
    if(count_non >140){
        //std::cout<<"Existe número: "<<count_non<<std::endl;
        num_digits++;
        is_number = true;
    }
    else{
        is_number = false;
    }
    //std::cout<<"Número de dígitos en el sudoku: "<<num_digits<<std::endl;
    //namedWindow("Threshold digit", CV_WINDOW_AUTOSIZE);
    //imshow("Threshold digit", output);

    findContours(contourImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    int idx = 0;
    size_t area = 0;

    for (size_t i = 0; i < contours.size(); i++){
        if (area < contours[i].size() ){
            idx = i;
            area = contours[i].size();
        }
    }

    Rect rec = boundingRect(contours[idx]);

    regionOfInterest = thresholdImage(rec);

    resize(regionOfInterest, outImage, Size(sizex, sizey));
    return outImage;
}

void Ocr::runselftest(){
    Mat img;
    CvMat* sample2 = cvCreateMat(1, imagesize, CV_32FC1);
    // SelfTest
    char file[255];
    int z = 0;
    while (z++ < 10){
        int iSecret = rand() % 10;
        //cout << iSecret;
        sprintf(file, "%s/%d.png", pathtoimages.c_str(), iSecret);
        img = imread(file, 1);
        Mat stagedImage = preprocess_image(img, sizex, sizey);
        for (int n = 0; n < imagesize; n++){
            sample2->data.fl[n] = stagedImage.data[n];
        }
        float detectedClass = knn->find_nearest(sample2, 1);
        if (iSecret != (int) ((detectedClass))){
            std::cout << "Incapaz de reconocer " << iSecret << " por sorteo "<< (int) ((detectedClass));
            exit(1);
        }
        std::cout << "Detectado " << (int) ((detectedClass)) << "\n";
        imshow("single", img);
        waitKey(0);
    }
}

bool Ocr::train(CvMat* trainData, CvMat* trainClasses){
    Mat img;
    char file[255];

    for (int i = 0; i < classes; i++){
        sprintf(file, "%s/digit_1%d.jpg", pathtoimages.c_str(), i);
        img = imread(file, 1);
        if (!img.data){
            std::cout << "File " << file << " not found\n";
            exit(1);
        }
        Mat outfile = preprocess_image(img, sizex, sizey);

        for (int n = 0; n < imagesize; n++){
            trainData->data.fl[i*imagesize + n] = outfile.data[n];
        }
        trainClasses->data.fl[i] = i;
    }
}

int Ocr::classify(Mat grayimage, int imagesize){
    CvMat* sample2 = cvCreateMat(1, imagesize, CV_32FC1);
    bool isnumber = false;
    Mat stagedImage = preprocess_image_digit(grayimage, 50, 50, isnumber);
    //namedWindow( "Image preprocesed to classify", CV_WINDOW_AUTOSIZE );
    //imshow( "Image preprocesed to classify", stagedImage);
    if(isnumber){
        for (int n = 0; n < imagesize; n++){
            sample2->data.fl[n] = stagedImage.data[n];
        }
        float detectedClass = knn->find_nearest(sample2, 1);

        cout<< (int) ((detectedClass));
    }
    else{
        cout << " ";
    }
    if(num_iterations%9==0){
        cout<<endl;
    }
    num_iterations++;
}
