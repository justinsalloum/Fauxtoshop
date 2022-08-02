/*
 * CS 106B/X Fauxtoshop
 * This instructor-provided file defines functions to help you open
 * and save images.
 * See fauxtoshop-provided.h for the function declarations and documentation.
 *
 * You don't need to modify this file.
 * Your code must work properly with an unmodified version of this file.
 *
 * @author Cynthia Lee and Marty Stepp
 * @version 2017/09/28
 * - initial version for 17au
 * - added "?" for file dialog to load/save images
 * - added showDiffWindow to display differing pixels from an image
 * - added fakeRandomNumberGenerator to fix randomness
 */

#include "fauxtoshop-provided.h"
#include <cmath>
#include "gevents.h"
#include "gfilechooser.h"
#include "gmath.h"
#include "gwindow.h"
#include "random.h"
#include "simpio.h"

// constants

const int WHITE = 0xFFFFFF;
const int BLACK = 0x000000;
const int GREEN = 0x00FF00;
const int NUM_MENU_OPTIONS = 4;
const std::string IMAGE_TYPES = "*.bmp,*.gif,*.ppm,*.jpg,*.png";

// function definitions

void fakeRandomNumberGenerator() {
    setRandomSeed(106);   // CS 106B/X forever!
    for (int i = 0; i < 1000000; i++) {
        autograder::randomFeedInteger(randomInteger(0, 10000));
    }
}

Vector<double> gaussKernelForRadius(int radius) {
    if (radius < 1) {
        Vector<double> empty;
        return empty;
    }
    Vector<double> kernel(radius * 2 + 1);
    double magic1 = 1.0 / (2.0 * radius * radius);
    double magic2 = 1.0 / (sqrt(2.0 * PI) * radius);
    int r = -radius;
    double div = 0.0;
    for (int i = 0; i < kernel.size(); i++) {
        double x = r * r;
        kernel[i] = magic2 * exp(-x * magic1);
        r++;
        div += kernel[i];
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel[i] /= div;
    }
    return kernel;
}

void getMouseClickLocation(int& row, int& col) {
    GMouseEvent event;
    do {
        event = getNextEvent(MOUSE_EVENT);
    } while (event.getEventType() != MOUSE_CLICKED);
    row = event.getY();
    col = event.getX();
}

bool openImageFromFilename(GBufferedImage& img, std::string filename) {
    if (filename == "?") {
        filename = GFileChooser::showOpenDialog("", IMAGE_TYPES);
    }
    try {
        img.load(filename);
        return true;
    } catch (ErrorException) {
        return false;
    }
}

bool saveImageToFilename(GBufferedImage& img, std::string filename) {
    if (filename == "?") {
        filename = GFileChooser::showSaveDialog("", IMAGE_TYPES);
    }
    try {
        img.save(filename);
        return true;
    } catch (ErrorException) {
        return false;
    }
}

void showDiffWindow(const GWindow& gw, std::string filename) {
    gw.compareToImage(filename);
    std::cout << "Press Enter to continue ... ";
    getLine();
}
