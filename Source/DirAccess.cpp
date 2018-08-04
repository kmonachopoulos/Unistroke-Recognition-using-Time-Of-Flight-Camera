/*
------------------------------------------------------------------------------------------------------------------------------------------
Project       : Depth Stroke Recognition
File          : DirAccess.cpp
Description   : Directory access. Stores the dir and the file names.
Author        : Konstantinos Monachopoulos
------------------------------------------------------------------------------------------------------------------------------------------
*/
/*
----------------------------------------------------------------------------------------------------------------------------------------
Includes
----------------------------------------------------------------------------------------------------------------------------------------
*/
#include "DirAccess.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;

/*
--------------------------------------------------------------------------------------------------
Functions definition
--------------------------------------------------------------------------------------------------
*/
DirAccess::DirAccess()
    : Dirname_("./Data") //	Constant dir name
{
    // Hand stroke use cases  //

    // Brackets
    stroke_list.push_back("rightSquare_brackets_0");  // Miss-classified
    stroke_list.push_back("rightSquare_brackets_1");  // Classified
    stroke_list.push_back("rightSquare_brackets_2");  // Miss-classified
    stroke_list.push_back("rightSquare_brackets_3");  // Classified
    stroke_list.push_back("rightSquare_brackets_4");  // Classified
    stroke_list.push_back("rightSquare_brackets_5");  // Classified

    //  Circle
    stroke_list.push_back("circle_ccw_0");  // Classified
    stroke_list.push_back("circle_ccw_1");  // Classified
    stroke_list.push_back("circle_ccw_2"); // Classified
    stroke_list.push_back("circle_ccw_4"); // Miss-classified
    stroke_list.push_back("circle_ccw_5"); // Miss-classified

    //  Circle sequence
    stroke_list.push_back("circle_sequence");  // Classified

    //  Rectangle
    stroke_list.push_back("rectangle_ccw_0"); // Classified
    stroke_list.push_back("rectangle_ccw_1");// Miss-classified
    stroke_list.push_back("rectangle_ccw_2");// Classified
    stroke_list.push_back("rectangle_ccw_3");// Classified
    stroke_list.push_back("rectangle_cw_0"); // Classified
    stroke_list.push_back("rectangle_cw_1"); // Classified
    stroke_list.push_back("rectangle_cw_2"); // Miss-classified

    // Star
    stroke_list.push_back("star_0"); // Classified
    stroke_list.push_back("star_1");// Classified

    // ZigZag
    stroke_list.push_back("zigzag_0");  // Classified
    stroke_list.push_back("zigzag_1"); // Classified

    //  Triangle
    stroke_list.push_back("triangle_ccw_0");// Classified
    stroke_list.push_back("triangle_ccw_1");// Classified
    stroke_list.push_back("triangle_ccw_2");// Classified
    stroke_list.push_back("triangle_ccw_3");// Classified
    stroke_list.push_back("triangle_ccw_4");// Classified

    //	Find the total number of folders (for iterations)
    Num_Of_Folders = stroke_list.size();
};

// Stores Depth images to a vector
void DirAccess::Getimages(void)
{
    for (int i = 0; i < DepthPath.size(); i++) {
        DepthImgs.push_back(cv::imread(DepthPath.at(i), CV_LOAD_IMAGE_ANYDEPTH));
    }
}

//	Stores the directory and the file names in vectors
void DirAccess::GetPaths(int Fcounter)
{
    //	Initialize buffer and append the buffer to complete the path
    StrPth = Dirname_;
    StrPth.append("/");
    StrPth.append(stroke_list.at(Fcounter)); //	Append foldername
    dir = opendir(StrPth.c_str()); //	Open directory stream - Treat StrPth buffer as const char* argument

    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {

            //	Initialize the buffer and append the buffer to complete the path
            StrPth = Dirname_;
            StrPth.append("/");
            StrPth.append(stroke_list.at(Fcounter)); //	Append foldername

            //	Check for navigation symbols '.' && '..' and avoid appending
            if (strcmp(ent->d_name, ".") != false && strcmp(ent->d_name, "..") != false) {
                last = StrPth.substr(StrPth.length() - 1); //	Get the last character

                //	Append directory separator if not already there, check for the last character
                if (last.compare(":") != false || last.compare("/") != false || last.compare("/") != false) {
                    StrPth.append("/");
                    StrPth.append(ent->d_name);
                }

                //	Decide what to do with the type of entry
                switch (ent->d_type) {
                case DT_LNK:
                case DT_DIR:
                case DT_REG:

                    DepthPath.push_back(StrPth);
                    StrPth.clear(); // Clear the buffer from the annexations to reuse it
                default:
                    //NOP;
                    break;
                }
            }
        }
    }
}
