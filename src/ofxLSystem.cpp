#include "ofxLSystem.h"

ofxLSystem::ofxLSystem(){

}

void ofxLSystem::setAxiom(string _axiom){
    axiom = _axiom;
};

void ofxLSystem::setRules(vector<string> _rulesContainer){
    rulesContainer = _rulesContainer;
};

void ofxLSystem::setTheta(float _theta){ theta = _theta;
    theta = _theta;
};

void ofxLSystem::build(){
    //check if axiom, rules and theta are ok,
    // if not, define some default
    try {
        validateInput(axiom, rulesContainer, theta);
    } catch (ofxLSInputError& e) {
        ofLogError(e.what());
        theta = 25.00;
        axiom = "F";
        rulesContainer = {"F -> F[+F][-F]"};
    }
    //clear the mesh
    mesh.clear();

    // setup the turtle, the sentences and the geometry
    setMeshMode(geometry);
    turtle.setup(stepLength, stepWidth, theta, geometry, randomYRotation, scaleWidth, resolution, textureRepeat);
    const vector<string> sentences =
        ofxLSystemGrammar::buildSentences(rulesContainer, depth, axiom, constants);

    // populate the mesh
    turtle.generate(mesh, sentences.back(), depth);
    getMesh().clear();
    getMesh().append(mesh);
    setBoundingBox(turtle.getBuildedBoundingBox());
    getMesh().enableNormals();
}

void ofxLSystem::save(string filename){
    mesh.save(filename);
}

void ofxLSystem::setMeshMode(ofxLSGeometryAvailable _geometry){
    switch (_geometry) {
        case LINES:
            mesh.setMode(OF_PRIMITIVE_LINES);
            getMesh().setMode(OF_PRIMITIVE_LINES);
            break;
        case TUBES:
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            getMesh().setMode(OF_PRIMITIVE_TRIANGLES);
            break;
        case TUBES_DEFORMED:
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            getMesh().setMode(OF_PRIMITIVE_TRIANGLES);
            break;
        case TRIANGLES:
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            getMesh().setMode(OF_PRIMITIVE_TRIANGLES);
            break;
        default:
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            getMesh().setMode(OF_PRIMITIVE_TRIANGLES);
            break;
    }
}

bool ofxLSystem::isAxiomInRules(string _axiom, vector<string> _rulesContainer){
    auto lettersInAxiom = ofxLSUtils::matchesInRegex(_axiom, "[a-zA-Z]");
    for(auto letter : lettersInAxiom){
        for(string rule : _rulesContainer){
            if(ofxLSUtils::countSubstring(rule, letter) > 0){
                return true;
            }
        }
    }

    return false;
}

bool ofxLSystem::thetaValueIsinRange(float theta){
    return (theta >= -360.00 && theta <= 360.00) ? true : false;
}

void ofxLSystem::validateInput(string _axiom, vector<string> _strRules, float theta){
    if(!isAxiomInRules(_axiom, _strRules)){
        throw ofxLSInputError("axiom is not in rules container");
    }
    if(!thetaValueIsinRange(theta)){
        throw ofxLSInputError("theta has to be between -360.00 and 360.00");
    }
}

void ofxLSystem::setScaleWidth(bool _setScaleWidth){
    if(
       ofxLSystemGrammar::detectGrammarType(rulesContainer) != "parametric" &&
       _setScaleWidth == true
    ){
        ofLogError("only parametric grammar supports setScale=true");
        scaleWidth = false;
    }else{
        scaleWidth = _setScaleWidth;
    }
}

void ofxLSystem::computeBoundingBox(){
    for(auto v : mesh.getVertices()){
        if (v.x < boundingBox.min.x) boundingBox.min.x = v.x;
        if (v.y < boundingBox.min.y) boundingBox.min.y = v.y;
        if (v.z < boundingBox.min.z) boundingBox.min.z = v.z;

        if (v.x > boundingBox.max.x) boundingBox.max.x = v.x;
        if (v.y > boundingBox.max.y) boundingBox.max.y = v.y;
        if (v.z > boundingBox.max.z) boundingBox.max.z = v.z;
    }
}

BoundingBox ofxLSystem::getBoundingBox() const {
    return boundingBox;
};

void ofxLSystem::setBoundingBox(BoundingBox _boundingBox) {
    boundingBox = _boundingBox;
};
