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

    int processStatus = currentRectangle.process( cvFinder.blobs );
    switch (processStatus) {
        // case CurrentRectangle.STATUS_NO_EYES:
        // case CurrentRectangle.STATUS_WITH_EYES:
        case CurrentRectangle::STATUS_NEW_EYES:
            ofLog() << "Eyes detected";
            break;
        case CurrentRectangle::STATUS_AGE_UP:
            ofLog() << "Age is " << currentRectangle.age;
            break;
        case CurrentRectangle::STATUS_AGE_OF_CONSENT_REACHED:
            ofLog() << "Age of consent reached";
            break;
        case CurrentRectangle::STATUS_EYES_LOST:
            ofLog() << "Eyes lost";
            checkLastRecord();
            break;
    }

    if ( player.isFrameNew() ) {
        checkNoEyes();
        recordEyes();
    }

    if ( player.getIsMovieDone() ) {
        ofLog() << "Finished";
        ofLog() << "********\n\n";
        ofLog() << "recorded chunks: " << recordedChunks;
        ofLog() << "discarded chunks: " << discardedChunks;
        ofLog() << "seconds elapsed: " << ofGetElapsedTimef();
        ofLog() << "video length: " << player.getDuration();
        ofExit(0);
    }
}

void ofApp::checkNoEyes(){
    if ( cvFinder.blobs.size() == 0 ) {
        framesWithoutEyes++;
    } else {
        if ( framesWithoutEyes != 0 && currentSpeed != VIDEO_MATCH_SPEED ) {
            ofLog() << "Video speed: Match (x" << VIDEO_MATCH_SPEED << ")";
            currentSpeed = VIDEO_MATCH_SPEED;
            player.setSpeed(currentSpeed);
        }
        framesWithoutEyes = 0;
    }

    if ( framesWithoutEyes == 100 ) {
        ofLog() << "Video speed: Skip (x" << VIDEO_SKIP_SPEED << ")";
        currentSpeed = VIDEO_SKIP_SPEED;
        player.setSpeed(currentSpeed);
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);

    ofNoFill();

    if ( ANALYZE_MODE ) {
        drawVideo();
        drawRaw();
        drawCurrentRectangle();
        drawCentroPromedio();
        drawEyes();
    } else {
        drawProgress();
    }
}

void ofApp::drawProgress() {
    ofSetColor(255);
    font.drawString(
        "Time: " + ofToString(floor(player.getPosition() * player.getDuration())) + "/" + ofToString(player.getDuration()),
        5,
        20);

    font.drawString(
        "Chunks recorded: " + ofToString(recordedChunks),
        5,
        40);
    font.drawString(
        "Chunks discarded: " + ofToString(discardedChunks),
        5,
        60);

    if ( currentSpeed == VIDEO_SKIP_SPEED ) {
        ofSetColor(0,255,0);
        font.drawString(
            "SKIP SPEED ON (x" + ofToString(VIDEO_SKIP_SPEED) + ")",
            5,
            80);
        ofSetColor(255);
    } else {
        font.drawString(
            "NORMAL SPEED (x" + ofToString(VIDEO_MATCH_SPEED) + ")",
            5,
            80);
    }

    if ( !currentRectangle.isEmpty() ) {
        if ( currentRectangle.age >= currentRectangle.AGE_OF_CONSENT ) {
            ofSetColor(255,0,0);
        } else {
            ofSetColor(255,255,0);
        }
        font.drawString(
            "RECORDING (age: " + ofToString(currentRectangle.age) + ")" ,
            5,
            100);
        ofSetColor(255);
    }
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

void ofApp::recordCurrentFrame() {
    recordCurrentFrame( player.getCurrentFrame() );
}
void ofApp::recordCurrentFrame(int currentFrame) {
    ofImage frame;
    frame.setFromPixels( player.getPixels() );
    frame.crop(
        currentRectangle.rect.getTopLeft().x,
        currentRectangle.rect.getTopLeft().y,
        currentRectangle.rect.getWidth(),
        currentRectangle.rect.getHeight()
    );
    frame.resize(RECORD_FORCE_WIDTH, RECORD_FORCE_WIDTH / currentRectangle.RECTANGLE_FORCE_ASPECT_RATIO);
    frame.save( recordConstructPath(currentFrame) );
}
string ofApp::recordConstructPath(int currentFrame) {
    string savePath = OUTPUT_DIRECTORY + arguments.at(1) + "_";
    string strCurrentFrame = ofToString(currentFrame);
    strCurrentFrame.insert( strCurrentFrame.begin(), 10 - strCurrentFrame.length(), '0' );
    savePath += strCurrentFrame;
    savePath += ".jpg";

    return savePath;
}

void ofApp::recordEyes() {
    if ( enableRecordEyes ) {
        if ( !currentRectangle.isEmpty() ) {
            // int currentFrame = player.getCurrentFrame();
            // if ( currentFrame > -1 ) {
                // if ( std::find(recordedFrames.begin(), recordedFrames.end(), currentFrame) == recordedFrames.end() ) { //si no está
                    recordedFramesAll++;
                    recordCurrentFrame(recordedFramesAll);
                    recordedFrames.push_back(recordedFramesAll);

                // }
            // }
        }
    }
}

void ofApp::checkLastRecord() {
    string dataPath = "bin/data/";

    if ( currentRectangle.previousRectangleAge < currentRectangle.AGE_OF_CONSENT ) {
        discardedChunks++;
        ofLog() << "Age of consent not reached, deleting recorded frames...";
        for ( int i = 0 ; i < recordedFrames.size() ; i++ ) {
            ofSystem("rm " + dataPath + recordConstructPath(recordedFrames[i]) );
        }
    } else {
        recordedChunks++;
        int framesToRemove = currentRectangle.getPreviousRectangleLastYearNumFrames();
        framesToRemove = floor( framesToRemove * 0.8 );
        ofLog() << "Chunk recorded, removing last " << framesToRemove << " frames";

        for ( int i = recordedFrames.size() - framesToRemove ; i < recordedFrames.size() ; i++ ) {
            ofSystem("rm " + dataPath + recordConstructPath(recordedFrames[i]) );
        }

    }

    recordedFrames.clear();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 'd' ) {
        ofLog(OF_LOG_NOTICE, "Position: " + ofToString(player.getPosition()));
        player.setPosition( player.getPosition() + 0.01 );
    }
    if ( key == 'a' ) {
        ofLog(OF_LOG_NOTICE, "Position: " + ofToString(player.getPosition()));
        player.setPosition( player.getPosition() - 0.01 );
    }
    if ( key == 's' ) {
        ofLog(OF_LOG_NOTICE, "Saving...");
        recordCurrentFrame();
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
