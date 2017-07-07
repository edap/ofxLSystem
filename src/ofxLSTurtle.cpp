#include "ofxLSTurtle.h"

void ofxLSTurtle::setup( float _moveLength, float _width, float _turnAngle, ofxLSGeometryAvailable _geometry, bool _randomYRotation, bool _scaleWidth, int _resolution, int _textureRepeat) {
    defaultLength = _moveLength;
    width = _width;
    theta = _turnAngle;
    resolution = _resolution;
    textureRepeat = _textureRepeat;
    geometry = _geometry;
    randomYRotation = _randomYRotation;
    scaleWidth = _scaleWidth;
    bookmarks.clear();
    branchContainer.clear();
    historySizes.clear();
    shared_ptr<ofNode> root(new ofNode);
    root->setPosition(origin);
    branchContainer.push_back(root);
}

void ofxLSTurtle::generate(ofVboMesh& mesh, const string _instruction, const int _depth) {
    resetBoundingBox();
    bool branching = false;
    auto instructions = getInstructionsFromString(_instruction);

    for (auto stringInstruction : instructions) {
        auto inst = ofxLSInstruction(stringInstruction);
        auto head = inst.getHead();
        if (head == "F") {
            branching =  true;
        }else if( head == "G") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->boom(inst.getLength(defaultLength));
            branchContainer.push_back(newJoin);
        }else if (head == "+") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->rollDeg(+inst.getAngle(theta));
            if(randomYRotation){
                newJoin->panDeg(ofRandom(30.00, 330.00));
            }
            branchContainer.push_back(newJoin);
        }else if (head == "-") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->rollDeg(-inst.getAngle(theta));
            if(randomYRotation){
                newJoin->panDeg(ofRandom(30.00, 330.00));
            }
            branchContainer.push_back(newJoin);
        }else if (head == "|") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->panDeg(+inst.getAngle(180.00));
            branchContainer.push_back(newJoin);
        }else if (head == "&") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->tiltDeg(+inst.getAngle(theta));
            branchContainer.push_back(newJoin);
        }
        else if (head == "^") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->tiltDeg(-inst.getAngle(theta));
            branchContainer.push_back(newJoin);
        }
        else if (head == "\\") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->panDeg(+inst.getAngle(theta));
            branchContainer.push_back(newJoin);
        }
        else if (head == "/") {
            shared_ptr<ofNode> newJoin(new ofNode);
            newJoin->setParent(*branchContainer.back());
            newJoin->panDeg(-inst.getAngle(180.00));
            branchContainer.push_back(newJoin);
        }
        else if (head == "[") {
            bookmarks.push_back(branchContainer.back());
        }
        else if (head == "]") {
            branchContainer.push_back(bookmarks.back());
            bookmarks.pop_back();
        }

        if (branching) {
            float length = inst.getLength(defaultLength);
            auto beginBranch = branchContainer.back();
            shared_ptr<ofNode> endBranch(new ofNode);
            endBranch->setParent(*branchContainer.back());
            endBranch->move(ofVec3f(0, length, 0));

            maybeVectorExpandsBoundingBox(endBranch->getGlobalPosition());

            auto widths = getPrevAndCurrentWidth(length);
            auto newBranch = ofxLSBranch(*beginBranch, *endBranch, widths);
            geometryBuilder.putIntoMesh(newBranch, mesh, geometry, resolution, length, textureRepeat);
            branchContainer.push_back(endBranch);
            branching = false;
        }
    }

    //TODO, separate the generation of the node skeleton to mesh construction
//    float distance;
//    for(auto b : branchContainer){
//        if(b->getParent() != nullptr){
//            cout << "Start"<< endl;
//            cout << b->getParent()->getGlobalPosition().x;
//            cout << b->getParent()->getGlobalPosition().y;
//            cout << b->getParent()->getGlobalPosition().x;
//            cout << "End" << endl;
//            cout << b->getGlobalPosition().x;
//            cout << b->getGlobalPosition().y;
//            cout << b->getGlobalPosition().x;
//        }else{
//            //root point
//        }
//
//    }
    branchContainer.clear();
    bookmarks.clear();
    historySizes.clear();
}

// In case there is the need to keep track of the differents branches width and lenght,
// as in the case when scaleWidth is set to true, this method does 2 things:
// 1) it stores all the sizes in the container historySizes, for each pair, the first calue is the lenght
// the second the width.
// 2) It returns the current width and the previous one, as the method name says.
// If there is no need to keep track of the branch  width, just returns a pair containing
// the default width, both for the prev and current width.
pair<float, float> ofxLSTurtle::getPrevAndCurrentWidth(float currentLength){
    if(!scaleWidth){
        return make_pair(width, width);
    }


    float currentWidth = (scaleWidth) ? getScaledWidth(currentLength) : width;
    if (historySizes.empty()) {
        historySizes.insert(make_pair(currentLength, currentWidth));
        return (make_pair(currentWidth, currentWidth));
    } else {
        map<float, float>::iterator current;
        if (historySizes.find(currentLength) == historySizes.end()) {
            historySizes.insert(make_pair(currentLength, currentWidth));
            current = historySizes.begin();
        } else {
            current = historySizes.find(currentLength);
        }
        auto prev = current;
        ++prev;
        return make_pair(prev->second, current->second);
    }
}

//it scales the with proportionally to the scaled length
//
float ofxLSTurtle::getScaledWidth(float currentLength){
    auto ratio = (defaultLength / currentLength );
    float currentWidth = width / ratio;
    return currentWidth;
    if (currentWidth < 0.2) {
        return 0.2;
    } else {
        return currentWidth;
    }
}

vector<string> ofxLSTurtle::getInstructionsFromString(string _str){
    // This regex deserves an explanation. Taking a string like:
    // "F[+F-F(1.0,3.2)^F&F\\F/F?|]YU(2.0)A(2.232)?\\/"
    // it returns a vector containing
    //    F
    //    +
    //    F
    //    -
    //    F(1.0,3.2)
    //    ^
    //    F
    //    &
    //    F
    //    \
    //    F
    //    /
    //    F
    //    ?
    //    |
    //    Y
    //    U(2.0)
    //    A(2.232)
    //    ?
    //    \
    //    /

    // Explanation:
    // "([A-Z\\^&\\+-\\?\\|/\\]\\[\\\\](\\([0-9\\.,]+\\))*)" this means take any string that start with a capital letter,
    // a ^, a & a +, -, a | a '/', a'\' a'[', a ']'and, if there are, take any number or point '.' enclosed between ()
    return ofxLSUtils::matchesInRegex(_str,"([A-Z\\^&\\+\\-\\?\\|/\\]\\[\\\\](\\([0-9\\.,]+\\))*)");
}

// keep in mind that this method does not consider the thikness of a branch, but just the position of the vertices
// that will be used later to generate the cylinder. Therefore, it is not accurate, but is is usefull to get the UV
// if you want higher precision, call ofxLSystem.computeBoundingBox(), that iterates on all the vertices composing the
// mesh
void ofxLSTurtle::maybeVectorExpandsBoundingBox(ofVec3f v){
    if (v.x < buildedBoundingBox.min.x) buildedBoundingBox.min.x = v.x;
    if (v.y < buildedBoundingBox.min.y) buildedBoundingBox.min.y = v.y;
    if (v.z < buildedBoundingBox.min.z) buildedBoundingBox.min.z = v.z;

    if (v.x > buildedBoundingBox.max.x) buildedBoundingBox.max.x = v.x;
    if (v.y > buildedBoundingBox.max.y) buildedBoundingBox.max.y = v.y;
    if (v.z > buildedBoundingBox.max.z) buildedBoundingBox.max.z = v.z;
};

void ofxLSTurtle::resetBoundingBox(){
    buildedBoundingBox.min = ofVec3f(0,0,0);
    buildedBoundingBox.max = ofVec3f(0,0,0);
}
