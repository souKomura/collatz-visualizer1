#include "term.hpp"

//--------------------------------------------------------------
Term::Term()
: Term(0)
{}

//--------------------------------------------------------------
Term::Term(int n){
    this->number = n;
    this->level = -1;
    this->next_number = -1; // only 1 (limit of 3x+1) has this property
    this->pos = glm::vec3(0, 0, 0);
}
