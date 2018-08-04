/*
------------------------------------------------------------------------------------------------------------------------------------------
Project       : Depth Stroke Recognition
File          : MultiStrokeRec.cpp
Description   : Stroke Classification based on depth images
Author        : Konstantinos Monachopoulos
------------------------------------------------------------------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------------------------
Includes
--------------------------------------------------------------------------------------------------
*/
#include "ImageProcessing.h"
#include <iostream>
#include "DirAccess.h"	//	C POSIX library
#include "Classification.h"


/*
--------------------------------------------------------------------------------------------------
Main Process
--------------------------------------------------------------------------------------------------
*/
int main(int argc, char * argv[])
{

  /*
	--------------------------------------------------------------------------------------------------
	Class Objects
	--------------------------------------------------------------------------------------------------
	*/
  DirAccess dirAccess;
  ImageProc imageProc;
  Classification classification;

for (int Fcounter = 0; Fcounter < dirAccess.Num_Of_Folders; Fcounter++) { // iterates over the folders

      // Get images paths
      dirAccess.GetPaths(Fcounter);
      if (dirAccess.DepthPath.empty()){
        std::cout << "Couldn't read images" << '\n';
        return 1;
      }
      // sort names in ascend order.
      sort(dirAccess.DepthPath.begin(),dirAccess.DepthPath.end());

      // get images of the specific stroke
      dirAccess.Getimages();

      for (unsigned int Vcounter = 0; Vcounter < dirAccess.DepthImgs.size(); Vcounter++) {

      //  Histogram equilization by iterations Threshold calculation + constant threshold in depth
			imageProc.HistEqu(&dirAccess.DepthImgs.at(Vcounter), &imageProc.FinalThres, 32);

      //	Execute thresholding with the calculated value
			imageProc.Thres(dirAccess.DepthImgs.at(Vcounter), imageProc.RawThreshImg, &imageProc.FinalThres);

      //	Track moving nearest object
			imageProc.BlobAnalysis(imageProc.RawThreshImg, imageProc.RawMeanBlob, classification.DecisionPoints);	//	can't print that to display all

      // Sample the classification points for prediction
      classification.Classify(classification.DecisionPoints, "Sample");

      // Show segmented image + center of mass BB detection window
      cv::imshow( dirAccess.stroke_list.at(Fcounter), imageProc.RawMeanBlob );
      cv::waitKey(10);
      }

      // Make the prediction using $1 Multistroke recognizer
      classification.Classify(classification.DecisionPoints, "Predict");

      // Wait for the user to press Key and proceed to the next geasture
      cv::imshow( dirAccess.stroke_list.at(Fcounter), imageProc.RawMeanBlob );
      cv::waitKey(0);

      // Clear the vectors ...
      dirAccess.DepthPath.clear();
      dirAccess.DepthImgs.clear();
      }

  return 0;
}
