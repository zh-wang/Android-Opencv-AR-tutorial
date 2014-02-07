#include <jni.h>
#include <android/log.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

#include <sstream>
#include <iostream>
#include <string>

#define LOG_TAG "android.zh.wang.CameraNativeTest@jni_part"
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

const int CUBE_SIZE = 5;
const int CHESS_SIZE = 25;


string IntToString(int number)
{
  stringstream ss;
  ss << number;
  return ss.str();
}


inline double dist(Point a, Point b){
    return sqrt( (a.x-b.x) * (a.x-b.x) + (a.y-b.y) * (a.y-b.y) );
}

class Cube{
public :
    vector<Point3f> srcPoints3D;
    vector<Point2f> dstPoints2D;
    Cube(){
        LOGI("cube init starts");
        float CUBExCHESS = CUBE_SIZE * CHESS_SIZE;
        for(int i=0;i<8;i++){
            switch(i){
                case 0:
                    srcPoints3D.push_back(Point3f(0,0,0));
                    break;
                case 1:
                    srcPoints3D.push_back(Point3f(CUBExCHESS,0,0));
                    break;
                case 2:
                    srcPoints3D.push_back(Point3f(CUBExCHESS,CUBExCHESS,0));
                    break;
                case 3:
                    srcPoints3D.push_back(Point3f(0,CUBExCHESS,0));
                    break;
                case 4:
                    srcPoints3D.push_back(Point3f(0,0,CUBExCHESS));
                    break;
                case 5:
                    srcPoints3D.push_back(Point3f(CUBExCHESS,0,CUBExCHESS));
                    break;
                case 6:
                    srcPoints3D.push_back(Point3f(CUBExCHESS,CUBExCHESS,CUBExCHESS));
                    break;
                case 7:
                    srcPoints3D.push_back(Point3f(0,CUBExCHESS,CUBExCHESS));
                    break;
                default:
                    break;
            }
        }
        LOGI("cube init ends");
    }
};


class CameraCalibrator{
    bool isCal, isRead;
    vector< vector<Point3f> > objectPoints;
    vector< vector<Point2f> > imagePoints;
    vector< vector<Point2f> > contoursFinded;
    vector<Point2f> imageCorners;
    vector<Point3f> objectCorners, contoursFindedObjectPoints;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    int flag;
    bool mustInitUndistort;
    Size boardSize;
    int successes;
    Mat rvecs, tvecs;
    vector<cv::Mat> rvecs_list, tvecs_list;
    Point sPoint, tPoint;
    
    public:
    int MAX_POINT_SIZE;
	CameraCalibrator() :
			flag(0), mustInitUndistort(true) {
        LOGI("cameraCalibrator init starts");
		isCal = isRead = false;
		successes = 0;
		boardSize = Size(7, 7);
		for (int i = 0; i < boardSize.height; i++) {
			for (int j = 0; j < boardSize.width; j++) {
				objectCorners.push_back(
                    Point3f(i * CHESS_SIZE, j * CHESS_SIZE, 0.0f));
			}
		}
		
		int x = 100;
		
		contoursFindedObjectPoints.push_back(
            Point3f(0, 0, 0.0f));
        contoursFindedObjectPoints.push_back(
            Point3f(0, x, 0.0f));
        contoursFindedObjectPoints.push_back(
            Point3f(x, x, 0.0f));
        contoursFindedObjectPoints.push_back(
            Point3f(x, 0, 0.0f));

		MAX_POINT_SIZE = 10;
		LOGI("cameraCalibrator init ends");
	}
	;

	void addChessboardPoints(Mat image, Mat mgray) {
		LOGI("add chessboard points");

        LOGI("find feature points");
        vector<KeyPoint> v;

        FastFeatureDetector detector(50);
        detector.detect(mgray, v);
        for( unsigned int i = 0; i < v.size(); i++ )
        {
            const KeyPoint& kp = v[i];
            circle(image, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
        }

		bool found = cv::findChessboardCorners(
				mgray,
				boardSize,
				imageCorners,
				CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE
						+ CV_CALIB_CB_FAST_CHECK);
		if (found) {
			LOGI("found");
			cv::cornerSubPix(
					mgray,
					imageCorners,
					cv::Size(5, 5),
					cv::Size(-1, -1),
					cv::TermCriteria(
							cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
							30, 0.1));
			if (imageCorners.size() == boardSize.area()) {
				addPoints(imageCorners, objectCorners);
				successes++;
			}
		}
	}
	
	 
    void drawProcessing(Mat image, Mat mgray, Size imageSize, Cube mCube){
        LOGI("drawProcessing starts");
        LOGI("find chess board");
        
        drawMarkerContours(image, mgray);

	    if(contoursFinded.size() > 0) {

            Mat m1(contoursFinded[0]);
            Mat m2(contoursFindedObjectPoints);
            
            LOGI("src size: %d", contoursFinded[0].size());
            LOGI("obj size: %d", contoursFindedObjectPoints.size());
            
			LOGI("solvePnP");
			cv::solvePnP(m2,
			             m1,
			             cameraMatrix,
			             distCoeffs,
			             rvecs,
			             tvecs);
			
			LOGI("projectPoints");
			projectPoints(mCube.srcPoints3D, rvecs, tvecs, cameraMatrix, distCoeffs, mCube.dstPoints2D);
			
			LOGI("points::::::::::::");
			for(int i=0;i<8;i++){
                LOGI("%f,%f,%f --> %f,%f", 
                    mCube.srcPoints3D[i].x, mCube.srcPoints3D[i].y, mCube.srcPoints3D[i].z,
                    mCube.dstPoints2D[i].x, mCube.dstPoints2D[i].y);
            }
			
			for(int i=0;i<2;i++){
                for(int j=0;j<4;j++){
                    if(j==3){
                        sPoint = mCube.dstPoints2D[i*4+j];
                        tPoint = mCube.dstPoints2D[i*4+0];
                    }
                    else {
                        sPoint = mCube.dstPoints2D[i*4+j];
                        tPoint = mCube.dstPoints2D[i*4+j+1];
                    }
                    cv::line(image, sPoint, tPoint, Scalar(0,255,0,255),2,8,0);
                }
            }
            
            for(int i=0;i<4;i++){
                sPoint = mCube.dstPoints2D[i];
                tPoint = mCube.dstPoints2D[i+4];
                cv::line(image, sPoint, tPoint, Scalar(0,255,0,255),2,8,0);
            }
            m1.release();
            m2.release();

        }
        LOGI("drawProcessing ends");
    }
    
    
    void drawProcessing2(Mat image, Mat mgray, Size imageSize, Cube mCube){
        LOGI("drawProcessing starts");
        LOGI("find chess board");
        bool found = cv::findChessboardCorners(
				mgray,
				boardSize,
				imageCorners,
				CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE
						+ CV_CALIB_CB_FAST_CHECK);
	    if(found) {
            LOGI("cornerSubPix");
            cv::putText(image, "foundChessBoard", Point(10,100), cv::FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0,0,255,255), 5 );
            cv::cornerSubPix(
					mgray,
					imageCorners,
					cv::Size(5, 5),
					cv::Size(-1, -1),
					cv::TermCriteria(
							cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
							30, 0.1));
							
			imagePoints.clear();
			objectPoints.clear();
			
			if (imageCorners.size() == boardSize.area()) {

                Mat m1(imageCorners);
                Mat m2(objectCorners);
                
                LOGI("src size: %d", imageCorners.size());
                LOGI("dst size: %d", objectCorners.size());
                
    			LOGI("solvePnP");
    			cv::solvePnP(m2,
    			             m1,
    			             cameraMatrix,
    			             distCoeffs,
    			             rvecs,
    			             tvecs);
    			
    			LOGI("projectPoints");
    			projectPoints(mCube.srcPoints3D, rvecs, tvecs, cameraMatrix, distCoeffs, mCube.dstPoints2D);
    			
    			LOGI("points::::::::::::");
    			for(int i=0;i<8;i++){
                    LOGI("%f,%f,%f --> %f,%f", 
                        mCube.srcPoints3D[i].x, mCube.srcPoints3D[i].y, mCube.srcPoints3D[i].z,
                        mCube.dstPoints2D[i].x, mCube.dstPoints2D[i].y);
                }
    			
    			for(int i=0;i<2;i++){
                    for(int j=0;j<4;j++){
                        if(j==3){
                            sPoint = mCube.dstPoints2D[i*4+j];
                            tPoint = mCube.dstPoints2D[i*4+0];
                        }
                        else {
                            sPoint = mCube.dstPoints2D[i*4+j];
                            tPoint = mCube.dstPoints2D[i*4+j+1];
                        }
                        cv::line(image, sPoint, tPoint, Scalar(0,255,0,255),2,8,0);
                    }
                }
                
                for(int i=0;i<4;i++){
                    sPoint = mCube.dstPoints2D[i];
                    tPoint = mCube.dstPoints2D[i+4];
                    cv::line(image, sPoint, tPoint, Scalar(0,255,0,255),2,8,0);
                }
                m1.release();
                m2.release();
            }
        }
        LOGI("drawProcessing ends");
    }
    

	void addPoints(const vector<Point2f> imageCorners,
			const vector<Point3f> objectCorners) {
		imagePoints.push_back(imageCorners);
		objectPoints.push_back(objectCorners);
	}

	int getSuccesses() {
		return successes;
	}

	bool getIsCal() {
		return isCal;
	}

	void setIsCal(bool b) {
		isCal = b;
	}
	
	bool getIsRead() {
        return isRead;
    }
    
    void setIsRead(bool b) {
        isRead = b;
    }

	double calibrate(Size imageSize) {
        LOGI("calibrate starts");
        if(objectPoints.size() != MAX_POINT_SIZE){
            for(int i=0;i<MAX_POINT_SIZE;i++)
                objectPoints.push_back(objectCorners);
        }
        LOGI("objectPoint.size: %d", objectPoints.size());
		mustInitUndistort = true;
		return calibrateCamera(objectPoints, // the 3D points
				imagePoints, // the image points
				imageSize, // image size
				cameraMatrix, // output camera matrix
				distCoeffs, // output distortion matrix
				rvecs_list, tvecs_list, // Rs, Ts
				flag); // set options
		LOGI("calibrate ends");
	}
	
	void savePoints(){
        LOGI("savePoints starts");
        FileStorage fs("/sdcard/CameraNativeTest/Points.xml", 
            FileStorage::WRITE);
        
        for(int i=0;i<MAX_POINT_SIZE;i++){
            Mat outMat(imagePoints[i]);
            string t = "POINT_MAP_" + IntToString(i);
            fs << t << outMat;
        }
        
        fs.release();
        LOGI("savePoints ends");
    }
    
    void saveCameraMatrix(){
        LOGI("saveCameraMatrix starts");
        FileStorage fs("/sdcard/CameraNativeTest/Points.xml", 
            FileStorage::WRITE);
        fs << "CAMERA_MATRIX" << cameraMatrix;
        fs << "DIST_COEFFS" << distCoeffs;
        LOGI("saveCameraMatrix ends");
    }
    
    bool readPoint(){
        LOGI("readPoints starts");
        FileStorage fs;
        fs.open("/sdcard/CameraNativeTest/Points.xml", 
            FileStorage::READ);
            
        if(!fs.isOpened()){
            LOGI("points file cannot be opened!!!");
            setIsRead(false);
            return isRead;
        }
        
        for(int i=0;i<MAX_POINT_SIZE;i++){
            Mat inMat;
            string t = "POINT_MAP_" + IntToString(i);
            fs[t] >> inMat;
            vector<Point2f> imageCornersRead = Mat_<Point2f>(inMat);
            imagePoints.push_back(imageCornersRead);
        }
        
        LOGI("imagePoints.size: %d", imagePoints.size());
        
        LOGI("readPoints ends");
        setIsRead(true);
        return isRead;
    }
    
    bool readCameraMatrix(){
        LOGI("readCameraMatrix starts");
        FileStorage fs;
        fs.open("/sdcard/CameraNativeTest/Points.xml", 
//        fs.open("/sdcard/CameraNativeTest/CameraMatrix.xml",
            FileStorage::READ);
            
        if(!fs.isOpened()){
            LOGI("camera matrix file cannot be opened!!!");
            setIsRead(false);
//            setIsCal(false);
            return isRead;
        }
        
        fs["CAMERA_MATRIX"] >> cameraMatrix;
        fs["DIST_COEFFS"] >> distCoeffs;
        
        LOGI("readCameraMatrix ends");
        setIsRead(true);
//        setIsCal(true);
        return isRead;
    }
    
    void drawMarkerContours(Mat image, Mat mgray){
        LOGI("drawMarkerContours starts");
        cv::Mat bin_img;
        cv::threshold(mgray, bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        
        cv::findContours(bin_img,
            contours,
            hierarchy,
            cv::RETR_TREE,
            cv::CHAIN_APPROX_SIMPLE);
            
        vector< vector< Point> >::iterator itc = contours.begin();
        for(;itc!=contours.end();){
            if(itc->size() <= 3) 
                itc = contours.erase(itc);
            else 
                ++itc;
        }
        
        
        LOGI("approx poly");
        vector< vector< Point> > contours_poly( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
        { 
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        }
        
        itc = contours_poly.begin();
        for(;itc!=contours_poly.end();){
            if(itc->size() != 4) 
                itc = contours_poly.erase(itc);
            else 
                ++itc;
        }
                
        
        LOGI("resize contours_poly");
        int csize = contours_poly.size();
        vector< Point > centers;
        vector< bool > isValid;
        for(int i=0;i<csize;i++){
            double x = 0, y = 0;
            for(int j=0;j<4;j++){
                x += contours_poly[i][j].x;
                y += contours_poly[i][j].y;
            }
            centers.push_back( Point( x/4, y/4 ) );
        }
        
        for(int i=0;i<csize;i++){
            bool ok = false;
            for(int j=0;j<csize;j++){
                if(i == j) continue;
                if(dist(centers[i], centers[j]) < 20){
                    ok = true;
                    break;
                }
            }
            if(ok) isValid.push_back(true);
            else isValid.push_back(false);
        }
        
        itc = contours_poly.begin();
        for(int i=0;itc!=contours_poly.end();++i){
            if(!isValid[i]) 
                itc = contours_poly.erase(itc);
            else 
                ++itc;
        }
        
        contoursFinded.clear();
        
        for(int i=0;i<contours_poly.size();i++){
            vector< Point2f > temp;
            for(int j=0;j<contours_poly[i].size();j++){
                temp.push_back( Point2f(contours_poly[i][j].x, contours_poly[i][j].y) );
            }
            contoursFinded.push_back(temp);
        }
        
        LOGI("draw contour");
        cv::drawContours(image,
            contours_poly,
            -1,
            Scalar(255,0,0,255),
            5);
        
        bin_img.release();
        LOGI("drawMarkerContours ends");
    }
   
};



CameraCalibrator mCameraCalibrator = CameraCalibrator();
Cube mCube = Cube();



extern "C" {
    
JNIEXPORT void JNICALL Java_zh_wang_android_opencv_ar_OpenCVARView_FindFeatures(JNIEnv* env, jobject thiz, jint width, jint height, jbyteArray yuv, jintArray bgra)
{
    LOGI("jni starts");
    jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
    jint*  _bgra = env->GetIntArrayElements(bgra, 0);

    Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
    Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
    Mat mgray(height, width, CV_8UC1, (unsigned char *)_yuv);

    //Please make attention about BGRA byte order
    //ARGB stored in java as int array becomes BGRA at native level
    cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);
    
    Size imageSize(width, height);
    
    // check for points data,
    // if we have no points data,
    // get 15 frames to create it.
    // Then set flat 'isRead' to true,
    // then skip this part
    LOGI("check for points data");
    
    if(!mCameraCalibrator.getIsRead()){
        mCameraCalibrator.readCameraMatrix();
        if(!mCameraCalibrator.getIsRead()){
        	LOGI("start camera calibrator");
            if(mCameraCalibrator.getSuccesses() < mCameraCalibrator.MAX_POINT_SIZE){
            	string calibratingProgress = "Calibrating success " +
                		IntToString(mCameraCalibrator.getSuccesses()) +
                				"/" + IntToString(mCameraCalibrator.MAX_POINT_SIZE);
                cv::putText(mbgra, calibratingProgress, Point(10,100), cv::FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0,0,255,255), 5 );
                mCameraCalibrator.addChessboardPoints(mbgra, mgray);
            } else {
                mCameraCalibrator.setIsRead(true);
                mCameraCalibrator.calibrate(imageSize);
                mCameraCalibrator.saveCameraMatrix();
            }
        }
    } else {
        // check for whether we calculate the
        // calibrator matrix,
        // if not, we calculate it and set the
        // flag 'isCal' to true
        // then skip this part
        mCameraCalibrator.drawProcessing(mbgra, mgray, imageSize, mCube);

    }
    
    
    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);
    LOGI("jni ends");
}


JNIEXPORT void JNICALL Java_zh_wang_android_opencv_ar_BitmapProcessing_GetCameraIntrisicParams(JNIEnv* env, jobject thiz)
{
    Size imageSize;
    for(int i=0;i<mCameraCalibrator.MAX_POINT_SIZE;i++){
        string s = "/sdcard/CameraNativeTest/cb" + IntToString(i) + ".jpg"; 
        cv::Mat image = cv::imread(s);
        string t = "image: " + s + " is read.";
        cv::resize(image, image, cv::Size(), 0.25, 0.25);
        t += "size: " + IntToString(image.size().width) + " " + IntToString(image.size().height);
        imageSize = Size(image.size().width, image.size().height);
        LOGI("%s", t.c_str());
        Mat gray(image.size().height, image.size().width, CV_8UC1);
        cv::cvtColor(image, gray, CV_BGR2GRAY);
        LOGI("start addPoints");
        mCameraCalibrator.addChessboardPoints(image, gray);
        image.release();
        gray.release();
    }
    LOGI("start calculating");
    mCameraCalibrator.calibrate(imageSize);
    mCameraCalibrator.saveCameraMatrix();
}


JNIEXPORT void JNICALL Java_zh_wang_android_opencv_ar_Homography_doHomography(JNIEnv* env, jobject thiz, jstring filename)
{
    LOGI("load image");

    const char *fnameptr = env->GetStringUTFChars(filename, NULL);
    LOGI(fnameptr);
    
    cv::Mat image = cv::imread(fnameptr);
    cv::resize(image, image, Size(640, 480));
    string t = "size: " + IntToString(image.size().width) + " " + IntToString(image.size().height);
    LOGI("%s", t.c_str());

    LOGI("find contour");
    
    cv::Mat gray_img, bin_img;
    cv::cvtColor(image, gray_img, CV_BGR2GRAY);
    cv::threshold(gray_img, bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
    
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::findContours(bin_img,
        contours,
        hierarchy,
        cv::RETR_TREE,
        cv::CHAIN_APPROX_SIMPLE);
        
    vector< vector< Point> >::iterator itc = contours.begin();
    for(;itc!=contours.end();){
        if(itc->size() <= 3) 
            itc = contours.erase(itc);
        else 
            ++itc;
    }
    
    cv::drawContours(image,
        contours,
        -1,
        Scalar(0,0,255,255),
        2);
    
    LOGI("approx poly");
    vector< vector< Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    { 
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    }
    
    itc = contours_poly.begin();
    for(;itc!=contours_poly.end();){
        if(itc->size() != 4) 
            itc = contours_poly.erase(itc);
        else 
            ++itc;
    }
            
    LOGI("draw contour");
    cv::drawContours(image,
        contours_poly,
        -1,
        Scalar(0,255,0,255),
        5);
    
    LOGI("resize contours_poly");
    int csize = contours_poly.size();
    vector< Point > centers;
    vector< bool > isValid;
    for(int i=0;i<csize;i++){
        double x = 0, y = 0;
        for(int j=0;j<4;j++){
            x += contours_poly[i][j].x;
            y += contours_poly[i][j].y;
        }
        centers.push_back( Point( x/4, y/4 ) );
    }
    
    for(int i=0;i<csize;i++){
        bool ok = false;
        for(int j=0;j<csize;j++){
            if(i == j) continue;
            if(dist(centers[i], centers[j]) < 10){
                ok = true;
                break;
            }
        }
        if(ok) isValid.push_back(true);
        else isValid.push_back(false);
    }
    
    itc = contours_poly.begin();
    for(int i=0;itc!=contours_poly.end();++i){
        if(!isValid[i]) 
            itc = contours_poly.erase(itc);
        else 
            ++itc;
    }
    
    LOGI("draw contour");
    cv::drawContours(image,
        contours_poly,
        -1,
        Scalar(255,0,0,255),
        5);
        
    
    
    LOGI("write image");
    cv::imwrite("/sdcard/CameraNativeTest/marker_after.png", image);
    
    env->ReleaseStringUTFChars(filename, fnameptr); 
}


}
