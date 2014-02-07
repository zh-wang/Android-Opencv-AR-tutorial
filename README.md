OpenCV AR (Argument Reality) Sample
===================

An sample AR application for android, using OpenCV.

Requirment:
1. An andorid device running 2.2 or above
2. OpenCV 2.4.x
3. android SDK and NDK, lastest version is recommanded.

You can get OpenCV from [Download OpenCV](http://opencv.org/downloads.html).  
Choose "OpenCV for Android".  

Before Building 
================
1. Setup your android SDK and NDK.
2. Edit jni/android.mk. Find the following line, then change the path of `OpenCV.mk` to your OpenCV's installation path.  

    include ../../OpenCV-2.4.8-android-sdk/sdk/native/jni/OpenCV.mk

3. Build the native source by `ndk-build`.
4. Build & Run by Eclipse or ant. 

* You __DO NOT__ need import "OpenCV Library 2.4.x" as a library project. This sample only use OpenCV's lib in native part.  

Instruction
================
* When the first time app is running, it will do calibration before AR projection. It will take 10 frames of __chessboard.png__ to make the calibration. The progress will be shown on screen as `calibrating success x / 10 `.  
* After that, you can use `use_this_marker.png` to check the AR projection. You will a blue contour on the detected marker, and a green cube on it. That's all. I'll add more features later.

Developed By
================
Zhenghong Wang - <viennakanon@gmail.com>

License
================
    Copyright 2014 Zhenghong Wang

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
