#include "ofxLSTube.h"

ofxLSTube::ofxLSTube(){
    
}

void ofxLSTube::setResolution(const int _res){
    if(_res < 3){
        resolution = 3;
    } else {
        resolution = _res;
    }
}

void ofxLSTube::generate(ofMesh& mesh, const ofxLSBranch branch, const float length){
    bool cap = true;
    const int radius = branch.capSizes.first;
    const int scaledRadius = branch.capSizes.second;

    // these variables are used to do not stretch the texture
    float circumferenceBottom = radius * 3.1415926f;
    float ratio = circumferenceBottom/length;
    float ratioCap = scaledRadius/length;
    float xWrapLimit = circumferenceBottom/(length/textureRepeat);
    float wrapLimitCap = ratioCap * textureRepeat;

    ofMatrix4x4 beginMatrix = branch.begin.getGlobalTransformMatrix();
    ofMatrix4x4 endMatrix = branch.end.getGlobalTransformMatrix();

    // Cylinder body
    int first = mesh.getNumVertices();
    for (int i = 0; i <= resolution; i++) {
        // if it is the last face, close it where the first face
        // was started
        if (i == resolution) {
            mesh.addIndex(first+(i*2));
            mesh.addIndex(first);
            mesh.addIndex(first+1);

            mesh.addIndex(first+1);
            mesh.addIndex(first+(i*2)+1);
            mesh.addIndex(first+(i*2));
        } else {
            mesh.addIndex(first+(i*2));
            mesh.addIndex(first+(i*2)+2);
            mesh.addIndex(first+(i*2)+3);

            mesh.addIndex(first+(i*2)+3);
            mesh.addIndex(first+(i*2)+1);
            mesh.addIndex(first+(i*2));
        }
    }

    for (int i = 0; i <= resolution; i++) {
        //calculate x and y component
        float theta = 2.0f * 3.1415926f * float(i) / float(resolution);
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        ofVec3f circleBottom = ofVec3f(x, 0.0, z);
        ofVec3f direction = (ofVec3f() + circleBottom).getNormalized();

        float xTop = scaledRadius * cosf(theta);
        float zTop = scaledRadius * sinf(theta);
        ofVec3f circleTop = ofVec3f(xTop, 0.0, zTop);
        ofVec2f tcoord;
        tcoord.x = ofMap(i, 0.f, resolution, 0.f, xWrapLimit);

        // bottom
        tcoord.y = 0;
        mesh.addVertex(circleBottom * beginMatrix);
        mesh.addNormal(direction * beginMatrix.getRotate());
        mesh.addTexCoord(tcoord);

        //top
        tcoord.y = textureRepeat;
        mesh.addVertex(circleTop * endMatrix);
        mesh.addNormal(direction * endMatrix.getRotate());
        mesh.addTexCoord(tcoord);
    }

    // Cylinder cap
    if (cap) {
        int topMiddlePoint = mesh.getNumVertices();
        ofVec3f topDir = glm::normalize(branch.end.getPosition());
        mesh.addVertex(branch.end.getGlobalPosition());
        mesh.addNormal(topDir * endMatrix.getRotate());
        mesh.addTexCoord(ofVec2f(wrapLimitCap/2,wrapLimitCap/2));

        for (int i = 0; i <= resolution; i++) {
            if (i == resolution) {
                //closing triangle
                mesh.addIndex(topMiddlePoint);
                mesh.addIndex(topMiddlePoint+ i + 1);
                mesh.addIndex(topMiddlePoint+1);
            } else {
                //indices
                mesh.addIndex(topMiddlePoint);
                mesh.addIndex(topMiddlePoint+ i + 1);
                mesh.addIndex(topMiddlePoint+ i + 2);
            }
            //add vertex
            float theta = 2.0f * 3.1415926f * float(i) / float(resolution);
            float x = scaledRadius * cosf(theta);
            float z = scaledRadius * sinf(theta);
            ofVec3f circleTemp = ofVec3f(x, 0.0, z);

            ofVec2f capTcoord;
            capTcoord.x = ofMap(x, -scaledRadius, scaledRadius, 0.f, wrapLimitCap);
            capTcoord.y = ofMap(z, -scaledRadius, scaledRadius, 0.f, wrapLimitCap);
            mesh.addVertex(circleTemp * branch.end.getGlobalTransformMatrix());
            mesh.addNormal(topDir * endMatrix.getRotate());
            mesh.addTexCoord(capTcoord);
        }
    }
}
