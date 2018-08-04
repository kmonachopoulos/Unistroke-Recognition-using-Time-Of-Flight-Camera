/*
------------------------------------------------------------------------------------------------------------------------------------------
Project       : Depth Stroke Recognition
File          : DirAccess.h
Description   : Directory access. Stores the dir and the file names.
Author        : Konstantinos Monachopoulos
------------------------------------------------------------------------------------------------------------------------------------------
*/
#pragma once

/*
----------------------------------------------------------------------------------------------------------------------------------------
Includes
----------------------------------------------------------------------------------------------------------------------------------------
*/
#include <vector>
#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>

/*
--------------------------------------------------------------------------------------------------
Class Declaration
--------------------------------------------------------------------------------------------------
*/
class DirAccess {
public:
    DirAccess(); //	Class constructor
    void GetPaths(int Fcounter); //	Stores paths to vectors
    void Getimages(void); //	Stores images to vectors

    const char* Dirname_; //	Constant predefined list of strokes
    short int Num_Of_Folders; //	Number of test folders

    std::vector<std::string> DepthPath; //	Holds depth path folders - strokes
    std::vector<cv::Mat> DepthImgs; //	Holds depth images
    std::vector<const char*> stroke_list; //	List of stroke names

private:
    DIR* dir; //	Type of directory stream that defines structure dirent
    std::string last; //	Holds the last character of the path for appending or avoiding navigation symbols
    std::string StrPth; //	Holds the path to a string
    struct dirent* ent; //	Holds serial number file ,type of entry
};
