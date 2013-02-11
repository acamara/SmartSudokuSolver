#include "sudokuanalysis.h"

SudokuAnalysis::SudokuAnalysis(){
    myocr = new Ocr();

}

SudokuAnalysis::~SudokuAnalysis(){
    //dtor
}

vector<Point> SudokuAnalysis::reordena_puntos_cuadrado(vector<Point> cuadrado_sudoku){
	vector<Point> v_puntos;
	double max = 0.0, min = 1000.0, sub_max = 0, sub_min = 1000.0;
	size_t max_pos = 0, min_pos = 0, sub_max_pos = 0, sub_min_pos = 0;

	for(size_t i = 0; i<cuadrado_sudoku.size(); i++){
		double suma = cuadrado_sudoku.at(i).x + cuadrado_sudoku.at(i).y;

		if(max<suma){
			max_pos = i;
			max = suma;
		}

		if(suma<min){
			min_pos = i;
			min = suma;
		}
	}

	for(size_t i = 0; i<cuadrado_sudoku.size(); i++){
		if(i != max_pos && i != min_pos){
			if(sub_max < cuadrado_sudoku.at(i).x){
				sub_max_pos = i;
				sub_max = cuadrado_sudoku.at(i).x;
			}
			if(cuadrado_sudoku.at(i).x < sub_min){
				sub_min_pos = i;
				sub_min = cuadrado_sudoku.at(i).x;
			}
		}
	}

	v_puntos.push_back(cuadrado_sudoku.at(min_pos));
	v_puntos.push_back(cuadrado_sudoku.at(sub_max_pos));
	v_puntos.push_back(cuadrado_sudoku.at(sub_min_pos));
	v_puntos.push_back(cuadrado_sudoku.at(max_pos));

	return v_puntos;
}

Mat SudokuAnalysis::transforma_perspectiva(vector<Point> cuadrado_vertices, Mat src_gray){
	vector<Point2f> src_vertices;
	for(size_t i = 0; i<cuadrado_vertices.size(); i++){
		src_vertices.push_back(cuadrado_vertices.at(i));
	}

	vector<Point2f> dst_vertices;
	dst_vertices.push_back(Point(0, 0));
	dst_vertices.push_back(Point(449, 0)); // Bug was: had mistakenly switched these 2 parameters
	dst_vertices.push_back(Point(0, 449));
	dst_vertices.push_back(Point(449, 449));

	Mat warpMatrix = getPerspectiveTransform(src_vertices, dst_vertices);

	Mat warp_image = src_gray.clone();
	warpPerspective(src_gray, warp_image, warpMatrix, Size(450,450));

	return warp_image;
}

void SudokuAnalysis::clasifica_linea(Vec4i line, vector<Vec4i> &vertical_lines, vector<Vec4i> &horitzontal_lines){
    if(pow(line[2]-line[0],2) >= pow(line[3]-line[1],2)){
        line[0] = 0;
        line[2] = 449;
        horitzontal_lines.push_back(line);
    }
    else{
        line[1] = 0;
        line[3] = 449;
        vertical_lines.push_back(line);
    }

}

vector<int> SudokuAnalysis::extrae_digitos(Mat warp_image){
    vector<vector<Point> > cuadrados_digitos;
    vector<Point> v_puntos;
    vector<int> sudoku_digits;

    Mat undistortedThreshed = warp_image.clone();
    //adaptiveThreshold(warp_image, undistortedThreshed, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 101, 1);

    int sep_x = 0;
    int k = 1;

    for(int i=0; i<9; i++){
        int sep_y = 0;
        for(int j=0; j<9; j++){
            /*
            v_puntos.clear();
            v_puntos.push_back(Point(sep_x,sep_y));
            v_puntos.push_back(Point(sep_x+50,sep_y));
            v_puntos.push_back(Point(sep_x,sep_y+50));
            v_puntos.push_back(Point(sep_x+50,sep_y+50));
            /**/
            Mat tmp(warp_image, cv::Range(sep_x, sep_x+50), cv::Range(sep_y, sep_y+50));
            int detected_num = myocr->classify(tmp, 2500);
            sudoku_digits.push_back(detected_num);


            // compute sum of positive matrix elements, iterator-based variant
            //namedWindow("Digit", CV_WINDOW_AUTOSIZE );
            //imshow("Digit", tmp);
            //waitKey(0);
            /*
            stringstream s;
            s << "digito_" << k<<".jpg";
            s.str();
            imwrite(s.str().c_str(), tmp);
            */

            cuadrados_digitos.push_back(v_puntos);
            //cout<<v_puntos<<endl;
            //k++;
            sep_y = sep_y + 50;
        }
        sep_x= sep_x+50;
    }

    //namedWindow( "Warp Image threshold", CV_WINDOW_AUTOSIZE );
	//imshow( "Warp Image threshold", undistortedThreshed );
	return sudoku_digits;
}

void SudokuAnalysis::muestra_gridpuzzle(Mat warp_image){
    Mat cdst2;
    cvtColor(warp_image, cdst2, CV_GRAY2BGR);
    Mat cdst3(450, 450, CV_8UC3);
    int sep_y = 0;

    for(size_t  i = 0; i < 10; i++ ){
        int sep_x = 0;
        for( size_t j = 0; j < 10; j++ ){
            //cout<<"x: "<<sep_x<<" y: "<<sep_y<<endl;
            //Lineas horizontales
            line(cdst2, Point(0, sep_y), Point(450, sep_y), Scalar(0,0,255), 1, CV_AA);
            //Lineas verticales
            line(cdst2, Point(sep_x, 0), Point(sep_x, 450), Scalar(0,0,255), 1, CV_AA);
            //Puntos del puzzle
            circle( cdst2, Point(sep_x, sep_y), 6, Scalar( 255, 0, 0 ), -1,8);
            sep_x = sep_x + 50;
        }
        sep_y = sep_y + 50;
    }

    namedWindow("Aprox lines", CV_WINDOW_AUTOSIZE );
	imshow("Aprox lines", cdst2);
}

vector<Point> SudokuAnalysis::detecta_sudoku_puzzle (Mat src, Mat src_gray){
	Mat dst_threshold, canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

    // Noise removal with gaussian blur
	GaussianBlur(src_gray, src_gray,Size(5,5),0);

	// Adaptive thresholding
	adaptiveThreshold(src_gray, dst_threshold, max_value, adaptative_method, threshold_type, blocksize, C );

	// Find contours in threshold image
	findContours( dst_threshold, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	//Evaluate all blobs to find blob with biggest area
	//biggest rectangle in the image must be sudoku square
	vector<vector<Point> > contours_poly( contours.size() );
	double max_area = 0;
	vector<Point> cuadrado_sudoku;
	int biggest = -1;

	for( size_t i = 0; i< contours.size(); i++ ){
		double area = contourArea(contours.at(i), false);
		if(area>100){
			double peri = arcLength(contours.at(i),true);
			approxPolyDP( Mat(contours[i]), contours_poly[i], 0.02*peri, true );

			if( area > max_area && contours_poly[i].size()==4) {
				cuadrado_sudoku = contours_poly[i];
				biggest = i;
				max_area = area;
				//cout << "biggest: "<<biggest<<" Max area: "<< area<<endl;
			}
		}
	}

	//cout<<"Cuadrado sudoku "<<cuadrado_sudoku<<endl;
	cuadrado_sudoku = reordena_puntos_cuadrado(cuadrado_sudoku);
	//cout<<"Vector de salida: "<<cuadrado_sudoku<<endl;

	// Draw sudoku puzzle contour
	Mat drawing = src.clone();
	Scalar color = Scalar(0,0,255);
	drawContours( drawing, contours_poly, biggest, color, 2, 8, hierarchy, 0, Point() );
	//circle(drawing, cuadrado_sudoku.at(3), 5, (0,255,0), 2, 8, 0);

    //cout<<"Número de línias verticales: "<<vertical_lines.size()<<endl;
	//cout<<"Número de línias horizontales: "<<horitzontal_lines.size()<<endl;

    //Display some images to see the process
	//namedWindow("Sudoku_threshold",1);
	//imshow("Sudoku_threshold", dst_threshold);

	//namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	//imshow( "Contours", drawing );

    return cuadrado_sudoku;
}
