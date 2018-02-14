#include "ofxLSLine.h"

ofxLSLine::ofxLSLine(){
    
}

void ofxLSLine::generate(ofMesh& mesh, ofxLSBranch branch){
    auto start = branch.begin.getGlobalPosition();
    auto end = branch.end.getGlobalPosition();
    auto numberCurrentVertices = mesh.getNumVertices();
    
    mesh.addVertex(start);
    mesh.addNormal(glm::normalize(start));
    
    mesh.addVertex(end);
    mesh.addNormal(glm::normalize(end));
    
    mesh.addIndex(numberCurrentVertices);
    mesh.addIndex(numberCurrentVertices+1);
}
