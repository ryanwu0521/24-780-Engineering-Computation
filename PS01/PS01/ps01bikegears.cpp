// Ryan Wu
// ID:weihuanw
// Carnegie Mellon University
// Eng. Computation, 24-780-B
// Prob. Set 1, Due Tues. Sept. 5, 2023 (Approved homework extension)
// Calculate and print gear ratios for all combinations of mountain bike's 27 speeds.

#include <iostream>
#include <term.h>
using namespace std;
int main(void)
{
    cout << ("24-780-B Engineering Computation Prob Set 1 – Bike Speed Calculator \n") << endl;
    cout << ("Gear Ratios:\n") << endl;
    int frontGears[] = { 0, 22, 32, 44 };                           // gear ratio inputs
    int rearGears[] = { 0, 34, 30, 26, 23, 20, 18, 16, 14, 12 };    // gear ratio inputs
    for (int x = 1; x <= 3; x++)                                    // x = frontGears
    for (int y = 1; y <= 9; y++)                                    // y = rearGears
    cout << x << '-'
         << y << ": "
         << (double)frontGears[x] / rearGears[y] << endl;
    printf("\n");
    printf("Press the ENTER key to close the program >");
    getc(stdin);
    system("clear");                                                // system("clear") does not work on Xcode
}
