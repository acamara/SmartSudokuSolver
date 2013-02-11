#include "vector"
#include "string"
#include "Ocr.h"
#include "sudokuanalysis.h"
#include "sudokusolver.h"

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
	vector<int> sudoku_digits = sudokuanalysis.extrae_digitos(warp_image);

    cout<<"Sudoku de entrada: "<<endl<<endl;
	for (int k = 0; k < 81; k++) {
        if(sudoku_digits[k]==-1) cout<<"| ";
        else cout<<'|'<<sudoku_digits[k];
        if((k+1)%9==0){
            cout<<endl;
        }
    }
    cout<<endl;
    cout<<"Sudoku resuelto: "<<endl<<endl;
    SudokuSolver::inicializa();
    //string linea="4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......";;

    SudokuSolver* S = SudokuSolver::soluciona(new SudokuSolver(sudoku_digits));
    if (S != NULL) {
        S->escribe(cout);
    }
    else {
        cout << "No hay solución";
    }
    delete S;
    cout << endl;

    waitKey(0);
    return 0;
}

