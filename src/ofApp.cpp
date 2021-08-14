#include "ofApp.h"
#include "ofMain.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    //ofLogToFile("log/myLogFile.txt", true);
    
    font1.load("sauce_code_pro2.ttf", 8);
    font2.load("sauce_code_pro2.ttf", 6);
    
    displayLevel = 20;
    
    //every number converge to one.
    terms.clear();
    terms[1] = Term(1);
    terms[1].level = 0;
    
    //register numbers to terms:unordered_map.
    //terms[i] is basically i(int) with previous number, next number and position of node.
    for(int i=1; i<100000; i++){
        registerTilKnown(i);
    }
    
    //terms[i] is i as collatz term (tyep: Term).
    //byLevel[lev] contains pointer of term which has lev steps to 1.
    byLevel = BFS(terms);
    
    //set position from tree structure
    setPosition();
}

//--------------------------------------------------------------
void ofApp::registerTilKnown(int n){
    /*
     * add n into terms, with previous numbers and next number.
     */
    
    if(terms[n].number != 0){
        return;
    }
    
    // when n=3, next=10. but 10 doesn't (yet) exist.
    // before register n to next number, as previous one, make sure terms[next number] exists.
    // 3x+1 is known as converging to 1 almost surely. So recursion will finish.
    int next_n = nextNumber(n);
    registerTilKnown(next_n);
    
    terms[n] = Term(n);
    terms[n].next_number = next_n;
    terms[n].level = terms[next_n].level + 1;
    terms[next_n].prevTerms.push_back(n);
    
    return;
}

int ofApp::nextNumber(int x){
    if(x%2 == 0){
        return x/2;
    } else {
        return 3*x + 1;
    }
}

//--------------------------------------------------------------
unordered_map<int, vector<Term*>> ofApp::BFS(unordered_map<int, Term> & _terms){
    /*
     * check every number in terms by breadth first search.
     */
    
    unordered_map<int, vector<Term*>> result;
    
    Term* startPtr = &(_terms[1]);
    queue<Term*> que;
    que.push(startPtr);
    
    while(!que.empty()){
        
        Term* nowPtr = que.front();
        que.pop();
        
        result[nowPtr->level].push_back(nowPtr);
        
        for(int child: nowPtr->prevTerms){
            que.push(&(_terms[child]));
        }
    }
    
    return result;
}

//--------------------------------------------------------------
void ofApp::setPosition(){
    /*
     * set terms position(glm::vec3).
     * x ... [-0.5, 0.5]
     * y ... [0. 1]
     * z ... 0
     */
    
    //1.  y is easy.
    for(int lev=0; lev < displayLevel; lev++){
        for(Term* tPtr: byLevel[lev]){
            //if lev=0 then y=1.0
            //if lev=displayLevel then y=0.0
            tPtr->pos.y = 1.0 - float(lev+1)/displayLevel;
        }
    }
    
    //2. top level. same to y coord.
    size_t topSize = byLevel[displayLevel-1].size();
    ofLogNotice() << displayLevel-1 << "(top) : " << byLevel[displayLevel-1].size();
    for(int i=0; i<topSize; i++){
        byLevel[displayLevel-1][i]->pos.x = (float)i/topSize;
    }
    
    //3. another levels.
    // Thir pos are depanding on previous layer.
    for(int lev=displayLevel-2; lev>=0; lev--){
        for(int i=0; i<byLevel[lev].size(); i++){
            
            Term* tPtr = byLevel[lev][i];
            
            float prevXSum = 0;
            int cnt = 0;
            for(int prev: tPtr->prevTerms){
                prevXSum += terms[prev].pos.x;
                cnt++;
            }
            if(cnt > 0){
                tPtr->pos.x = prevXSum / cnt;
            } else{
                tPtr->pos.x = 0;
            }
        }
    }
    
    //4. centering
    for(int lev=0; lev<displayLevel; lev++){
        float maxX = 0;
        float minX = ofGetWidth();
        for(Term* tPtr: byLevel[lev]){
            if(tPtr->pos.x > maxX) maxX = tPtr->pos.x;
            if(tPtr->pos.x < minX) minX = tPtr->pos.x;
        }
        
        float diff = (maxX + minX) / 2;
        
        for(Term* tPtr: byLevel[lev]){
            tPtr->pos.x -= diff;
        }
    }

    //5. easing edges.
    float Thr = 1/8.0;
    for(int lev=1; lev<displayLevel; lev++){
        for(Term* tPtr: byLevel[lev]){

            float parentX = terms[tPtr->next_number].pos.x;
            float diffX = parentX - tPtr->pos.x;

            if(abs(diffX) > Thr){
                tPtr->pos.x = parentX + -Thr * diffX/abs(diffX);
            }
        }
    }
    
    //6. more easing
    for(int lev=1; lev<displayLevel-1; lev++){
        for(Term* tPtr: byLevel[lev]){
            
            if(tPtr->prevTerms.size() != 1) continue;
            
            float prevX = terms[tPtr->prevTerms[0]].pos.x;
            float nextX = terms[tPtr->next_number].pos.x;
            
            tPtr->pos.x += ((prevX + nextX) / 2 - tPtr->pos.x) * 0.5;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(0, 0, 36), ofColor(40, 40, 66));
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    
    // transform matrix.
    // (x, y, z) -> (wx, hy, z)
    float seed[9] = {w,0,0, 0,h,0, 0,0,1};
    glm::mat3 nodeTform = glm::make_mat3(seed);
    
    ofPushMatrix();
    ofTranslate(w/2, 5,  0);
    
    //display circle and lines.
    for(int lev=0; lev<displayLevel; lev++){
        for(int i=0; i<byLevel[lev].size(); i++){
            
            Term* tPtr = byLevel[lev][i];
            int number = tPtr->number;
            glm::vec3 drawpos = byLevel[lev][i]->pos * nodeTform;
            
            ofSetColor(gradiented(glm::pow(number / 20000.0, 0.3)));
            ofFill();
            ofSetLineWidth(2);
            
            if(lev != displayLevel-1){
                for(int child: tPtr->prevTerms){
                    ofDrawLine(drawpos, terms[child].pos * nodeTform);
                }
            }
            
            ofDrawCircle(drawpos, 14);
            
            ofSetColor(255, 255, 255);
            
            int digit = (int)log10(number) + 1;
            
            // displat number
            if(digit == 1){
                font1.drawString(to_string(tPtr->number), drawpos.x-3, drawpos.y+5);
            } else if (digit == 2){
                font1.drawString(to_string(tPtr->number), drawpos.x-7, drawpos.y+5);
            } else if (digit == 3){
                font2.drawString(to_string(tPtr->number), drawpos.x-8, drawpos.y+3);
            } else if (digit == 4){
                font2.drawString(to_string(tPtr->number), drawpos.x-8, drawpos.y+3);
            }else{
                font2.drawString(to_string(tPtr->number), drawpos.x-11, drawpos.y+3);
            }

        }
    }
    ofPopMatrix();
}

float ofApp::log10(float x){
    return log(x) / log(10);
}

ofColor ofApp::gradiented(float x){
    return ofColor(
                    MIN(110, ofMap(x, 0, 1, 32, 110)),
                    MIN(190, ofMap(x, 0, 1, 48, 190)),
                    MIN(255, ofMap(x, 0, 1, 128, 200))
                   );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 's'){
        ofSaveScreen("frames/" + ofToString(ofGetFrameNum()) + ".png");
    }
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
