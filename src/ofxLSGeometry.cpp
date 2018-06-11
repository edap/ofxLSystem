#include "ofxLSGeometry.h"

void ofxLSGeometry::putIntoMesh(const ofxLSBranch branch, ofVboMesh& mesh,ofxLSGeometryAvailable _geometryType, int resolution, float length, int textureRepeat){
    //Branch deve contenere solo 2 ofNode, uno per l'inizio del segmento e
    // uno per la fine. Tutto il resto viene implementato dalla geometria
    switch (_geometryType) {
        case TUBES:
            tube.setTextureRepeat(textureRepeat);
            tube.setResolution(resolution);
            tube.generate(mesh, branch, length);
            break;
//        case TUBES_DEFORMED:
//            tubeDeformed.generate(mesh, branch, width);
//            break;
        case LINES:
            line.generate(mesh, branch);
            break;
        case TRIANGLES:
            triangle.setResolution(resolution);
            triangle.generate(mesh, branch, length);
            break;
        default:
            line.generate(mesh, branch);
            break;
    }
}
