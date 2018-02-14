#pragma once
#include "ofMain.h"
#include "ofxLSystem.h"
#include "ofxGui.h"
#include "ofxMaxim.h"

typedef struct{
    ofVec3f min = ofVec3f(0,0,0);
    ofVec3f max = ofVec3f(0,0,0);
}ForestBoundingBox;

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    void maybeDrawGui();
    // Audio output and input methods
    void audioOut(float * output, int bufferSize, int nChannels);
    void saveMeshPressed();
    ForestBoundingBox getBoundingBox();

    vector<ofxLSystem> treeContainer;
    ofMesh forest;
    ofxPanel   gui;
    ofParameter<ofVec3f> lightPos;
    ofxColorSlider materialColor;
    ofxColorSlider ringColor;
    ofParameter<float> scale;
    ofParameter<float> uNRing;
    ofParameter<float> thickness;
    ofParameter<bool> enableSmooth = false;
    ofParameter<bool> useLight = false;
    ofParameter<float> smoothedVal;
    ofParameter<float> minThreshold;
    ofxButton saveMesh;
    bool       drawGui = true;
    ofLight    light;
    ofEasyCam  cam;
    ofShader   shader;
    ofVec2f    uResolution;
    ofVec2f    uTreeResolution;
    int        nTree = 60;

    ofVec2f    uMinTree;
    ofVec2f    uMaxTree;
    float      maxHeight;

    //maximilian
    int		bufferSize;
    int		sampleRate;
    ofxMaxiSample sample;
    maxiMix mymix;
    double wave;
    double outputs[2];
    float smoothedVolume = 0;
    float scaledVolume = 0;
    float decayRate = 0.05;
    float threshold;
};
