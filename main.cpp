#include "mandelbrot.h"
#include "window.h"

#include <tclap/CmdLine.h>

#include <iostream>

int main(int argc, char *argv[]) {
    bool smooth;
    int iterations;
    try {
        TCLAP::CmdLine cmd("Mandelbrot set image generator", ' ', "0.1");
        TCLAP::SwitchArg smoothArg("s", "smooth", "Use smooth colors", cmd);
        TCLAP::ValueArg<int> iterArg("i", "iter", "Number of iterations [integer]", false, 500, "iterations", cmd);

        cmd.parse(argc, argv);

        smooth = smoothArg.getValue();
        iterations = iterArg.getValue();
    } catch (TCLAP::ArgException &e) {
        std::cout << "Error parsing arguments!" << std::endl;
        return 1;
    }

    std::cout << "smooth: " << smooth << ", iterations: " << iterations << std::endl;

    // Define the size of the image
    window<int> screen(0, 1200, 0, 1200);
    // The domain in which we test for points
    window<double> fract(-2.2, 1.2, -1.7, 1.7);

    mandelbrot(screen, fract, iterations, smooth);

    return 0;
}
