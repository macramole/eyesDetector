#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "CurrentRectangle.h"

class ofApp : public ofBaseApp{

	public:
        ofVideoPlayer player;
        ofxCvHaarFinder cvFinder;

        const float FINDER_SCALE_FACTOR = 1.1;
        const int FINDER_NEIGHBORS = 4;
        const int FINDER_MIN_WIDTH = 80;
        const int FINDER_MIN_HEIGHT = 15;

		int VIDEO_WIDTH;
		int VIDEO_HEIGHT;
		const float VIDEO_SKIP_SPEED = 3;
		const float VIDEO_MATCH_SPEED = 1;
		float currentSpeed = VIDEO_MATCH_SPEED;
		const bool ANALYZE_MODE = false;

		const string OUTPUT_DIRECTORY = "recorder/";

		/*
			Track and record frames to OUTPUT_DIRECTORY
			if age of consent is reached
		*/
		bool enableRecordEyes = true;
		void recordEyes();
		void recordCurrentFrame(int currentFrame);
		void recordCurrentFrame();
		void checkLastRecord();
		string recordConstructPath(int frameNumber);
		const int RECORD_FORCE_WIDTH = 440;
		vector<int> recordedFrames;
		int recordedChunks = 0;
		int discardedChunks = 0;
		int recordedFramesAll = 0;

		// bool saveFrame = false;

		CurrentRectangle currentRectangle;
		// int currentRectangleFrames = 0;
		// int currentRectangleHadRectanglesInside = false;
		int framesWithoutEyes = 0;

		ofTrueTypeFont font;

        void setup();
		void update();
		void draw();

		void checkNoEyes();

		void drawRaw();
		void drawVideo();
		void drawCurrentRectangle();
		void drawEyes();
		void drawCentroPromedio();
		void drawProgress();

		void processCurrentRectangle();
		bool isAnyRectangleInsideCurrentRectangle();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		vector<string> arguments;

};
