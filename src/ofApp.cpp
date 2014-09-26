#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofDirectory data("");
    data.listDir();
    vector<ofFile> files = data.getFiles();
    vector<string> filenames(files.size());
    for(auto it : files){
        filenames.push_back(it.getFileName());
    }
    gui = new ofxUICanvas();
    ddl = gui->addDropDownList("tiling", filenames);
    ddl->setAllowMultiple(false);
    rotateOffA = rotateOffB = 0;
    gui->addSlider("rotateOffA", 0, -1.5, &rotateOffA);
    gui->addSlider("rotateOffB", 0, -1.5, &rotateOffB);
    aimJump = 1;
    gui->addSlider("aimJump", 1, 2, &aimJump);
    showSVG = false;
    gui->addToggle("showSVG", &showSVG);
    showSVG = true;
    gui->addToggle("showGenerated", &showGenerated);
    numTile = ofxUIVec3f(1, 1);
    gui->add2DPad("numTiles", ofxUIVec3f(1, 100), ofxUIVec3f(1, 100), &numTile);
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    
    tileOffsetH = ofPoint(0, 0);
    tileOffsetV = ofPoint(0, 0);
    //svgSource.load("6_tile_tile.svg");//"hex_tile.svg");//
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "rotateOffA" || e.getName() == "rotateOffB" || e.getName() == "aimJump"){
        updateSVG = true;
    } else if (e.getName() == "tiling"){
        //selected a new tiling
        vector<string> selected = ddl->getSelectedNames();
        if (selected.size() <= 0){
            return;
        }
        string newFile = selected[0];
        svgSource = ofxSVG();
        svgSource.load(newFile);
        tileOffsetH = ofPoint(0, 0);
        tileOffsetV = ofPoint(0, 0);
        for (int i = 0; i < svgSource.getNumPath(); i++){
            ofPath p = svgSource.getPathAt(i);
            // svg defaults to non zero winding which doesn't look so good as contours
            //need to do this in order to get outlines!
            p.setPolyWindingMode(OF_POLY_WINDING_ODD);
            
            ofPoint currCenter;
            vector<ofPolyline> pl = p.getOutline();
            if (p.getStrokeWidth() == 0){
                //this is a tiling guide
                //I'm being silly and assuming the horizontal tiling is defined first...
                ofPoint tileOffsetTemp = p.getOutline()[0].getVertices()[1] - p.getOutline()[0].getVertices()[0];
                if (tileOffsetH.x == 0 && tileOffsetH.y == 0){
                    tileOffsetH = tileOffsetTemp;
                } else {
                    tileOffsetV = tileOffsetTemp;
                }
            } else {
                for(auto it : pl){
                    currCenter = it.getCentroid2D();
                    if (i == 0){
                        minX = currCenter.x;
                        maxX = currCenter.x;
                    } else {
                        minX = MIN(minX, currCenter.x);
                        maxX = MAX(maxX, currCenter.x);
                    }
                    centers.push_back(currCenter);
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPoint currOffset(0, 0);
    float currMaxX = maxX + tileOffsetH.x * ((int)numTile.x);
    for(int x = 0; x < (int)numTile.x; x++){
        for(int y = 0; y < (int)numTile.y; y++){
            ofTranslate(currOffset);
            if (showSVG){
                svgSource.draw();
            }
            if (showGenerated){
                for(int i = 0; i < svgSource.getNumPath(); i++){
                    ofPath p = svgSource.getPathAt(i);
                    if (p.getStrokeWidth() == 0){
                        continue;
                    }
                    //        //draw svg for reference
                    //        p.setUseShapeColor(false);
                    //        p.setFilled(false);
                    //        ofSetColor(0, 0, 0);
                    //        p.draw();
                    
                    ofSetColor(0, 0, 0);
                    //p.setFillColor(ofColor(ofRandom(255)));
                    vector<ofPolyline> pl = p.getOutline();
                    for(auto it : pl){
                        double ratioRight = (it.getCentroid2D().x + currOffset.x - minX) / (currMaxX - minX);
                        double rotateOff = rotateOffB * ratioRight + rotateOffA * (1 - ratioRight);
                        
                        for(int i = 0; i < it.size(); i++){
                            
                            //each line is point i -> point i+1
                            ofPoint A = it[i];
                            ofPoint B = it[(i+1)%it.size()];
                            ofPoint C = it[(i+((int)aimJump))%it.size()];
                            ofPoint D = it[(i+((int)aimJump)+1)%it.size()];
                            ofPoint E = (A+B)/2;
                            ofPoint F = (C+D)/2;
                            ofPoint G = (E+F)/2;
                            
                            //get angle of line
                            ofPoint bOffset = B - A;
                            double ABa = atan2(bOffset.x, bOffset.y);
                            ofPoint gOffset = G - E;
                            double EGa = atan2(gOffset.x, gOffset.y);
                            
                            double GZa = EGa - 1.5;
                            double GZl = tan(EGa - ABa - rotateOff)*ofDist(E.x, E.y, G.x, G.y);
                            ofPoint Z = G+ofPoint(sin(GZa), cos(GZa))*GZl;
                            
                            ofLine(E, Z);
                            ofLine(F, Z);
                            //draw short line of angle + x radian
                            // ofLine(it[i], it[i] + ofPoint(sin(ABa+rotateOff)*10, cos(ABa+rotateOff)*10));
                        }
                    }
                }
            }
            ofTranslate(-currOffset);
            currOffset += tileOffsetV;
        }
        currOffset -= tileOffsetV * ((int)numTile.y);
        currOffset += tileOffsetH;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
