#ifndef SUDOKUANALYSIS_H
#define SUDOKUANALYSIS_H

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp">
#include <vector>
#include "ocr.h"

using namespace cv;
using namespace std;

class SudokuAnalysis{
    public:
        SudokuAnalysis();
        ~SudokuAnalysis();
        vector<Point> detecta_sudoku_puzzle (Mat src, Mat src_gray);
        vector<Point> reordena_puntos_cuadrado(vector<Point> cuadrado_sudoku);
        void muestra_gridpuzzle(Mat warp_image);
        void extrae_digitos(Mat warp_image);
        void clasifica_linea(Vec4i line, vector<Vec4i> &vertical_lines, vector<Vec4i> &horitzontal_lines);
        Mat transforma_perspectiva(vector<Point> cuadrado_vertices, Mat src_gray);
    protected:
    private:
        Ocr *myocr;
        int threshold_type = 1;
        int adaptative_method = 1;
        int blocksize = 11;
        int const max_value = 255;
        int C = 2;
};

#endif // SUDOKUANALYSIS_H
