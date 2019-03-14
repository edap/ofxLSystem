#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    readFilesDirectory();
    selectedTheta.addListener(this, &ofApp::selectedThetaChanghed);
    selectedDepth.addListener(this, &ofApp::selectedDepthChanghed);
    saveMesh.addListener(this, &ofApp::saveMeshPressed);
    // on writing xml files, take care of this
    // https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
    gui.loadFromFile("trees/bush-1.xml");
    gui.setup();
    gui.setPosition(ofPoint(5, 50));
    gui.setName("treed");
    gui.add(selectedRule.set("rule","F->F[-F][+F]"));
    axiom.setName("axiom");
    gui.add(axiom);
    gui.add(randomYRotation.set("Random Y Rotation", false));
    gui.add(definedConstant.set("constants", ""));
    gui.add(selectedDepth.set("depth", 2, 1, 9));
    gui.add(selectedTheta.set("theta", 25.00, 0.00, 45.00));
    gui.add(bgColor.set("bg color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
    gui.add(treeColor.set("tree color", ofColor(255, 0, 0), ofColor(0, 0), ofColor(255, 255)));
    gui.add(specularColor.set("specular color", ofColor(255, 255, 0), ofColor(0, 0), ofColor(255, 255)));
    gui.add(saveMesh.setup("save mesh"));
    ofSetVerticalSync(true);
    oldDepth = selectedDepth;

    //light
    light.setPosition(500, 1000, 1600);
    light.enable();

    //camera
    cam.setPosition(0, 100, 200);
    cam.lookAt(ofVec3f(0, -20, 0));

    startLSystem(axiom,
                 selectedRule,
                 selectedTheta,
                 selectedDepth,
                 definedConstant,
                 randomYRotation
                 );
}

//--------------------------------------------------------------
void ofApp::update(){
    material.setDiffuseColor(ofColor(treeColor));
    material.setSpecularColor(ofColor(specularColor));
    lsystem.pan(0.4);
}

void ofApp::startLSystem(string axiom,
                         string rulesString,
                         float theta,
                         int depth,
                         string _constants,
                         bool randomZRotation){
    auto constants = getConstants(_constants);
    auto rules = getRules(rulesString);
    lsystem.setAxiom(axiom);
    lsystem.setRules(rules);
    lsystem.setTheta(theta);
    lsystem.setStep(depth);
    lsystem.setScaleWidth(true);
    lsystem.setGeometry(TRIANGLES);
    lsystem.setConstants(constants);
    lsystem.setRandomYRotation(randomYRotation);
    lsystem.build();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(bgColor);

    maybeDrawGui();
    ofEnableDepthTest();
    // scene
    cam.begin();
    //ofDrawAxis(100);
    material.begin();
    lsystem.draw();
    //lsystem.drawWireframe();
    //lsystem.drawNormals(20);
    material.end();
    cam.end();
    ofDisableDepthTest();
}

void ofApp::readFilesDirectory(){
    selected = 0;
    dir.listDir("trees");
    dir.sort();
    for(auto file:dir){
        fileNames.push_back(file.path());
    }
}

void ofApp::loadSettings(string filename){
    gui.clear();
    gui.setName("treed");
    readFilesDirectory();
}

void ofApp::selectedThetaChanghed(float & aselectedTheta){
    startLSystem(axiom,
                 selectedRule,
                 selectedTheta,
                 selectedDepth,
                 definedConstant,
                 randomYRotation);
}

void ofApp::selectedDepthChanghed(int & aselectedDepth){
    if(oldDepth!= aselectedDepth){
        oldDepth = aselectedDepth;
        startLSystem(axiom,
                     selectedRule,
                     selectedTheta,
                     selectedDepth,
                     definedConstant,
                     randomYRotation);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key)
    {
        case OF_KEY_RETURN:
            gui.loadFromFile(fileNames[selected]);
            break;

        case OF_KEY_UP:
            selected = (selected - 1);
            if (selected < 0) selected = fileNames.size() - 1;
            break;

        case OF_KEY_DOWN:
            selected = (selected + 1) % fileNames.size();
            break;
        case 's':
            gui.saveToFile(fileNames[selected]);
            break;

        case 'g':
            drawGui = !drawGui;
            break;

        default:
            break;
    }
}
//it expects a single string like "y=0.54353;t=1"
map<string,float>ofApp::getConstants(string _constant) const{
    map<string,float> result;
    auto parts = ofSplitString(_constant, ";");
    for (auto con : parts){
        auto def = ofSplitString(con, "=");
        if(def.size() == 2){
            result.insert(make_pair(def[0], ofToFloat(def[1])));
        }
    }
    return result;
}

vector<string>ofApp::getRules(string rules) const{
    vector<string> stringRules;
    if(rules.find(";") != std::string::npos){
        //multiple rules
        auto rule_container = ofSplitString(rules, ";");
        for(auto r:rule_container){
            stringRules.push_back(r);
        }
    }else{
        //single rule
        stringRules.push_back(rules);
    }
    return stringRules;
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

void ofApp::maybeDrawGui(){
    if (drawGui) {
        //fps
        string msg = "\n\nfps: " + ofToString(ofGetFrameRate(), 2);
        ofDrawBitmapStringHighlight(msg, 10, 10);
        //gui
        gui.draw();
        // file selection
        ofPushStyle();
        ofDisableLighting();
        for (unsigned i = 0; i < fileNames.size(); ++i){
            if (i == selected) ofSetColor(0, 255, 0);
            else ofSetColor(255, 0, 0);
            ofDrawBitmapString(fileNames[i], 10, 500 +(20 * (i + 1)));
        }
        ofEnableLighting();
        ofPopStyle();
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

void ofApp::saveMeshPressed(){
    string randomFilename = ofGetTimestampString() + ".ply";
    lsystem.save(randomFilename);
}
