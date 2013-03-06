#include "save_image.h"
#include "window.h"

#include <chrono>
#include <complex>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

typedef function<complex<double>(complex<double>, complex<double>)> iterFunc;

// Check if a point is in the set or escapes to infinity, return the number of iterations
int escape(complex<double> c, int iterMax, const iterFunc &func) {
    complex<double> z(0);
    int iter = 0;

    while (abs(z) < 2.0 && iter < iterMax) {
        z = func(z, c);
        iter++;
    }

    return iter;
}

// Convert a pixel coordinate to the complex domain
complex<double> scale(window<int> &screen, window<double> &fract, complex<double> c) {
    complex<double> aux(c.real()/(double)screen.width()*fract.width() + fract.xMin(),
            c.imag()/(double)screen.height()*fract.height() + fract.yMin());
    return aux;
}

// Loop over each pixel from our image and check if the point associated with
// this pixel escapes to infinity
void getNumberIterations(window<int> &screen, window<double> &fract, int iterMax,
        vector<int> &colors, const iterFunc &func) {
    int k = 0;
    for (int j = screen.yMin(); j < screen.yMax(); ++j) {
        for (int i = screen.xMin(); i < screen.xMax(); ++i) {
            complex<double> c((double) i, (double) j);
            c = scale(screen, fract, c);
            colors[k] = escape(c, iterMax, func);
            k++;
        }
    }
}

void fractal(window<int> &screen, window<double> &fract, int iterMax, vector<int> &colors,
        const iterFunc &func, const char *fname, bool smoothColor) {
    auto start = chrono::steady_clock::now();
    getNumberIterations(screen, fract, iterMax, colors, func);
    auto end = chrono::steady_clock::now();
    auto ellapsedTime = chrono::duration<double, milli>(end - start).count();

    cout << "Time to generate " << fname << " = " << ellapsedTime << " ms" << endl;

    // Save and show the resulting image
    plot(screen, colors, iterMax, fname, smoothColor);
}

void mandelbrot(window<int> &screen, window<double> &fract, int iterMax, bool smoothColor, const char *fname) {
    // The function used to calculate the fractal
    auto func = [] (complex<double>z, complex<double> c) -> complex<double> {
        return (z*z) + c;
    };

    vector<int> colors(screen.size());

    // Loop over all pixels in screen and check membership in the Mandelbrot Set
    fractal(screen, fract, iterMax, colors, func, fname, smoothColor);
}
