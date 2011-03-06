#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

class testApp : public ofSimpleApp{
	
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
};

#endif

