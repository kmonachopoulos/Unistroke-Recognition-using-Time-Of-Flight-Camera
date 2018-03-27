/*
------------------------------------------------------------------------------------------------------------------------------------------
Project       : Depth Stroke Recognition
File          : ImageProcessing.h
Description   : Image Processing functions
Author        : Konstantinos Monahopoulos
------------------------------------------------------------------------------------------------------------------------------------------
*/
#pragma once

/*
----------------------------------------------------------------------------------------------------------------------------------------
Includes
----------------------------------------------------------------------------------------------------------------------------------------
*/
#include <opencv2/opencv.hpp>
#include <array>

/*
--------------------------------------------------------------------------------------------------
Class Declaration
--------------------------------------------------------------------------------------------------
*/
class ImageProc {

public:
    ImageProc(); //	Class constructor
    void HistEqu(cv::Mat* ImgSrc, int32_t* Thres, uint16_t bins); //	Histogram equilization by iterations
    void Thres(cv::Mat& ImgSrc, cv::Mat& ImgDest, int32_t* Thres); //	Thresholding the image using histogram threshold as input
    void BlobAnalysis(cv::Mat& ImgSrc, cv::Mat& ImgDest, std::vector<double> &AverageXY); //	Detects the Hand and plots a rectangle

    cv::Mat RawWriter, RawThreshImg, RawBlob, RawMeanBlob;
    int32_t FinalThres;

private:
    void Hist(cv::Mat* ImgSrc, cv::Mat* HistDst, uint16_t bins); //	Calculates histogram of an uint16 image using bins. Works exactly like calcHist()
    void CumHist(cv::Mat* HistSrc, cv::Mat* CumHistDst); //	Calculates the cumulative histogram

    // ------------------------
    // Hist, CumHist variables
    // ------------------------
    cv::Mat discValues;
    bool flag;
    uint16_t Index;
    double steps, min, max;

    // ------------------------
    // HistEqu variables
    // ------------------------
    cv::Mat Equhist, EquCumHist;
    int32_t EquIndex, RandThresInit, NewThresh;
    float m1, m2;
    uint32_t Adder;

    // ------------------------
    // BlobDetect variables
    // ------------------------
    cv::Rect SubImageROI; //	Sub image - current block
    std::array<std::vector<double>, 2> AverageXY; //	Final coordinates for recognition
    std::vector<double> AverageX, AverageY, AverageGMean; //	Temp coordinates and Average values
    float GlobalMean; //	Max distance of an the object
    int Stepx, Stepy, BlockH, BlockW, Endx, Endy; //	Mean block variables
    float LocalMean, MeanAdder; //	Adder for mean value
    double MeanCoor[2]; //	Mean value coordinates
    float AccAverageGMean, AccAverageY, AccAverageX; //	Accumulated coordinates and Average values
};
