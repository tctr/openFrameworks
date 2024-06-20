#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"

#include "ofxOpenCv.h"
#include <vector>

//UNCOMMENT TO USE CAMERA. CAMERA ONLY WORKS ON DEVICE NOT SIMULATOR
#define USE_CAMERA

class ofApp : public ofxAndroidApp{
	
	public:
		
		void setup();
		void exit();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();

        void deviceRefreshRateChanged(int refreshRate);
        void deviceHighestRefreshRateChanged(int refreshRate);
        void deviceRefreshRateChangedEvent(int &refreshRate);
        void deviceHighestRefreshRateChangedEvent(int & refreshRate);
				
		ofImage img;
		
		#ifdef USE_CAMERA
			ofVideoGrabber grabber;
		#endif
		ofxCvColorImage colorCv;
		ofxCvColorImage colorCvSmall;
		ofxCvGrayscaleImage grayCv;
		ofxCvHaarFinder faceFinder;
		ofImage colorImg;
		
		std::vector<ofxCvBlob> faces;

		int one_second_time;
		int camera_fps;
		int frames_one_sec;
};