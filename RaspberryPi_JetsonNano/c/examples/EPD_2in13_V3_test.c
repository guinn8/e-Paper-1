/*****************************************************************************
* | File      	:   EPD_2in13_V3_test.c
* | Author      :   Waveshare team
* | Function    :   2.13inch e-paper V3 test demo
* | Info        :
*----------------
* |	This version:   V1.1
* | Date        :   2021-10-30
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_Test.h"
#include "EPD_2in13_V3.h"
#include <time.h> 


void initEPD() {
    Debug("EPD_2in13_V3_test Demo\r\n");
    if(DEV_Module_Init() != 0) {
        exit(-1); // exit if initialization fails
    }
    Debug("e-Paper Init and Clear...\r\n");
    EPD_2in13_V3_Init();
}

void measureClearTime() {
    struct timespec start = {0,0}, finish = {0,0};
    clock_gettime(CLOCK_REALTIME, &start);
    EPD_2in13_V3_Clear();
    clock_gettime(CLOCK_REALTIME, &finish);
    Debug("%ld S\r\n", finish.tv_sec - start.tv_sec);
}

UBYTE* createAndConfigImage() {
    UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8) : (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
    UBYTE *BlackImage = (UBYTE *)malloc(Imagesize);
    if(BlackImage == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        exit(-1);
    }
    Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90, WHITE);
    Paint_Clear(WHITE);
    return BlackImage;
}

void drawOnImage(UBYTE *BlackImage) {
    Debug("Drawing\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawNum(0, 10, 123456789, &Font24, WHITE, BLACK);
}

void finalizeEPD(UBYTE *BlackImage) {
    Debug("Goto Sleep...\r\n");
    EPD_2in13_V3_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);
    Debug("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
}

void drawNumberBox(UWORD x, UWORD y, UWORD boxSize, int number, sFONT* font) {
    char buf[4];
    snprintf(buf, sizeof(buf), "%d", number);
    Paint_DrawRectangle(x, y, x + boxSize, y + boxSize, BLACK, 2, DRAW_FILL_EMPTY);
    Paint_DrawString_EN(x, y + 5, buf, font, WHITE, BLACK);
}

void drawShoeSizes(UBYTE *BlackImage, int numbers[], int stockNumbers[], int numBoxes) {
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // Constants
    const UWORD titleHeight = 30;
    const UWORD boxSize = 35;
    const UWORD xStart = 3;
    UWORD y = titleHeight + 10;

    // Draw title bar
    Paint_DrawRectangle(0, 0, EPD_2in13_V3_HEIGHT, titleHeight, BLACK, 2, DRAW_FILL_EMPTY);
    Paint_DrawString_EN(5, 5, "Cool Shoe", &Font24, WHITE, BLACK);

    // Draw sizes
    UWORD x = xStart;
    for(int i = 0; i < numBoxes; i++) {
        drawNumberBox(x, y, boxSize, numbers[i], &Font24);
        x += boxSize;
    }

    // Draw stock numbers below sizes
    y += boxSize;
    x = xStart;
    for(int i = 0; i < numBoxes; i++) {
        drawNumberBox(x, y, boxSize, stockNumbers[i], &Font24);
        x += boxSize;
    }
}




int EPD_2in13_V3_test(void) {
    initEPD();
    measureClearTime();
    UBYTE *BlackImage = createAndConfigImage();
    // drawOnImage(BlackImage);

    int sizes[] = {8, 9, 10, 11, 12, 13, 14};
    int stockNumbers[] = {0, 2, 5, 3, 4, 1, 5}; // Example stock numbers, you can populate this differently
    int numBoxes = sizeof(sizes) / sizeof(sizes[0]);  // Calculate the number of elements in the array

    // Draw shoe sizes
    drawShoeSizes(BlackImage, sizes, stockNumbers, numBoxes);

    // // Draw stock numbers
    // drawShoeSizes(BlackImage, stockNumbers, numBoxes);

    EPD_2in13_V3_Display_Base(BlackImage);
    finalizeEPD(BlackImage);

    return 0;
}
