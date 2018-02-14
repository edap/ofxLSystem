#include "ofApp.h"
const bool debugAudio = false;

//--------------------------------------------------------------
void ofApp::setup(){
    saveMesh.addListener(this, &ofApp::saveMeshPressed);
    ofDisableAlphaBlending();
    ofEnableDepthTest();
    float halfScreen = ofGetWidth()/2;

    light.setup();
    light.enable();
    light.setPosition(ofVec3f(400,300,200));
    light.lookAt(ofVec3f(0,0,0));

    map<string, float> constants;
    constants.insert(make_pair("R", 1.356));

    for(int i = 0; i<nTree; i++){
        ofxLSystem tree;
        tree.setAxiom("A(100)");
        tree.setRules({"A(s) -> F(s)[+A(s/R)][-A(s/R)]"});
        tree.setRandomYRotation(true);
        tree.setConstants(constants);
        tree.setStep(abs(ofRandom(5, 10)));
        tree.setScaleWidth(true);
        tree.setStepWidth(abs(ofRandom(30.5, 40.5)));
        tree.setStepLength(abs(ofRandom(150,300)));
        tree.build();
        tree.pan(ofRandom(30.00, 330.00));
        auto pos = ofVec2f(ofRandom(-halfScreen, halfScreen),
                ofRandom(-halfScreen, halfScreen));

        tree.setPosition(ofVec3f(pos.x, 0, pos.y));
        for (int i=0; i<tree.getMesh().getNumVertices(); i++) {
            ofVec3f actualVert =tree.getMesh().getVerticesPointer()[i];
            ofVec3f newVert = actualVert * tree.getGlobalTransformMatrix();
            tree.getMesh().getVerticesPointer()[i] = newVert;
        }
        forest.append(tree.getMesh());
    }
    uResolution = ofVec2f(ofGetWidth(), ofGetHeight());

    if(ofIsGLProgrammableRenderer()){
        shader.load("shaders_gl3/ring.vert", "shaders_gl3/ring.frag");
    }

    gui.setup();
    gui.add(scale.set("scale", 1.0, 0.02, 5.0));
    gui.add(uNRing.set("uNRing", 1.0, 1.0, 15.0));
    gui.add(thickness.set("thickness", 0.001, 0.001, 0.1));
    gui.add(useLight.set("use light", false));
    gui.add(lightPos.set("lightPosition",
                         ofVec3f(400,300,200), ofVec3f(-800,-800,-800), ofVec3f(800,800,800)));
    gui.add(materialColor.setup("material&bg color",
                                ofColor(0), ofColor(0), ofColor(255)));

    gui.add(ringColor.setup("ring color",
                                ofColor(255), ofColor(0), ofColor(255)));
    gui.add(enableSmooth.set("enable smothed volume", false));
    gui.add(minThreshold.set("minTreshold", 0.1, 0.1, 0.3));
    gui.add(smoothedVal.set("smoothed value", 0.97, 0.70, 0.99));
    gui.add(saveMesh.setup("save meshes"));
    //ofSetVerticalSync(true);
    auto boundingBox = getBoundingBox();
    uMinTree = boundingBox.min;
    uMaxTree = boundingBox.max;

    float treeWidth = abs(uMinTree.x) + uMaxTree.x;
    float treeHeight = abs(uMinTree.y) + uMaxTree.y;
    uTreeResolution = ofVec2f(treeWidth, treeHeight);

    //audio
    sample.load(ofToDataPath("beat.wav"));
    sampleRate 	= 44100;
    bufferSize	= 512;
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(materialColor);
    cam.begin();

    shader.begin();
    shader.setUniform1f("uTime", ofGetElapsedTimef());
    shader.setUniform1f("uThickness", thickness);
    shader.setUniform1f("uScale", scale);
    shader.setUniform1f("uThreshold", threshold);
    shader.setUniform1f("uNRing", uNRing);
    shader.setUniform3f("uLightPosition", lightPos.get());
    shader.setUniform4f("uMaterialColor", ofColor(materialColor));
    shader.setUniform4f("uRingColor", ofColor(ringColor));
    shader.setUniform4f("uRingColor", ofColor(ringColor));
    shader.setUniform2f("uResolution", uResolution );
    shader.setUniform2f("uTreeResolution", uTreeResolution );
    shader.setUniform2f("uMinTree", uMinTree);
    shader.setUniform2f("uMaxTree", uMaxTree);
    shader.setUniform1i("uUseLight", useLight);

    forest.draw();

    shader.end();
    cam.end();
    maybeDrawGui();
}

void ofApp::update(){

}


void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    float rms = 0.0;
    int numCounted = 0;
    for (int i = 0; i < bufferSize; i++){
        wave = sample.play();
        mymix.stereo(wave, outputs, 0.5);
        output[i*nChannels    ] = outputs[0];
        output[i*nChannels + 1] = outputs[1];

        //rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html
        float left = outputs [0];
        float right = outputs[1];
        rms += left * left;
        rms += right * right;
        numCounted += 2;
    }

    rms /= (float)numCounted;
    rms = sqrt(rms);

    if (enableSmooth) {
        smoothedVolume *= smoothedVal;
        smoothedVolume += 0.07 * rms;
    } else {
        smoothedVolume = rms;
    }

    //onset detection as in http://openframeworks.cc/ofBook/chapters/sound.html
    threshold = ofLerp(threshold, minThreshold, decayRate);
    if(rms > threshold) {
        threshold = rms;
    }
    
}

void ofApp::maybeDrawGui(){
    if(drawGui){
        ofDisableDepthTest();
        gui.draw();
        ofEnableDepthTest();
    }
}

ForestBoundingBox ofApp::getBoundingBox(){
    ForestBoundingBox boundingBox;
    for(auto v : forest.getVertices()){
        if (v.x < boundingBox.min.x) boundingBox.min.x = v.x;
        if (v.y < boundingBox.min.y) boundingBox.min.y = v.y;
        if (v.z < boundingBox.min.z) boundingBox.min.z = v.z;

        if (v.x > boundingBox.max.x) boundingBox.max.x = v.x;
        if (v.y > boundingBox.max.y) boundingBox.max.y = v.y;
        if (v.z > boundingBox.max.z) boundingBox.max.z = v.z;
    }
    return boundingBox;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            drawGui = !drawGui;
            break;

        default:
            break;
    }
}



void ofApp::saveMeshPressed(){
    string randomFilename = ofGetTimestampString() + ".ply";
    forest.save(randomFilename);
}
