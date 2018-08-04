/*
------------------------------------------------------------------------------------------------------------------------------------------
Project       : Depth Stroke Recognition
File          : ImgProc.cpp
Description   : Image processing functions
Author        : Konstantinos Monachopoulos
------------------------------------------------------------------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------------------------
Includes
--------------------------------------------------------------------------------------------------
*/

#include "ImageProcessing.h"
#include <numeric> //	For vector accumulation

/*
--------------------------------------------------------------------------------------------------
Functions definition
--------------------------------------------------------------------------------------------------
*/

ImageProc::ImageProc()
{
    //	Initialize all elements to UINT16_MAX (max depth distance) - CV_16UC1 => 16 - bits , 1 channel
    RawThreshImg = cv::Mat(240, 320, CV_16UC1, cv::Scalar::all(UINT16_MAX));
    RawWriter = cv::Mat(240, 320, CV_16UC3, cv::Scalar::all(UINT16_MAX));
    RawMeanBlob = cv::Mat(240, 320, CV_16UC3, cv::Scalar::all(UINT16_MAX));

    //	Hist, CumHist, HistEqu variables
    flag = false;
    EquIndex = 0;
    RandThresInit = 0;
    NewThresh = 0;
    Adder = 0;
}

// -------------------------------------------------------------------------------
// Calculates optimized threshold using histogram equilization by iterations
// -------------------------------------------------------------------------------
void ImageProc::HistEqu(cv::Mat* ImgSrc, int32_t* Thres, uint16_t bins)
{
    this->Hist(ImgSrc, &Equhist, bins); //	Calculate histogram
    this->CumHist(&Equhist, &EquCumHist); //	Calculate cumulative histogram
    RandThresInit = 0; //	Initialize Random Threshold

    for (int i = 0; i < discValues.cols - 1; i++) { //	Find the sum of discrete values matrix
        RandThresInit = RandThresInit + (uint32_t)discValues.at<uint16_t>(i);
    }

    RandThresInit = RandThresInit / discValues.cols; //	Calculate the init threshold (The mean value)

    while (NewThresh != RandThresInit) {
        NewThresh = RandThresInit; //	In every loop update the threshold with the new threshold
        EquIndex = 0;
        Adder = 0;

        for (int16_t i = 0; i < bins; i++) { // Find how many values in histogram are below the threshold.
            if (discValues.at<uint16_t>(i) < RandThresInit) {
                EquIndex += 1;
            }
        }

        if (EquIndex == 0)
            EquIndex = 1;

        // Dynamic allocate matrices for R1 histogram and R1 cumulative histogram
        cv::Mat R1_Histogram = cv::Mat::zeros(1, EquIndex, CV_32S);
        cv::Mat R1_Cumulative = cv::Mat::zeros(1, EquIndex, CV_32S);

        //	Fill R1 histogram with values that belong below the threshold
        for (int16_t i = 0; i < EquIndex; i++) {
            R1_Histogram.at<int32_t>(i) = Equhist.at<int32_t>(i);
        }
        this->CumHist(&R1_Histogram, &R1_Cumulative); //	Calculate cumulative histogram (for normalization)

        //	Multiplay each bin value with the corresponding depths
        for (int16_t i = 0; i < EquIndex; i++) {
            Adder = (R1_Histogram.at<int32_t>(i) * discValues.at<uint16_t>(i)) + Adder;
        }

        //	Calculate center (mean value) of R1 histogram (Normalized)
        m1 = (float)Adder / R1_Cumulative.at<int32_t>(EquIndex - 1);
        EquIndex = 0;
        Adder = 0;

        // Find how many values in histogram are over or equal to the threshold.
        for (int16_t i = 0; i < bins; i++) {
            if (discValues.at<uint16_t>(i) >= RandThresInit) {
                EquIndex += 1;
            }
        }

        if (EquIndex == 0)
            EquIndex = 1;

        // Dynamic allocate matrices for R2 histogram and R2 cumulative histogram
        cv::Mat R2_Histogram = cv::Mat::zeros(1, EquIndex, CV_32S);
        cv::Mat R2_Cumulative = cv::Mat::zeros(1, EquIndex, CV_32S);

        //	Fill R2 histogram with values that belong over or equal to the threshold
        for (int16_t i = R1_Histogram.cols; i < bins; i++) {
            R2_Histogram.at<int32_t>(i - R1_Histogram.cols) = Equhist.at<int32_t>(i);
        }
        this->CumHist(&R2_Histogram, &R2_Cumulative); //	Calculate cumulative histogram (for normalization)

        for (int16_t i = R1_Histogram.cols; i < discValues.cols; i++) {
            Adder = (R2_Histogram.at<int32_t>(i - R1_Histogram.cols) * discValues.at<uint16_t>(i)) + Adder;
        }

        //	Calculate center (mean value) of R1 histogram (Normalized)
        m2 = (float)Adder / R2_Cumulative.at<int32_t>(EquIndex - 1);

        //	Calculate mean of means (new threshlod)
        RandThresInit = (int32_t)(m1 + m2) / 2;

        //	Free the matrices from memory for re-allocation
        R1_Histogram.release();
        R2_Histogram.release();
        R1_Cumulative.release();
        R2_Cumulative.release();
    }
    *Thres = RandThresInit; //	Return the address of the final threshold
    return;
}

// -------------------------------------------------------------------------------
// Calculates histogram of a uint16 image using bins. Works like calcHist()
// -------------------------------------------------------------------------------
void ImageProc::Hist(cv::Mat* ImgSrc, cv::Mat* HistDst, uint16_t bins)
{
    cv::Mat HistDstTmp = cv::Mat::zeros(1, bins, CV_32S); //	Range ‭(-2147483648~2147483647)‬ Initialize the hist Mat to all zeros (size of bins - Matlab style)
    discValues = cv::Mat::zeros(1, bins, CV_16UC1); //	Initialize the discrete values zeros (size of bins)
    cv::minMaxLoc((*ImgSrc), &min, &max); //	Find the maximum and the minimum value of source image to fix the range of Histogram
    steps = max / (bins); //	Steps hold the intermediate hist values for comparison
    discValues.at<uint16_t>(0) = 1; //	Discrete values first element to one (avoid zero counting in histogram)

    for (int index_ = 1; index_ < bins; index_++) //	Fill discValues with step gaps
        discValues.at<uint16_t>(index_) = (uint16_t)(index_ * steps);

    for (int r = 0; r < ImgSrc->rows; r++) { //	Fill the histogram values
        for (int c = 0; c < ImgSrc->cols; c++) {
            flag = false;

            for (Index = 0; Index < bins - 1; Index++) { //	For every bin (until bin-1) check if the pixels falls in the gap
                if (ImgSrc->at<uint16_t>(r, c) >= discValues.at<uint16_t>(Index) && ImgSrc->at<uint16_t>(r, c) < discValues.at<uint16_t>(Index + 1)) {
                    HistDstTmp.at<int32_t>(Index) += 1; //	(int32_t <=> CV_32S) increase the histogram in the specific gap
                    flag = true;
                }
            }

            if (flag == false) { //	If flag is false, it means that the pixel value did not fall in the gap (Index increased in the last for loop)
                HistDstTmp.at<int32_t>(Index) += 1; //	(int32_t=CV_32S) So it belongs to the final most right gap
            }
        }
    }
    (*HistDst) = HistDstTmp;
    HistDstTmp.release();
    return;
}

// ------------------------------------
// Calculates the cumulative histogram
// ------------------------------------
void ImageProc::CumHist(cv::Mat* HistSrc, cv::Mat* CumHistDst)
{
    (*HistSrc).copyTo(*CumHistDst); //	Copy the source histogram to the cumulative histogram
    for (Index = 1; Index < HistSrc->cols; Index++) { //	For every bin add the current src and the previous dst histograms
        CumHistDst->at<int32_t>(Index) = HistSrc->at<int32_t>(Index) + CumHistDst->at<int32_t>(Index - 1);
    }
    return;
}


// ------------------------------------
// Threshhold implementation
// ------------------------------------
void ImageProc::Thres(cv::Mat& ImgSrc, cv::Mat& ImgDest, int32_t* Thres)
{
    ImgSrc.copyTo(ImgDest);
    for (int r = 0; r < ImgSrc.rows; r++) { //	Threshold ImgSrcCopy using Thres & copy the crusial pixels from ImgSrc to ImgDest
        for (int c = 0; c < ImgSrc.cols; c++) {
            if (ImgSrc.at<uint16_t>(r, c) < (*Thres - 14800)) { // Constant threshold should be adjusted with respect to image depths
                ImgDest.at<uint16_t>(r, c) = UINT16_MAX;
            }
            else {
                ImgDest.at<uint16_t>(r, c) = ImgSrc.at<uint16_t>(r, c);
            }
        }
    }
    return;
}

// --------------------------------------------------------
// Blob detection by mean of means through blocking
// --------------------------------------------------------
void ImageProc::BlobAnalysis(cv::Mat& ImgSrc, cv::Mat& ImgDest, std::vector<double> &AverageXY)
{
    //	Region of interest of the image
    cv::Rect ROI_I = cv::Rect(ImgSrc.rows - (ImgSrc.rows - 20), ImgSrc.cols - (ImgSrc.cols - 20), (ImgSrc.cols - 40), (ImgSrc.rows - 40));
    cv::Mat CropedROI_I = cv::Mat::zeros((ImgSrc.cols - 80), (ImgSrc.rows - 80), CV_16U); //	Width,	Height
    CropedROI_I = ImgSrc(ROI_I);

		AverageXY.clear();
		//AverageXY[1].clear();

    GlobalMean = 0; //	Minmum mean distance

    for (int Nx = 16; Nx < 18; Nx++) { //	For small areas use 6~9. Higher Nx,Ny mean very dense small regions
        for (int Ny = 16; Ny < 18; Ny++) {

            Stepx = (CropedROI_I.cols) / (2 * Nx); //	Step size
            Stepy = (CropedROI_I.rows) / (2 * Ny); //	Step size
            BlockH = (CropedROI_I.rows) / Ny; //	High of the block
            BlockW = (CropedROI_I.cols) / Nx; //	Width of the block
            Endx = (2 * Nx) - 1; //	End of cols
            Endy = (2 * Ny) - 1; //	End of rows

            for (int x = 0; x < Endx; x++) {
                for (int y = 0; y < Endy; y++) {

                    //	Crop the block
                    SubImageROI = cv::Rect(x * Stepx, y * Stepy, BlockW, BlockH);
                    cv::Mat CropedBlock = cv::Mat::zeros(BlockH, BlockW, CV_16U);
                    CropedBlock = CropedROI_I(SubImageROI);

                    //	Mean Value of the current block
                    MeanAdder = 0;
                    for (int k = 0; k < CropedBlock.rows; k++) {
                        for (int l = 1; l < CropedBlock.cols; l++) {
                            MeanAdder += CropedBlock.at<uint16_t>(k, l);
                        }
                    }

                    LocalMean = MeanAdder / (CropedBlock.rows + CropedBlock.cols); //	Find the local mean

                    //	Find the closest area (minimum distance)
                    if (LocalMean > GlobalMean) {
                        GlobalMean = LocalMean; //	Local extrema
                        MeanCoor[0] = x * Stepx; //	Catch the coordinates
                        MeanCoor[1] = y * Stepy;
                    }

                    CropedBlock.release(); //	Release memory of block image
                }
            }

            //	Store the outcomes to vectors
            AverageX.push_back(MeanCoor[0]);
            AverageY.push_back(MeanCoor[1]);
            AverageGMean.push_back(GlobalMean);
        }
    }

    //	Find the average of averages.
    AccAverageGMean = (double)(std::accumulate(AverageGMean.begin(), AverageGMean.end(), 0) / AverageGMean.size()); //	Global extrema (Average of means)
    AccAverageY = (double)(std::accumulate(AverageY.begin(), AverageY.end(), 0) / AverageY.size()); //	Average col
    AccAverageX = (double)(std::accumulate(AverageX.begin(), AverageX.end(), 0) / AverageX.size()); //	Average row

    AverageX.clear(); //	Release memory of average x
    AverageY.clear(); //	Release memory of average y
    AverageGMean.clear(); //	Release memory of mean value

    ImgSrc.copyTo(ImgDest); //	To (return) update the image in case it wont get into if statement

		//AverageXY.push_back(AccAverageX + 20);
		//AverageXY.push_back(AccAverageY + 20);


    if (AccAverageGMean > 350000) { //	old value 400000	//	fixed -   - Look for small regions

				AverageXY.push_back(AccAverageX + 20);
				AverageXY.push_back(AccAverageY + 20);

        cv::Rect R;
				R = { (uint16_t)AverageXY.at(0) - 15, (uint16_t)AverageXY.at(1) - 15, 50, 50 }; //	 For the rectangle

        cv::Point textOrg;
        textOrg = { (uint16_t)AverageXY.at(0) - 5, (uint16_t)AverageXY.at(1) - 25 }; //	coordinates for the text

        // POI print options
        std::cout << "Desicion points" <<  AverageXY.at(0) << "  " << AverageXY.at(1) <<'\n';
        //std::cout << "path.push_back(Point2D(" << AverageXY.at(0) << ", "<< AverageXY.at(1) << "));" << std::endl;

        cv::cvtColor(ImgDest, ImgDest, CV_GRAY2RGB, 3);	//	Convert the image to 3-Channel
        cv::rectangle(ImgDest, R, cv::Scalar(41120, 19000, 29000), 2, cv::LINE_8, 0);	//	Draw Rectangle
        cv::putText(ImgDest, "Center of Mass", textOrg, cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(6000, 11120, 41000), 1.5, 8);	//	Show Text
    }else{
		}
}
