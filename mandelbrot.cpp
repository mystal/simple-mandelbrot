#include "save_image.h"
#include "window.h"

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

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
    for (int j = screen.yMin(); j < screen.yMax(); ++j) {
        for (int i = screen.xMin(); i < screen.xMax(); ++i) {
            complex<double> c((double) i, (double) j);
            c = scale(screen, fract, c);
            colors.push_back(escape(c, iterMax, func));
        }
    }
}

// Loop over each pixel from our image and check if the point associated with
// this pixel escapes to infinity
void getNumberIterationsOpenCl(window<int> &screen, window<double> &fract, int iterMax,
        vector<int> &colors, vector<cl::Device> &devices) {
    cl::Context context(devices);
    //TODO: check for errors creating the context

    cl::Buffer devColors(context, CL_MEM_READ_ONLY, screen.size()*sizeof(int));
    for (int j = screen.yMin(); j < screen.yMax(); ++j) {
        for (int i = screen.xMin(); i < screen.xMax(); ++i) {
            complex<double> c((double) i, (double) j);
            c = scale(screen, fract, c);
            colors.push_back(escape(c, iterMax, func));
        }
    }
}

void fractal(window<int> &screen, window<double> &fract, int iterMax,
        const iterFunc &func, const char *fname, bool smoothColor, bool opencl) {
    vector<cl::Device> devices;
    if (opencl) {
        vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            cout << "No OpenCL platforms available! Exiting..." << endl;
            return;
        }
        cout << "Available platforms:" << endl;
        for (int i = 0; i < platforms.size(); ++i) {
            cl::Platform platform = platforms[i];
            string s;
            platform.getInfo(CL_PLATFORM_NAME, &s);
            cout << i << ": " << s << endl;
        }
        int choice = -1;
        cout << "Select a platform: ";
        cin >> choice;
        cl::Platform platform = platforms[choice];

        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        cout << "Available devices:" << endl;
        for (int i = 0; i < devices.size(); ++i) {
            cl::Device device = devices[i];
            string s;
            device.getInfo(CL_DEVICE_NAME, &s);
            cout << i << ": " << s << endl;
        }
        choice = -1;
        cout << "Select a device: ";
        cin >> choice;
        cl::Device device = devices[choice];
        devices.clear();
        devices.push_back(device);
    }

    vector<int> colors(screen.size());

    auto start = chrono::steady_clock::now();
    if (opencl) {
        getNumberIterationsOpenCl(screen, fract, iterMax, colors, device);
    } else {
        getNumberIterations(screen, fract, iterMax, colors, func);
    }
    auto end = chrono::steady_clock::now();
    auto ellapsedTime = chrono::duration<double, milli>(end - start).count();

    cout << "Time to generate " << fname << " = " << ellapsedTime << " ms" << endl;

    // Save and show the resulting image
    plot(screen, colors, iterMax, fname, smoothColor);
}

void mandelbrot(window<int> &screen, window<double> &fract, int iterMax, bool smoothColor, const char *fname, bool opencl) {
    // The function used to calculate the fractal
    auto func = [] (complex<double>z, complex<double> c) -> complex<double> {
        return (z*z) + c;
    };

    // Loop over all pixels in screen and check membership in the Mandelbrot Set
    fractal(screen, fract, iterMax, func, fname, smoothColor, opencl);
}
