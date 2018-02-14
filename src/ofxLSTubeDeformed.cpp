#include "ofxLSTubeDeformed.h"

ofxLSTubeDeformed::ofxLSTubeDeformed(){
    ring.clear();
    middleRing.clear();
    randomNumbers.clear();
    buildRing(resolution);
    calcSide();
    generateSetRandomNumbers(resolution);
}

void ofxLSTubeDeformed::generate(ofMesh& mesh, const ofxLSBranch branch, const int radius){
    ofMatrix4x4 beginMatrix = branch.begin.getGlobalTransformMatrix();
    ofMatrix4x4 endMatrix = branch.end.getGlobalTransformMatrix();
    middleRing.clear();
    // first part
    addRoundIndices(resolution,mesh.getNumVertices(), mesh);
    std::random_shuffle (randomNumbers.begin(), randomNumbers.end() );
    unsigned int index_random_vector = 0;
    for(auto circleTemp : ring){
        // bottom
        mesh.addVertex(circleTemp * beginMatrix);
        ofVec3f direction = (ofVec3f() + circleTemp).getNormalized();
        //middle
        ofVec3f medTemp = circleTemp;
        int middle = (branch.end.getPosition().z - branch.begin.getPosition()).z/2;
        medTemp.z = middle+ ofRandom(-middle/2, +middle/2);
        medTemp.x = medTemp.x + randomNumbers.at(index_random_vector);
        medTemp.y = medTemp.y + randomNumbers.at(index_random_vector);
        middleRing.push_back(medTemp);
        mesh.addVertex(medTemp * beginMatrix);
        index_random_vector ++;
    }
    // second part
    addRoundIndices(resolution,mesh.getNumVertices(), mesh);
    int i = 0;
    for(auto circleTemp : ring){
        mesh.addVertex(middleRing.at(i) * beginMatrix);
        //top
        mesh.addVertex(circleTemp * endMatrix);
        i ++;
    }
    calcNormals(mesh);
}

void ofxLSTubeDeformed::buildRing(const int _resolution){
    for (int i = 0; i < _resolution; i++){
        //calculate x and y component
        float theta = 2.0f * 3.1415926f * float(i) / float(_resolution);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        ofVec3f circleTemp = ofVec3f(x, y, 0.0);
        ring.push_back(circleTemp);
    }
}

void ofxLSTubeDeformed::calcSide(){
    side = ring.at(0).distance(ring.at(1));
    variation = side/2;
}

void ofxLSTubeDeformed::addRoundIndices(const int _resolution,const int _totalVertices, ofMesh& mesh){
    for (int i = 0; i < _resolution; i++){
        // if it is the last face, close it where the first face
        // was started
        if(i == _resolution -1){
            mesh.addIndex(_totalVertices+(i*2));
            mesh.addIndex(_totalVertices);
            mesh.addIndex(_totalVertices+1);
            
            mesh.addIndex(_totalVertices+1);
            mesh.addIndex(_totalVertices+(i*2)+1);
            mesh.addIndex(_totalVertices+(i*2));
        }else{
            mesh.addIndex(_totalVertices+(i*2));
            mesh.addIndex(_totalVertices+(i*2)+2);
            mesh.addIndex(_totalVertices+(i*2)+3);
            
            mesh.addIndex(_totalVertices+(i*2)+3);
            mesh.addIndex(_totalVertices+(i*2)+1);
            mesh.addIndex(_totalVertices+(i*2));
        }
    }
}

void ofxLSTubeDeformed::calcNormals(ofMesh& mesh){
    mesh.clearNormals();
    // https://forum.openframeworks.cc/t/mesh-lighting-normal-issues/18127/11
    // via https://github.com/ofZach/ofxMeshUtils/blob/master/src/ofxMeshUtils.cpp
    // inspired by http://www.iquilezles.org/www/articles/normals/normals.htm
    for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(ofPoint(0,0,0));

    //itero un triangolo alla volta
    for( int i=0; i < mesh.getIndices().size(); i+=3 ){
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];

        //calcolo la posizione e la direzione di due vertici di un triangolo
        ofVec3f e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
        ofVec3f e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
        //uso i due vertici per trovare il normal
        ofVec3f no = e2.cross( e1 );

        // depending on your clockwise / winding order, you might want to reverse the e2 / e1 above if your normals are flipped.

        mesh.getNormals()[ia] += no;
        mesh.getNormals()[ib] += no;
        mesh.getNormals()[ic] += no;
    }
    //normalizzo tutti i vettori che definiscono i normals
    for(int i=0; i < mesh.getNormals().size(); i++ ) {
        mesh.getNormals()[i] = glm::normalize(mesh.getNormals()[i]);
    }
}

void ofxLSTubeDeformed::generateSetRandomNumbers(const unsigned int resolution){
    for(unsigned int i = 0; i < resolution; i++){
        randomNumbers.push_back(ofRandom(-variation, +variation));
    }
}
