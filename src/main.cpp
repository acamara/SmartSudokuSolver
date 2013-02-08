#include "vector"
#include "string"
#include "sstream"
#include "Ocr.h"
#include "SudokuAnalysis.h"

using namespace std;

int main(int argc, char** argv) {
    //Read sudoku image
    Mat image, image_gray;
    image = imread("imagenes/revista.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    //Show Sudoku image
	namedWindow("Sudoku",CV_WINDOW_AUTOSIZE);
	imshow("Sudoku", image);

	// Convert to Grayscale (Sudoku Image)
	cvtColor(image, image_gray, CV_RGB2GRAY, 0);
	//Show Sudoku image grayscale
	//namedWindow("Sudoku_grayscale",CV_WINDOW_AUTOSIZE);
	//imshow("Sudoku_grayscale", image_gray);

    //Create sudoku analysis class
    SudokuAnalysis sudokuanalysis;

    // Detecta sudoku puzzle
    vector<Point> cuadrado_sudoku = sudokuanalysis.detecta_sudoku_puzzle(image, image_gray);

    //Transforma la perspectiva
	Mat warp_image = sudokuanalysis.transforma_perspectiva(cuadrado_sudoku, image_gray);

	//Extrae los digitos del sudoku
	sudokuanalysis.extrae_digitos(warp_image);

    waitKey(0);
    return 0;
}

