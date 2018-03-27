### Introduction
This is a personal project that uses Time of flight (TOF) Depth images to classify the gestures using  $1 Unistroke Recognizer [http://depts.washington.edu/madlab/proj/dollar/]
Given a sequence of depth imagesrecorded from a DepthSense camera I created a C++ application that isolates the hand in the scene, computes the nearest object in-front
of the camera (hand) in the depth-image space), and uses the sequence of positions to classify the stroke.

### Dataset copyrights
I don't own the dataset copyrights I am just using the data for this project.

### Implementation Guide
###### Extract Dataset
'''
cd Data
tar -xvzf Data.tar.gz
'''

###### Dependencies
Install Opencv from source

###### How to Build
'''10.2.139.23
mkdir build
cd build
cmake ..
make
'''

###### How to Run
'''
cd <root project dir>
./bin/MultistrokeRec
'''

This will run the gestures example by example using the small dataset that I have.


### Algorithm

For each frame I calculate a segmentation threshold using Segmentation by iteration method. After the hand (which is the closest object on the camera) is
segmented, I calculate the mean of mass sequentially, by dividing the image into smaller parts and keeping the one with the bigger mass. The smallest
window will have the biggest probability to have the hand segmented inside. The devision proportion can be set manually. After the detection,
a bounding box is attached to the object and the coordinates start to recorded. When the recording is finished the coordinates are fed into the
$1 Unistroke Recognizer that is responsible to recognize the sequence.

I have changed the pattern coordinates of some recognition gestures and added some more to cover the needs of the project. The original sequence was recorded at 30 fps.
The gesture sequence is parameterized, so that the sequence (stroke) starts ~ at the first frame and finish at the last. The algorithm is not capable for now
to identify when a movement starts, but you can extend the functionality for that. This algorithm does not recognize the hand, but segments whatever objects
that are closest to the camera with respect to all other vissible objects.

### Sample Results

![visualize_sample_results](https://user-images.githubusercontent.com/3832904/37982756-5ac2bcee-31e9-11e8-9dcb-5b231f8b8856.png)

Identified 22/28 gestures. Accuracy : 0.7857142857142857

![alt tag](https://github.com/kmonachopoulos/Unistroke-Recognition-using-Time-Of-Flight-Camera/blob/master/Visualize_Sample_Results.png)

To improve the accuracy, you could create more accurate Decision points using even smaller division windows and store more representative movements to $1 Unistroke Recognizer.

1) In Source/ImageProcessing.cpp , comment line 284
2) In Source/ImageProcessing.cpp , uncomment line 285
3) Capture the coordinates of specific gesture (or calculate the average)
4) Replace in Header/SampleGestures.h the coordinates of the gesture
5) Re-build
