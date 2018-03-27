/*
------------------------------------------------------------------------------------------------------------------------------------------
Project       : Depth Stroke Recognition
File          : Classification.cpp
Description   : This code uses unistroke $1 gesture recognition library created by Baylor Wetzeli
Author        : Konstantinos Monahopoulos
------------------------------------------------------------------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------------------------
Includes
--------------------------------------------------------------------------------------------------
*/
#include "Classification.h"

/*
--------------------------------------------------------------------------------------------------
Functions definition
--------------------------------------------------------------------------------------------------
*/
Classification::Classification() {

	//	RECOGNITION	//

	//	This pointer contains all the function implementations of its class.
	//	The class constructor is in file GeometricReecognizer.cpp
	//	All the other classes in the same namespace has the constructor definition
	//	inside the class.
	GeoR = &GeoRec;

	//	Creates a pointer to struct SampleGestures, this struct contains all the predefined
	//	samples in coordinates. Inside the structure are the functions for the templates.
	//	These functions return object type Path2D.
	SampleG = &SampleGest;

	//	Creates a pointer to a vector that holds the detected coordinate stream
	Path2DT = &Path2DTest;

	//	Creates a pointer for the current coordinates. Class Point2D is inside GeometricRecognizerTypes.h
	//	and accepts two argumets (int,int) - (float,float), (overrided) to construct.
	//	Point 2D holds the coordinates (x,y) and stores it inside the class.
	Point2DT = &Point2DTest;

	//	Function loadTemplates loads all of the templates for recognition, because it takes no argumets
	//	all of the templates will be loaded. In order to load specific templates you must comment
	//	the unwanted templates.
	GeoR->loadTemplates();
}

//	Auto - classification function, just pass the coordinates
void Classification::Classify( std::vector<double> DecPoints, const std::string& ClassifierStatus) {
	//std::cout << DecPoints.empty() << '\n';

	if (ClassifierStatus == "Sample" && !DecPoints.empty() ){
		//	Collect desicion points
		Path2DT->push_back(DollarRecognizer::Point2D( DecPoints.at(0), DecPoints.at(1) ));

	}else if(ClassifierStatus == "Predict"){
		DollarRecognizer::RecognitionResult r = GeoR->recognize(*Path2DT);		//	Deecision making
		cout << "Recognized gesture: " << r.name << endl;
		cout << "1$ Prediction Score:" << r.score << endl;
		Path2DT->clear();
	}
}
