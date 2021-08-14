#pragma once
#include "ofMain.h"
#include "term.hpp"
#include<unordered_map>

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
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void registerTilKnown(int n);
    unordered_map<int, vector<Term*>> BFS(unordered_map<int, Term> & _term);
    int nextNumber(int x);
    void setPosition();
    float log10(float x);
    ofColor gradiented(float x);
    
    unordered_map<int, Term> terms;
    unordered_map<int, vector<Term*>> byLevel;
    
    int displayLevel;
    ofTrueTypeFont font1;
    ofTrueTypeFont font2;
};
