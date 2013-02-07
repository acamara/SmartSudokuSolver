#ifndef OCR_H
#define OCR_H

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp">
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
#define MAX_NUM_IMAGES    60000

class Ocr{
    public:
        Ocr();
        ~Ocr();

        bool train(CvMat* trainData, CvMat* trainClasses);
        int classify(Mat grayimage, int ImageSize);
        Mat preprocess_image_digit(Mat grayimage, int sizex, int sizey, bool& is_number);
        Mat preprocess_image(Mat inImage,int sizex, int sizey);
        void runselftest();
    private:


    private:
        int train_samples = 1;
        int classes = 10;
        int sizex = 50;
        int sizey = 50;
        int ImageSize = sizex*sizey;
        string pathToImages = "train_numbers";
        KNearest *knn;
        CvMat* trainData;
        CvMat* trainClasses;
        int numRows, numCols, numImages;
        int num_iterations;
        int num_digits;
};

#endif // OCR_H
