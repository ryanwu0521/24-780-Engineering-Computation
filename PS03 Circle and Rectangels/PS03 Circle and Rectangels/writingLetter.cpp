#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include "fssimplewindow.h"
#include "ysglfontdata.h"

int main()
{
    FsOpenWindow(16, 16, 800, 600, 1);
    
    while(FsInkey() == FSKEY_NULL)  {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        glRasterPos2i(100, 100);  // sets position
        glColor3ub(0, 0, 255);
        YsGlDrawFontBitmap20x32("Test Message"); // there are other font sizes
        
        FsSwapBuffers();
        FsPollDevice();
    }
    
    return 0;
}
