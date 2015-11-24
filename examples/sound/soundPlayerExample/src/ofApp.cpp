#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	// on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
	// use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
	
	synth.load("sounds/synth.wav");
	beats.load("sounds/1085.mp3");
	vocals.load("sounds/Violet.mp3");
	synth.setVolume(0.75f);
	beats.setVolume(0.75f);
	vocals.setVolume(0.5f);
	font.load("Sudbury_Basin_3D.ttf", 32);
	beats.setMultiPlay(false);
	vocals.setMultiPlay(true);
	
	sampleRate = 44100;
	wavePhase = 0;
	pulsePhase = 0;
	
	// start the sound stream with a sample rate of 44100 Hz, and a buffer
	// size of 512 samples per audioOut() call
	ofSoundStreamSetup(2, 0, sampleRate, 512, 3);
}

//--------------------------------------------------------------
void ofApp::update(){

	ofBackground(255,255,255);

	// update the sound playing system:
	ofSoundUpdate();
	
	unique_lock<mutex> lock(audioMutex);
	
	// this loop is building up a polyline representing the audio contained in
	// the left channel of the buffer
	
	// the x coordinates are evenly spaced on a grid from 0 to the window's width
	// the y coordinates map each audio sample's range (-1 to 1) to the height
	// of the window
	
	waveform.clear();
	for(size_t i = 0; i < lastBuffer.getNumFrames(); i++) {
		float sample = lastBuffer.getSample(i, 0);
		float x = ofMap(i, 0, lastBuffer.getNumFrames(), 0, ofGetWidth());
		float y = ofMap(sample, -1, 1, 0, ofGetHeight());
		waveform.addVertex(x, y);
	}
	
	rms = lastBuffer.getRMSAmplitude();


}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
	
	// base frequency of the lowest sine wave in cycles per second (hertz)
	float frequency = 172.5;
	
	// mapping frequencies from Hz into full oscillations of sin() (two pi)
	float wavePhaseStep = (frequency / sampleRate) * TWO_PI;
	float pulsePhaseStep = (0.5 / sampleRate) * TWO_PI;
	
	// this loop builds a buffer of audio containing 3 sine waves at different
	// frequencies, and pulses the volume of each sine wave individually. In
	// other words, 3 oscillators and 3 LFOs.
	
	for(size_t i = 0; i < outBuffer.getNumFrames(); i++) {
		
		// build up a chord out of sine waves at 3 different frequencies
		float sampleLow = sin(wavePhase);
		float sampleMid = sin(wavePhase * 1.5);
		float sampleHi = sin(wavePhase * 2.0);
		
		// pulse each sample's volume
		sampleLow *= sin(pulsePhase);
		sampleMid *= sin(pulsePhase * 1.04);
		sampleHi *= sin(pulsePhase * 1.09);
		
		float fullSample = (sampleLow + sampleMid + sampleHi);
		
		// reduce the full sample's volume so it doesn't exceed 1
		fullSample *= 0.3;
		
		// write the computed sample to the left and right channels
		outBuffer.getSample(i, 0) = fullSample;
		outBuffer.getSample(i, 1) = fullSample;
		
		// get the two phase variables ready for the next sample
		wavePhase += wavePhaseStep;
		pulsePhase += pulsePhaseStep;
	}
	
	unique_lock<mutex> lock(audioMutex);
	lastBuffer = outBuffer;
}


//--------------------------------------------------------------
void ofApp::draw(){

	// draw the background colors:
	float widthDiv = ofGetWidth() / 3.0f;
	ofSetHexColor(0xeeeeee);
	ofDrawRectangle(0,0,widthDiv,ofGetHeight());
	ofSetHexColor(0xffffff);
	ofDrawRectangle(widthDiv,0,widthDiv,ofGetHeight());
	ofSetHexColor(0xdddddd);
	ofDrawRectangle(widthDiv*2,0,widthDiv,ofGetHeight());


	//---------------------------------- synth:
	if (synth.isPlaying()) ofSetHexColor(0xFF0000);
	else ofSetHexColor(0x000000);
	font.drawString("synth !!", 50,50);

	ofSetHexColor(0x000000);
	string tempStr = "click to play\npct done: "+ofToString(synth.getPosition())+"\nspeed: " + ofToString(synth.getSpeed()) + "\npan: " + ofToString(synth.getPan()) ;
	ofDrawBitmapString(tempStr, 50,ofGetHeight()-50);



	//---------------------------------- beats:
	if (beats.isPlaying()) ofSetHexColor(0xFF0000);
	else ofSetHexColor(0x000000);
	font.drawString("beats !!", widthDiv+50,50);

	ofSetHexColor(0x000000);
	tempStr = "click and drag\npct done: "+ofToString(beats.getPosition())+"\nspeed: " +ofToString(beats.getSpeed());
	ofDrawBitmapString(tempStr, widthDiv+50,ofGetHeight()-50);

	//---------------------------------- vocals:
	if (vocals.isPlaying()) ofSetHexColor(0xFF0000);
	else ofSetHexColor(0x000000);
	font.drawString("vocals !!", widthDiv*2+50,50);

	ofSetHexColor(0x000000);
	tempStr = "click to play (multiplay)\npct done: "+ofToString(vocals.getPosition())+"\nspeed: " + ofToString(vocals.getSpeed());	
	ofDrawBitmapString(tempStr, widthDiv*2+50,ofGetHeight()-50);
	
	ofSetColor(ofColor::white);
	ofSetLineWidth(1 + (rms * 30.));
	waveform.draw();


}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	// continuously control the speed of the beat sample via drag,
	// when in the "beat" region:
	float widthStep = ofGetWidth() / 3.0f;
	if (x >= widthStep && x < widthStep*2){
		beats.setSpeed( 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*1.0f);
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	float widthStep = ofGetWidth() / 3.0f;
	if (x < widthStep){
		synth.play();
		synth.setSpeed( 0.1f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*10);
		synth.setPan(ofMap(x, 0, widthStep, -1, 1, true));
	} else if (x >= widthStep && x < widthStep*2){
		beats.play();
	} else {
		vocals.play();
		vocals.setSpeed( 0.1f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*3);
		//map x within the last third of window to -1 to 1 ( for left / right panning )
		vocals.setPan( ofMap(x, widthStep*2, widthStep*3, -1, 1, true) );
	}
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
