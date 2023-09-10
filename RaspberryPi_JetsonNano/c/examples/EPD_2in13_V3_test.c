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
#include <assert.h>  // For assert() function

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Initialize E-Paper Display (EPD)
void initEPD() {
    Debug("EPD_2in13_V3_test Demo\r\n");
    if(DEV_Module_Init() != 0) {
        exit(-1);
    }
    EPD_2in13_V3_Init();
}

// Measure the time required to clear the EPD
void measureClearTime() {
    struct timespec start = {0,0}, finish = {0,0};
    clock_gettime(CLOCK_REALTIME, &start);
    EPD_2in13_V3_Clear();
    clock_gettime(CLOCK_REALTIME, &finish);
    Debug("%ld S\r\n", finish.tv_sec - start.tv_sec);
}

// Create and configure image buffer
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

// Finalize EPD and free resources
void finalizeEPD(UBYTE *BlackImage) {
    EPD_2in13_V3_Sleep();
    free(BlackImage);
    DEV_Delay_ms(2000);
    DEV_Module_Exit();
}

#define TOTAL_BOXES 7
#define EPD_WIDTH EPD_2in13_V3_HEIGHT
#define BOX_GAP 2
#define BOX_SIZE ((EPD_WIDTH - (BOX_GAP * (TOTAL_BOXES - 1))) / TOTAL_BOXES)
// Draw the title bar
void drawTitle(const char *title) {
    Paint_DrawRectangle(0, 0, EPD_2in13_V3_HEIGHT, 30, BLACK, 2, DRAW_FILL_FULL);
    Paint_DrawString_EN(5, 5, title, &Font24, BLACK, WHITE);
}



typedef enum {
    STYLE_DEFAULT,
    STYLE_INVERTED,
    STYLE_INVERTED_STRIKE,
} NumberBoxStyle;

typedef struct {
    UWORD number;
    NumberBoxStyle style;
} NumberBox_t;

void drawNumberBox(UWORD x, UWORD y, NumberBox_t *numberbox){
    assert(numberbox->number >= 0 && numberbox->number <= 99);
    assert((x + BOX_SIZE) <= EPD_2in13_V3_HEIGHT);
    assert((y + BOX_SIZE) <= EPD_2in13_V3_WIDTH);

    const UWORD rectXEnd = x + BOX_SIZE - 1;
    const UWORD rectYEnd = y + BOX_SIZE - 1;
    const int num_digits = (numberbox->number > 9) ? 2 : 1;
    const int totalTextWidth = num_digits * 17;
    const int totalTextHeight = 22;
    UWORD textX = (x + (BOX_SIZE - totalTextWidth) / 2);
    const UWORD textY = y + (BOX_SIZE - totalTextHeight) / 2;

    int foreground, background, fill, fill_colour, stroke_width;
    switch(numberbox->style) {
        case STYLE_INVERTED_STRIKE:
            Paint_DrawLine(x, y, rectXEnd, rectYEnd, BLACK, 2, LINE_STYLE_SOLID);
        case STYLE_INVERTED:
            foreground = BLACK;
            background = WHITE;
            fill = DRAW_FILL_EMPTY;
            fill_colour = foreground;
            stroke_width = 2;
            break;
        default:  // STYLE_DEFAULT
            foreground = WHITE;
            background = BLACK;
            fill = DRAW_FILL_FULL;
            fill_colour = background;
            stroke_width = 1;
            y += 1;
            textX -= 1;
    }

    Paint_DrawRectangle(x, y, rectXEnd, rectYEnd, fill_colour, stroke_width, fill);
    Paint_DrawNum(textX, textY, numberbox->number, &Font24, foreground, background);
}

void drawNumberBoxes(UBYTE *BlackImage, NumberBox_t numbers[], int numBoxes, const UWORD yOffset) {
    assert(numBoxes <= TOTAL_BOXES);
    for (UWORD x = 0, i = 0; i < numBoxes; ++i) {
        drawNumberBox(x, yOffset, &numbers[i]);
        x += (BOX_SIZE + BOX_GAP);
    }
}

int EPD_2in13_V3_test(void) {
    initEPD();
    measureClearTime();
    UBYTE *BlackImage = createAndConfigImage();
    drawTitle("Oboz Bridger");

    NumberBox_t sizes[] = {
        {.style = STYLE_DEFAULT, .number = 8},
        {.style = STYLE_INVERTED, .number = 9},
        {.style = STYLE_INVERTED_STRIKE, .number = 10},
        {.style = STYLE_DEFAULT, .number = 11},
        {.style = STYLE_DEFAULT, .number = 12},
        {.style = STYLE_DEFAULT, .number = 13},
        {.style = STYLE_INVERTED_STRIKE, .number = 14},
    };

    int numBoxes = ARRAY_SIZE(sizes);  // Using the macro here
    Paint_DrawString_EN(5, 35, "$260.00", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(5, 60, "Best seller!", &Font24, WHITE, BLACK);

    drawNumberBoxes(BlackImage, sizes, numBoxes, 88);
    EPD_2in13_V3_Display_Base(BlackImage);
    finalizeEPD(BlackImage);

    return 0;
}
