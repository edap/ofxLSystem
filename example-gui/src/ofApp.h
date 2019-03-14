#pragma once

#include "ofMain.h"
#include "ofxLSystem.h"
#include "ofParameter.h"
#include "ofxGui.h"

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
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void saveMeshPressed();
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void selectedThetaChanghed(float & selectedTheta);
    void selectedDepthChanghed(int & selectedDepth);
    void fileSaved(int & file);

private:
    void loadSettings(string filename = "bush-1.xml");
    void readFilesDirectory();
    map<string,float> getConstants(string _constants) const;
    vector<string> getRules(string rules) const;
    void startLSystem(
                      string axiom,
                      string selectedRule,
                      float theta,
                      int depth,
                      string constants,
                      bool randomZRotation);
    void maybeDrawGui();
    string trim(string str);
    bool drawGui = true;
    int selected;

    ofxPanel gui;
    ofParameter<bool> vSync;

    //LSystem parameters
    ofParameter<string> selectedRule;
    ofParameter<string> definedConstant;
    ofParameter<string> axiom;
    ofParameter<int>    selectedDepth;
    ofParameter<bool>   randomYRotation;
    unsigned int oldDepth = 1;
    ofParameter<float>  selectedTheta;
    ofParameter<ofColor> bgColor;
    ofParameter<ofColor> treeColor;
    ofParameter<ofColor> specularColor;
    ofxButton saveMesh;

    vector<string> fileNames;
    ofDirectory dir;

    ofEasyCam        cam;
    ofLight          light;
    ofxLSystem       lsystem;
    ofMaterial       material;
    ofTrueTypeFont   font;
};
