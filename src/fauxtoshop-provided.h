/*
 * CS 106B/X Fauxtoshop
 * This instructor-provided file declares functions to help you open
 * and save images.
 * See fauxtoshop-provided.cpp for the function definitions.
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

#ifndef _FAUXTOSHOP_PROVIDED_H
#define _FAUXTOSHOP_PROVIDED_H

#include <string>
#include "gbufferedimage.h"
#include "vector.h"

/* constants for predefined colors */
extern const int WHITE;
extern const int BLACK;
extern const int GREEN;

/*
 * "Rigs" the random number generator used by randomInteger() and similar
 * functions so that the same sequence of integers will be returned.
 * This is useful for testing the scatter function to get the same outputs
 * that we got for our test outputs.
 */
void fakeRandomNumberGenerator();

/*
 * Takes a radius and computes a 1-dimensional Gaussian blur kernel
 * with that radius. The 1-dimensional kernel can be applied to a
 * 2-dimensional image in two separate passes: first pass goes over
 * each row and does the horizontal convolutions, second pass goes
 * over each column and does the vertical convolutions. This is more
 * efficient than creating a 2-dimensional kernel and applying it in
 * one convolution pass.
 *
 * This code is based on the C# code posted by Stack Overflow user
 * "Cecil has a name" at this link:
 * http://stackoverflow.com/questions/1696113/how-do-i-gaussian-blur-an-image-without-using-any-in-built-gaussian-functions
 *
 * This is only here in in case you decide to impelment a Gaussian
 * blur as an OPTIONAL extension (see the suggested extensions part
 * of the spec handout).
 */
Vector<double> gaussKernelForRadius(int radius);

/*
 * Waits for the user to click the mouse on the current graphical window,
 * and stores the x/y coordinates clicked into the given row/col variables.
 * Note that the 'row' is the y-coordinate and the 'col' is the x-coordinate.
 */
void getMouseClickLocation(int& row, int& col);

/*
 * Attempts to load an image from the given file name and places its pixel data
 * into the given buffered image by reference.
 * Returns true if the load is successful; otherwise returns false.
 */
bool openImageFromFilename(GBufferedImage& img, std::string filename);

/*
 * Attempts to save the pixel data from the given buffered image
 * into the given file name.
 * Returns true if the save is successful; otherwise returns false.
 */
bool saveImageToFilename(GBufferedImage& img, std::string filename);

/*
 * Shows a graphical window displaying pixel differences between
 * the window contents and the contents of the given image file.
 * The differences are highlighted in magenta.
 */
void showDiffWindow(const GWindow& gw, std::string filename);

#endif // _FAUXTOSHOP_PROVIDED_H
