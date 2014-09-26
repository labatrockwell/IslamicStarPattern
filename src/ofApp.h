#pragma once

#include "ofMain.h"
#include "ofxSVG.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void guiEvent(ofxUIEventArgs &e);
    
    ofxSVG svgSource, svgResult;
    vector<ofPolyline> outlines;
    vector<ofPoint> centers;
    ofxUICanvas* gui;
    float rotateOffA, rotateOffB;
    float minX, maxX;
    float aimJump;
    bool showSVG, updateSVG, showGenerated;
    ofPoint tileOffsetH, tileOffsetV;
    ofxUIVec3f numTile;
    ofxUIDropDownList* ddl;
};
