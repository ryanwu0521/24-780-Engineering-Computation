// Ryan Wu
// ID: weihuanw
// Carnegie Mellon University
// Eng. Computation, 24-780-B
// Prob. Set 2-1 Plane Crash, Due Tues. Sept. 12, 2023


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <cmath>

void setRandomNumberSeedFromTime(void) {
  int currentTime;
  currentTime=time(NULL);
  srand(currentTime);
}
int main(void) {
// Given problem varibles (Test)
// double massPlane = 8000,
// cableLength = 20,
// thetaInit = 60,
// thetaFinal = 15 ,
// gravity = 9.81,
// velfinal,
// maxtension;
// float guess;
    
    // Define problem varibles' range
    int massPlane_min = 6000,
        massPlane_max = 10000,
        cableLength_min = 15,
        cableLength_max = 25,
        thetaInit_min = 50,
        thetaInit_max = 80,
        thetaFinal_min = 5,
        thetaFinal_max = 20;
    
    // Given problem varibles
    setRandomNumberSeedFromTime();
    double velfinal, maxtension, gravity = 9.81,
           massPlane = rand()% (massPlane_max - massPlane_min + 1) + massPlane_min,
           cableLength = rand()% (cableLength_max - cableLength_min + 1) + cableLength_min,
           thetaInit = rand()% (thetaInit_max - thetaInit_min + 1) + thetaInit_min,
           thetaFinal = rand()% (thetaFinal_max - thetaFinal_min + 1) + thetaFinal_min;
    float guess_velFinal;
    int guess_maxTension;
    
// Print random varible numbers (test)
// std::cout << massPlane << "," << cableLength << "," << thetaInit << ","
// << thetaFinal << std::endl;
    
    // Final velocity & max tension calculation
    velfinal = sqrt(2 * gravity * cableLength * (cos(thetaFinal*(M_PI/180))-cos(thetaInit*(M_PI/180))));
    maxtension = ((massPlane * (pow(velfinal,2)/cableLength)) + massPlane * gravity * (cos(thetaFinal*(M_PI/180))))/1000;
    
    // Final velocity & max tension rounding
    float velFinal = std::round(velfinal*10)/10;
    int maxTension = std::round(maxtension);
    
// Print final velocity & max tension (test)
// std::cout << velFinal << "," << maxTension << std::endl;

    // Given text question
    printf("The gantry structure in the photo is used to test the response of an airplane during a crash. As shown, the plane, having a massPlane of %.0f [g], ", massPlane);
    printf("is hoisted back until thetaInit is %.0f [°]", thetaInit);
    printf(", and then the pull-back cable is released when the plane is at rest. Determine the speed velFinal of the plane just before crashing into the ground when thetaFinal = %.0f [°]", thetaFinal);
    printf(". Also,what is the maximum tension maxTension developed in the supporting cable (which has length of cableLength) during the motion? Neglect the effect of lift caused by the wings during the motion and the size of the airplane.\n\n\n\n");

    // Comparing velFinal results
    printf("What is velFinal in [m/s]? :");
    scanf("%f",&guess_velFinal);
    printf("What is maxTension in [kN]? :");
    scanf("%d",&guess_maxTension);
    if(velFinal==guess_velFinal and maxTension==guess_maxTension) {
        printf("You've got both answers right!\n");
    }
    
    else if(velFinal==guess_velFinal and maxTension!=guess_maxTension)
    {
        printf("You've got the right answer for velFinal but wrong answer for maxTension!\nThe anwser for maxTension is %d [kN]\n", maxTension);
    }
    else if(velFinal!=guess_velFinal and maxTension==guess_maxTension)
    {
        printf("You've got the right answer for maxTension but wrong answer for velFinal!\nThe anwser for velFinal is %.1f [m/s]\n", velFinal);
    }
    else
    {
    printf("You got both answers wrong :(\nThe anwser for velFinal is %.1f [m/s]\n", velFinal);
    printf("The anwser for maxTension is %d [kN]\n", maxTension);
    }
return 0;
}
