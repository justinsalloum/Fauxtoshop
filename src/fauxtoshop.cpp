/**
 * This program implements a simple photoshop application. It prompts the user to enter an image
 * filename and then opens the image in a GUI. Upon this the user is presented with 4 choices of
 * filters which then can apply to the image: 1) Scatter 2) Edge detection 3) "Green screen" with
 * another image 4) Compare image with another image. If the user applies any of the first three
 * filters, they have the option to save their result as a new image.
 *
 * @author Justin Salloum
 */

#include <iostream>
#include "console.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "fauxtoshop-provided.h"   // instructor-provided code
#include "simpio.h"
#include "strlib.h"
using namespace std;

bool openImageToProcess(GBufferedImage& img, const string& prompt, bool allowBlankToQuit = false);
void displayImage(GWindow& gw, GBufferedImage& img);
void applyImageFilter(GWindow& gw, GBufferedImage& img);

void applyScatter(GBufferedImage& img);
int getRandomScatterPixel(const Grid<int>& imgGrid, int row, int col, int scatterRadius);
void applyEdgeDetection(GBufferedImage& img);
bool isEdge(const Grid<int>& imgGrid, int row, int col, int threshold);
int colourDifference(int pixel1, int pixel2);
void applyGreenScreen(GBufferedImage& img);
void getStickerLocation(int& row, int& col);
void compareImage(GWindow& gw, GBufferedImage& img);
void maybeSaveImage(GBufferedImage& img);
int getIntegerInRange(const string& prompt, int low, int high = INT_MAX);

int main() {
    cout << "Welcome to Fauxtoshop!" << endl;

    // basic setup of Graphics Window
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setExitOnClose(true);
    gw.setVisible(true);
    GBufferedImage img;

    // uncomment this if you want the same random numbers on each run
    fakeRandomNumberGenerator();

    while (openImageToProcess(
               img, "Enter name of image file to open (or blank to quit): ", true)) {
        displayImage(gw, img);
        applyImageFilter(gw, img);
        cout << endl;
    }

    cout << "Exiting." << endl;
    return 0;
}

/*
 * Repeatedly prompts the user with the given prompt to enter an image filename and tries to open
 * the image in the GUI using the provided buffered image. If the user enters a valid image
 * filename, then the image is opened. If the allowBlankToQuit parameter is set to true and the
 * hits ENTER, no image is opened. Returns true if an image was loaded, false otherwise.
 *
 */
bool openImageToProcess(GBufferedImage& img, const string& prompt, bool allowBlankToQuit) {
    while (true) {
        string filename = getLine(prompt);
        if (allowBlankToQuit && filename == "") {
            return false;
        }
        cout << "Opening image file, may take a minute..." << endl;
        if (openImageFromFilename(img, filename)) {
            return true;
        }
    }
}

/*
 * Displays the given image on the graphics window.
 */
void displayImage(GWindow& gw, GBufferedImage& img) {
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);
}

/*
 * Prompts the user to enter a number from 1 and 4 and applies the corresponding filter to the
 * image. Additionally for the first three filters the user will also have the option to save their
 * new image. If the user does not enter a number from 1 to 4 then they will be reprompted.
 */
void applyImageFilter(GWindow& gw, GBufferedImage& img) {
    while (true) {
        cout << "Which image filter would you like to apply?" << endl;
        cout << "\t1 - Scatter" << endl;
        cout << "\t2 - Edge detection" << endl;
        cout << "\t3 - \"Green screen\" with another image" << endl;
        cout << "\t4 - Compare image with another image" << endl;
        int filter = getInteger("Your choice: ", "Illegal integer format. Try again.");
        switch (filter) {
            case 1:
                return applyScatter(img);
            case 2:
                return applyEdgeDetection(img);
            case 3:
                return applyGreenScreen(img);
            case 4:
                return compareImage(gw, img);
            default:
                break;
        }
    }
}

/*
 * Creates a "scattered" image from the given image and based on a radius provided by the user.
 * For each pixel in the given image, scattering involves randomly selecting a pixel from a nearby
 * row/column and choosing that as the colour of the pixel in the new image at the original
 * position. The user then has the option of saving the resulting image.
 */
void applyScatter(GBufferedImage& img) {
    int scatterRadius = getIntegerInRange("Enter degree of scatter [1 - 100]: ", 1, 100);
    Grid<int> original = img.toGrid();
    Grid<int> result(original.numRows(), original.numCols());

    for (int row = 0; row < original.numRows(); row++) {
        for (int col = 0; col < original.numCols(); col++) {
            result[row][col] = getRandomScatterPixel(original, row, col, scatterRadius);
        }
    }

    img.fromGrid(result);
    maybeSaveImage(img);
}

/*
 * Randomly selects a pixel from the given image grid that is within scatterRadius of the given row
 * and column. E.g. if row and col are (10, 10) and scatterRadius is 5, then the pixel will be
 * chosen randomly between (5, 5) and (15, 15).
 */
int getRandomScatterPixel(const Grid<int>& imgGrid, int row, int col, int scatterRadius) {
    while (true) {
        int randomRow = randomInteger(row - scatterRadius, row + scatterRadius);
        int randomCol = randomInteger(col - scatterRadius, col + scatterRadius);
        if (imgGrid.inBounds(randomRow, randomCol)) {
            return imgGrid[randomRow][randomCol];
        }
    }
}

/*
 * Creates a new black and white image from the given image where each pixel is black if is an edge
 * or white if is not an edge in the original image. The user inputs a threshold, and each pixel
 * will be considered to be an edge if at least one of its neighbors has a difference of greater
 * than this threshold from it. The user then has the option of saving the resulting image.
 */
void applyEdgeDetection(GBufferedImage& img) {
    int threshold = getIntegerInRange("Enter threshold for edge detection: ", 0);
    Grid<int> original = img.toGrid();
    Grid<int> result(original.numRows(), original.numCols());

    for (int row = 0; row < original.numRows(); row++) {
        for (int col = 0; col < original.numCols(); col++) {
            result[row][col] = isEdge(original, row, col, threshold) ? BLACK : WHITE;
        }
    }

    img.fromGrid(result);
    maybeSaveImage(img);
}

/*
 * Returns true if the pixel at the given row and column in the given image is an edge. The pixel
 * is considered to be an edge if at least one of its neighbors has a difference of greater than
 * the given threshold from it.
 */
bool isEdge(const Grid<int>& imgGrid, int row, int col, int threshold) {
    int originalPixel = imgGrid[row][col];
    for (int i = row -1; i <= row + 1; i++) {
        for (int j = col -1; j <= col + 1; j++) {
            if (imgGrid.inBounds(i, j)) {
                int otherPixel = imgGrid[i][j];
                if (colourDifference(originalPixel, otherPixel) > threshold) {
                    return true;
                }
            }
        }
    }
    return false;
}

/*
 * Computes the colour difference between the two given pixels. The colour difference is calculated
 * by taking the absolute difference beween the two red, green and blue values individually, and
 * then returning the maximum of these three differences.
 */
int colourDifference(int pixel1, int pixel2) {
    int red1, green1, blue1;
    GBufferedImage::getRedGreenBlue(pixel1, red1, green1, blue1);

    int red2, green2, blue2;
    GBufferedImage::getRedGreenBlue(pixel2, red2, green2, blue2);

    int redDiff = abs(red1 - red2);
    int greenDiff = abs(green1 - green2);
    int blueDiff = abs(blue1 - blue2);

    return max(max(redDiff, greenDiff), blueDiff);
}

/*
 * Pastes a "sticker" image of the user's choice on top of the given background image, ignoring any
 * part of the sticker image that is close to pure green in colour. The user will also specify the
 * "tolerance" for green that is not pure green, and the row/column pixel position to place the
 * sticker image. The user then has the option of saving the resulting image.
 */
void applyGreenScreen(GBufferedImage& img) {
    cout << "Now choose another file to add to your background image." << endl;
    GBufferedImage stickerImg;
    openImageToProcess(stickerImg, "Enter name of image file to open: ");

    int tolerance = getIntegerInRange("Now choose a tolerance threshold: ", 0, 100);
    int stickerRow, stickerCol;
    getStickerLocation(stickerRow, stickerCol);

    Grid<int> result = img.toGrid();
    Grid<int> stickerGrid = stickerImg.toGrid();
    for (int row = 0; row < stickerGrid.numRows(); row++) {
        for (int col = 0; col < stickerGrid.numCols(); col++) {
            if (result.inBounds(stickerRow + row, stickerCol + col)) {
                int stickerPixel = stickerGrid[row][col];
                if (colourDifference(stickerPixel, GREEN) > tolerance) {
                    result[stickerRow + row][stickerCol + col] = stickerPixel;
                }
            }
        }
    }

    img.fromGrid(result);
    maybeSaveImage(img);
}

/*
 * Gets the location in (row,col) format from the user for where to place the sticker image. If the
 * user hits ENTER, they will be prompted to use the mouse to select the location instead of
 * manually typing it in.
 */
void getStickerLocation(int& row, int& col) {
    while (true) {
        string line =
                getLine("Enter location to place image as \"(row,col)\" (or blank to use mouse): ");
        if (line == "") {
            cout << "Now click the background image to place new image:" << endl;
            getMouseClickLocation(row, col);
            cout << "You chose (" << row << "," << col << ")" << endl;
            break;
        // Enforces the (...,...) string format.
        } else if (startsWith(line, '(') && endsWith(line, ')') && stringContains(line, ',')) {
            int commaIndex = stringIndexOf(line, ',');
            // Gets the two string tokens between the parentheses and the comma.
            string rowString = line.substr(1, commaIndex - 1);
            string colString = line.substr(commaIndex + 1, line.length() - commaIndex - 2);
            if (stringIsInteger(rowString) && stringIsInteger(colString)) {
                row = stringToInteger(rowString);
                col = stringToInteger(colString);
                if (row >= 0 && col >= 0) {
                    break;
                }
            }
        }
    }
}

/*
 * Compares the given image with a new image of the user's choice by computing the number of pixels
 * that differ between the two images. If the number is non-zero then the number is printed out and
 * a new graphical window is opened to display the imgae difference.
 */
void compareImage(GWindow& gw, GBufferedImage& img) {
    cout << "Now choose another image file to compare to" << endl;
    GBufferedImage otherImg;
    openImageToProcess(otherImg, "Enter name of image file to open: ");

    int numDifferentPixels = img.countDiffPixels(otherImg);
    if (numDifferentPixels == 0) {
        cout << "These images are the same!" << endl;
    } else {
        cout << "These images differ in " << numDifferentPixels << " pixel locations!" << endl;
        showDiffWindow(gw, otherImg.getFilename());
    }
}

/*
 * Prompts the user to enter a filename to save the given image to. If the save fails then the user
 * will be reprompted. If the user hits ENTER then saving will be skipped.
 */
void maybeSaveImage(GBufferedImage& img) {
    while (true) {
        string filename = getLine("Enter filename to save image (or blank to skip saving): ");
        if (filename == "" || saveImageToFilename(img, filename)) {
            break;
        }
    }
}

/*
 * Gets an integer from the user between low and high inclusively with the given prompt.
 */
int getIntegerInRange(const string& prompt, int low, int high) {
    while (true) {
        int num = getInteger(prompt);
        if (num >= low && num <= high) {
            return num;
        }
    }
}
