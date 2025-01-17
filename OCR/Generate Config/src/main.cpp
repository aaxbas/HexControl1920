#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include<iostream>
#include<vector>

const int min_area = 100;
const int image_width = 20;
const int image_height = 30;

using namespace cv;
using namespace std;

int main(){
	Mat imgRGB;
	Mat imgGray;
	Mat imgBlurring;
	Mat imgBW;
	Mat imgBWCopy;

	vector<vector<Point>> ptContours;
	vector<Vec4i> v4iHierarchy;

	Mat GroupInts;
	Mat FlattenedFloats;

	vector<int> Chars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
        'U', 'V', 'W', 'X', 'Y', 'Z' };

	imgRGB = imread("Input_Chars.png");

	if (imgRGB.empty()){
		cout<<"Image not in directory\n";
		return(0);
	}

	cvtColor(imgRGB,imgGray,COLOR_RGB2GRAY);

	GaussianBlur(imgGray,
			imgBlurring,
			Size(5,5),
			0);

	adaptiveThreshold(imgBlurring,
			imgBW,
			255,
			ADAPTIVE_THRESH_GAUSSIAN_C,
			THRESH_BINARY_INV,
			11,
			2);

	imgBWCopy = imgBW.clone();
	
	imshow("imgBW",imgBW);
	imshow("Blurring",imgBlurring);

	findContours(imgBWCopy,
			ptContours,
			v4iHierarchy,
			RETR_EXTERNAL,
			CHAIN_APPROX_SIMPLE);

	//for (int i = 0; i < ptContours.size(); i++){
		if (contourArea(ptContours[1]) > min_area){
			Rect rectangleChar = boundingRect(ptContours[1]);
			rectangle(imgRGB, rectangleChar, Scalar(255,0,0),2);
			Mat matROI = imgBW(rectangleChar);
			Mat matROIResized;
			resize(matROI, matROIResized, Size(image_width, image_height));

			imshow("matROI",matROI);
			imshow("matROIResized", matROIResized);
			imshow("imgRGB", imgRGB);
			
			int userInput = waitKey(0);
			cout<<userInput<<"\n";
			if (userInput == 27){
				return(0);
			}
			else if (userInput == 170){
				cout<<"Shift press observed\n";
				userInput = waitKey(0);
				userInput = (userInput - 32);
				cout<<userInput<<" New input\n";
			}
			
			if (find(Chars.begin(), Chars.end(), userInput) != Chars.end()){
				cout<<"valid input appending the xml files.\n";

				GroupInts.push_back(userInput);

				Mat matImageFloat;

				matROIResized.convertTo(matImageFloat, CV_32FC1);

				Mat FlatFloats = matImageFloat.reshape(1,1);

				FlattenedFloats.push_back(FlatFloats);
			}
		}
	//}

	cout<<"Training Completed\n";

	FileStorage fsClassifications("group_ints.xml", FileStorage::WRITE);
	if (fsClassifications.isOpened() == false){
		cout<<"Unable to open group_ints\n";
		return(0);
	}

	fsClassifications<<"groups"<<GroupInts;
	fsClassifications.release();

	FileStorage fsTrainingImage("image.xml",FileStorage::WRITE);
	if (fsTrainingImage.isOpened() == false){
		cout<<"Unable to open image\n";
		return(0);
	}
	fsTrainingImage<<"images"<<FlattenedFloats;
	fsTrainingImage.release();
	return(0);

		
		
			
}

