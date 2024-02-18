// Ryan Wu
// ID: weihuanw
// Carnegie Mellon University
// Eng. Computation, 24-780-B
// Individual Project (Demo), Due Sun. Oct. 29, 2023
// This demo is a recreation of the classic bouncing DVD logo screensaver with my personal twist. The program has 2 DVD logos bouncing off walls and colliding with each other for 60 seconds. A countdown timer, wall bounced counter, and a logo collision counter are included in the program. After 60 seconds of run time, the program exit with a Halloween theme closing message.

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <cmath>
#include <string>

#include "fssimplewindow.h"
#include "yspng.h"
#include "yssimplesound.h"
#include "ysglfontdata.h"
#include "GraphicFont.h"

// make sure the DVD logo does not turn back during random color generation
bool IsBlackColor(unsigned char r, unsigned char g, unsigned char b)
{
    return r == 0 && g == 0 && b == 0;
}

// DVD logo random color generation (non-black)
void GenerateRandomColor(unsigned char &r, unsigned char &g, unsigned char &b)
{
    do
    {
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;
    } while (IsBlackColor(r, g, b)); // ensure it's not black
}

// my main fucntion
int main(void)
{
    FsOpenWindow(16, 16, 800, 600, 1);  // open graphic and sound

    // font data
    TimesNewRomanFont timesnewroman;
    ComicSansFont comicsans;
    ImpactFont impact;
    
    // image data
    YsRawPngDecoder png1, png2;
    
    // sound data
    YsSoundPlayer soundPlayer;
    YsSoundPlayer::SoundData backgroundSound, hitWallSound, collisionSound, timerSound, thankYouSound;
    
    // check loaded image
    if (YSOK != png1.Decode("dvd_logo.png") || YSOK != png2.Decode("dvd_logo.png"))
    {
        std::cerr << "Failed to load DVD logo images!" << std::endl;
        return 1;
    }

    // start sound player and load input sound wav
    if (YSOK == backgroundSound.LoadWav("halloween_theme.wav") &&
        YSOK == hitWallSound.LoadWav("hit_wall.wav") &&
        YSOK == collisionSound.LoadWav("window_error.wav") &&
        YSOK == timerSound.LoadWav("count_down.wav") &&
        YSOK == thankYouSound.LoadWav("thank_you.wav"))
    {
        soundPlayer.Start();
        soundPlayer.PlayBackground(backgroundSound);
    }
    else
    {
        std::cerr << "Failed to initialize sound player!" << std::endl;
        return 1;
    }

    // declare varibles for DVD logo 1
    int imgWidth1 = png1.wid;
    int imgHeight1 = png1.hei;
    int x1 = 200 - imgWidth1 / 8;
    int y1 = 100 - imgHeight1 / 8;
    int dx1 = -3;
    int dy1 = -3;
    unsigned char r1, g1, b1;
    GenerateRandomColor(r1, g1, b1);

    // declare varibles for DVD logo 2
    int imgWidth2 = png2.wid;
    int imgHeight2 = png2.hei;
    int x2 = 600 - imgWidth2 / 8;
    int y2 = 500 - imgHeight2 / 8;
    int dx2 = +3;
    int dy2 = +3;
    unsigned char r2, g2, b2;
    GenerateRandomColor(r2, g2, b2);
    
    // declaring timer, wall & collision counter varibles
    int collisionCounter = 0;
    int wallCounter = 0;
    auto startTime = std::chrono::steady_clock::now();
    bool collisionOccurred = false;
    bool timerSoundPlayed = false;
    std::chrono::steady_clock::time_point collisionStartTime;

    // main DVD screensaver animation loop
    while (FsInkey() != FSKEY_ESC)
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        
        int distX = x1 - x2;
        int distY = y1 - y2;
        int distance = sqrt(distX * distX + distY * distY);
        
        if (distance < (imgWidth1 / 10) + (imgWidth2 / 10))
        {
            collisionCounter++;
            dx1 *= -1;
            dy1 *= -1;
            dx2 *= -1;
            dy2 *= -1;
            
            GenerateRandomColor(r1, g1, b1);
            GenerateRandomColor(r2, g2, b2);
            soundPlayer.PlayOneShot(collisionSound);
            
            collisionOccurred = true;
            collisionStartTime = std::chrono::steady_clock::now();
        }
        
        FsPollDevice();
        glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glEnable(GL_TEXTURE_2D);
        GLuint textureID1, textureID2;
        glGenTextures(1, &textureID1);
        glGenTextures(1, &textureID2);
        
        // drawing Logo 1
        glBindTexture(GL_TEXTURE_2D, textureID1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth1, imgHeight1, 0, GL_RGBA, GL_UNSIGNED_BYTE, png1.rgba);
        glColor3ub(r1, g1, b1);
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        glVertex2i(x1 - imgWidth1 / 8, y1 - imgHeight1 / 8);
        glTexCoord2d(1.0, 0.0);
        glVertex2i(x1 + imgWidth1 / 8, y1 - imgHeight1 / 8);
        glTexCoord2d(1.0, 1.0);
        glVertex2i(x1 + imgWidth1 / 8, y1 + imgHeight1 / 8);
        glTexCoord2d(0.0, 1.0);
        glVertex2i(x1 - imgWidth1 / 8, y1 + imgHeight1 / 8);
        glEnd();
        glDeleteTextures(1, &textureID1);
        
        // drawing Logo 2
        glBindTexture(GL_TEXTURE_2D, textureID2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth2, imgHeight2, 0, GL_RGBA, GL_UNSIGNED_BYTE, png2.rgba);
        glColor3ub(r2, g2, b2);
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        glVertex2i(x2 - imgWidth2 / 8, y2 - imgHeight2 / 8);
        glTexCoord2d(1.0, 0.0);
        glVertex2i(x2 + imgWidth2 / 8, y2 - imgHeight2 / 8);
        glTexCoord2d(1.0, 1.0);
        glVertex2i(x2 + imgWidth2 / 8, y2 + imgHeight2 / 8);
        glTexCoord2d(0.0, 1.0);
        glVertex2i(x2 - imgWidth2 / 8, y2 + imgHeight2 / 8);
        glEnd();
        glDeleteTextures(1, &textureID2);
        
        // collision counter
        timesnewroman.setColorHSV(35, 1, 1); // set font color to orange
        std::string collisionText = "Logo Collided: " + std::to_string(collisionCounter);
        timesnewroman.drawText(collisionText.c_str(), 560, 50, 0.35);
        
        // wall bounce counter
        timesnewroman.setColorHSV(35, 1, 1); // set font color to orange
        std::string wallText = "Wall Bounced: " + std::to_string(wallCounter);
        timesnewroman.drawText(wallText.c_str(), 290, 50, 0.35);
        
        // count down timer
        int remainingTime = 60 - static_cast<int>(elapsedTime / 1000); // update remaining time in seconds
        int newElapsedTime = static_cast<int>(elapsedTime/ 1000);
        timesnewroman.setColorHSV(35, 1, 1); // set font color to orange
        std::string timerText = "Time Remain: " + std::to_string(remainingTime) + "s";
        timesnewroman.drawText(timerText.c_str(), 0, 50, 0.35);
        
        // showing collision message in window
        if (collisionOccurred)
        {
            impact.setColorHSV(0, 1, 1); // set font color to red
            std::string collisionMessage = "Collision Occurred!";
            impact.drawText(collisionMessage.c_str(), 150, 600, 1);
            
            auto collisionElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - collisionStartTime).count();
            
            if (collisionElapsedTime >=1000)
            {
                collisionOccurred = false; // reset the collision message
            }
        }
        
        FsSwapBuffers();
        FsSleep(3); // animation speed (larger = slower)
        
        // position updates for both DVD logos
        x1 += dx1;
        y1 += dy1;
        x2 += dx2;
        y2 += dy2;
        
        // wall bounce logic for DVD Logo 1
        if (x1 - imgWidth1 / 8 <= 0 || x1 + imgWidth1 / 8 >= 800)
        {
            dx1 *= -1; // change direction
            GenerateRandomColor(r1, g1, b1);
            soundPlayer.PlayOneShot(hitWallSound);
            wallCounter++;
        }
        if (y1 - imgHeight1 / 8 <= 0 || y1 + imgHeight1 / 8 >= 600)
        {
            dy1 *= -1; // change direction
            GenerateRandomColor(r1, g1, b1);
            soundPlayer.PlayOneShot(hitWallSound);
            wallCounter++;
        }
        
        // wall bounce logic for DVD Logo 2
        if (x2 - imgWidth2 / 8 <= 0 || x2 + imgWidth2 / 8 >= 800)
        {
            dx2 *= -1; // change direction
            GenerateRandomColor(r2, g2, b2);
            soundPlayer.PlayOneShot(hitWallSound);
            wallCounter++;
        }
        if (y2 - imgHeight2 / 8 <= 0 || y2 + imgHeight2 / 8 >= 600)
        {
            dy2 *= -1; // change direction
            GenerateRandomColor(r2, g2, b2);
            soundPlayer.PlayOneShot(hitWallSound);
            wallCounter++;
        }
        
        // count down timer sound player logic
        if (remainingTime == 4 && !timerSoundPlayed)
        {
            soundPlayer.PlayOneShot(timerSound);
            timerSoundPlayed = true; // reset the sound player
        }
        
        // exit loop after 60 seconds
        if (elapsedTime >= 60050)
        {
            // writing closing message
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // change background to white
            glClear(GL_COLOR_BUFFER_BIT);
            comicsans.setColorHSV(0, 0, 0); // set font color to black
            std::string timesUpMessage = "Time's Up! Happy Spooky Halloween Y'all :)";
            comicsans.drawText(timesUpMessage.c_str(), 10, 330, 0.4);
            
            // load and draw a cute pumpkin image
            YsRawPngDecoder pumpkinImg;
            if (YSOK == pumpkinImg.Decode("pumpkin.png"))
            {
                glEnable(GL_TEXTURE_2D);
                GLuint pumpkinTexture;
                glGenTextures(1, &pumpkinTexture);
                glBindTexture(GL_TEXTURE_2D, pumpkinTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pumpkinImg.wid, pumpkinImg.hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, pumpkinImg.rgba);
                
                glColor3ub(255, 255, 255); // Set color to white
                glBegin(GL_QUADS);
                glTexCoord2d(0.0, 0.0);
                glVertex2i(400 - pumpkinImg.wid / 8, 130 - pumpkinImg.hei/ 8);
                glTexCoord2d(1.0, 0.0);
                glVertex2i(400 + pumpkinImg.wid / 8, 130 - pumpkinImg.hei/ 8);
                glTexCoord2d(1.0, 1.0);
                glVertex2i(400 + pumpkinImg.wid / 8, 130 + pumpkinImg.hei/ 8);
                glTexCoord2d(0.0, 1.0);
                glVertex2i(400 - pumpkinImg.wid / 8, 130 + pumpkinImg.hei/ 8);
                glEnd();
                
                glDeleteTextures(1, &pumpkinTexture);
            }
            
            // load and draw a cute ghost image
            YsRawPngDecoder ghostImg;
            if (YSOK == ghostImg.Decode("ghost.png"))
            {
                glEnable(GL_TEXTURE_2D);
                GLuint ghostTexture;
                glGenTextures(1, &ghostTexture);
                glBindTexture(GL_TEXTURE_2D, ghostTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ghostImg.wid, ghostImg.hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, ghostImg.rgba);
                
                glColor3ub(255, 255, 255); // Set color to white
                glBegin(GL_QUADS);
                glTexCoord2d(0.0, 0.0);
                glVertex2i(400 - ghostImg.wid / 6, 470 - ghostImg.hei / 6);
                glTexCoord2d(1.0, 0.0);
                glVertex2i(400 + ghostImg.wid / 6, 470 - ghostImg.hei / 6);
                glTexCoord2d(1.0, 1.0);
                glVertex2i(400 + ghostImg.wid / 6, 470 + ghostImg.hei/ 6);
                glTexCoord2d(0.0, 1.0);
                glVertex2i(400 - ghostImg.wid / 6, 470 + ghostImg.hei/ 6);
                glEnd();
                
                glDeleteTextures(1, &ghostTexture);
            }
            
            FsSleep(1000); // 1 second delay
            
            // play thank you sound effect
            soundPlayer.PlayOneShot(thankYouSound);
            FsSwapBuffers();
            FsSleep(4000); // 4 seconds delay
            break;
        }
    }
    // extra validation for number of collision printed in the termial window
    std::cout << "Number of Collisions: " << collisionCounter << std::endl;

    // closing out the program
    soundPlayer.End();
    FsCloseWindow();
    return 0;
}
