#ifndef OCR_H
#define OCR_H

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp">

using namespace cv;
using namespace std;

class Ocr{
    public:
        Ocr();
        ~Ocr();

        bool train(CvMat* trainData, CvMat* trainClasses);
        int classify(Mat grayimage, int imagesize);
        Mat preprocess_image_digit(Mat grayimage, int sizex, int sizey, bool& is_number);
        Mat preprocess_image(Mat inImage,int sizex, int sizey);
        void runselftest();
    private:


    private:
        int train_samples = 1;
        int classes = 10;
        int sizex = 50;
        int sizey = 50;
        int imagesize = sizex*sizey;
        string pathtoimages = "train_numbers";
        KNearest *knn;
        CvMat* trainData;
        CvMat* trainClasses;
        int num_digits;
};

#endif // OCR_H
