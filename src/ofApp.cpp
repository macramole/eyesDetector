#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    player.load(arguments.at(1));
    // player.setSpeed(0.2);
    player.play();
    player.setVolume(0);
    player.setLoopState(OF_LOOP_NONE);
    // player.setPosition( 0.45 ); //0.45 pequeños //0.165 empieza
    // player.setPosition( 0.165 ); //0.45 pequeños //0.165 empieza
    VIDEO_WIDTH = player.getWidth();
    VIDEO_HEIGHT = player.getHeight();

    cvFinder.setup("parojosG.xml");
    cvFinder.setNeighbors(FINDER_NEIGHBORS);
    cvFinder.setScaleHaar(FINDER_SCALE_FACTOR);

    font.loadFont("RobotoMonoRegular.ttf", 12);
    // ofSetFrameRate(2);
}

//--------------------------------------------------------------
void ofApp::update(){
    player.update();
    cvFinder.findHaarObjects(player.getPixels(), FINDER_MIN_WIDTH, FINDER_MIN_HEIGHT);
    // cvFinder.findHaarObjects(player.getPixels());
    currentRectangle.process( cvFinder.blobs );
    checkNoEyes();
}

void ofApp::checkNoEyes(){
    if ( cvFinder.blobs.size() == 0 ) {
        framesWithoutEyes++;
    } else {
        if ( framesWithoutEyes != 0 ) {
            player.setSpeed(VIDEO_MATCH_SPEED);
        }
        framesWithoutEyes = 0;
    }

    if ( framesWithoutEyes == 100 ) {
        player.setSpeed(VIDEO_SKIP_SPEED);
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);

    ofNoFill();
    drawVideo();
    drawRaw();
    drawCurrentRectangle();
    drawEyes();
    drawCentroPromedio();
}

void ofApp::drawVideo() {
    ofSetColor(255);
    player.draw(0,0);
}

void ofApp::drawRaw() {
    ofSetColor(0,255,0);
    for(int i = 0; i < cvFinder.blobs.size(); i++) {
        ofRectangle rect = cvFinder.blobs[i].boundingRect;
        ofDrawRectangle( rect );
        font.drawString(
            ofToString(rect.width) + "x" + ofToString(rect.height),
            rect.getRight() - 40,
            rect.getBottom());
    }
}
void ofApp::drawCentroPromedio() {
    if ( currentRectangle.centroPromedio.length() > 0 ) {
        ofSetColor(255,0,0);
        ofFill();
        ofDrawCircle( currentRectangle.centroPromedio, 2.0 );
        ofNoFill();
    }
}

void ofApp::drawCurrentRectangle() {
    if ( !currentRectangle.isEmpty() ) {
        if ( currentRectangle.isCandidate ) {
            ofSetColor(0);
            ofFill();
            ofDrawRectangle(
                VIDEO_WIDTH,
                0,
                VIDEO_WIDTH,
                VIDEO_HEIGHT
            );
            ofNoFill();
            ofSetColor(255,255,0);
        } else {
            ofSetColor(255,0,0);
            ofFill();
            ofDrawRectangle(
                VIDEO_WIDTH,
                0,
                VIDEO_WIDTH,
                VIDEO_HEIGHT
            );
            ofNoFill();

            ofSetColor(255,0,0);
        }

        ofDrawRectangle( currentRectangle.rect );

        font.drawString(
            ofToString(currentRectangle.frames),
            currentRectangle.rect.getRight() - 10,
            currentRectangle.rect.getBottom() + 15);
        font.drawString(
            "Age: " + ofToString(currentRectangle.age),
            currentRectangle.rect.getLeft(),
            currentRectangle.rect.getTop() + 15);
    }
}
void ofApp::drawEyes() {
    ofSetColor(255);
    ofImage frame;
    frame.setFromPixels( player.getPixels() );
    float heightToMantainRatio =
        VIDEO_WIDTH * currentRectangle.rect.getHeight() / currentRectangle.rect.getWidth();
    frame.drawSubsection(
            VIDEO_WIDTH,
            VIDEO_HEIGHT - heightToMantainRatio * 2,
            VIDEO_WIDTH,
            heightToMantainRatio,
            currentRectangle.rect.getTopLeft().x,
            currentRectangle.rect.getTopLeft().y,
            currentRectangle.rect.getWidth(),
            currentRectangle.rect.getHeight()
    );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 'd' ) {
//        ofLog(OF_LOG_WARNING, ofToString( player.getPosition() ));
        ofLog(OF_LOG_NOTICE, "Position: " + ofToString(player.getPosition()));
        player.setPosition( player.getPosition() + 0.01 );
    }
    if ( key == 'a' ) {
        ofLog(OF_LOG_NOTICE, "Position: " + ofToString(player.getPosition()));
        player.setPosition( player.getPosition() - 0.01 );
    }
    if ( key == 's' ) {
        ofLog(OF_LOG_NOTICE, "Saving...");

        ofImage frame;
        frame.setFromPixels( player.getPixels() );
        frame.crop(
            currentRectangle.rect.getTopLeft().x,
            currentRectangle.rect.getTopLeft().y,
            currentRectangle.rect.getWidth(),
            currentRectangle.rect.getHeight()
        );
        frame.save(arguments.at(1) + "_" + ofToString(player.getCurrentFrame()) + ".jpg");
        ofLog(OF_LOG_NOTICE, "Saved");

    }
    if ( key == 'q' ) {
        ofExit(0);
    }
    if ( key == 't' ) {
        ofLog(OF_LOG_NOTICE, "Position: " + ofToString(player.getPosition()));
    }
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
