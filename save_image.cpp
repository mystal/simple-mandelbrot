#include "save_image.h"

#include <FreeImage.h>

#include <tuple>
#include <vector>

using namespace std;

tuple<int, int, int> getRgbPiecewiseLinear(int n, int iterMax) {
    int N = 256; // colors per element
    int N3 = N*N*N;
    // map on the 0..1 interval (real numbers)
    double t = (double)n/iterMax;
    // expand n on the 0..256^3 interal (integers)
    n = (int)(t*(double)N3);

    int b = n/(N*N);
    int nn = n - b*N*N;
    int r = nn/N;
    int g = nn - r*N;

    return tuple<int, int, int>(r, g, b);
}

tuple<int, int, int> getRgbSmooth(int n, int iterMax) {
    // map n on the 0..1 interval
    double t = (double)n/iterMax;

    // use smooth polynomials for r, g, b
    int r = (int)(9*(1 - t)*t*t*t*255);
    int g = (int)(15*(1 - t)*(1 - t)*t*t*255);
    int b = (int)(8.5*(1 - t)*(1 - t)*(1 - t)*t*255);

    return tuple<int, int, int>(r, g, b);
}

void plot(window<int> &screen, vector<int> &colors, int iterMax,
        const char *fname, bool smoothColor) {
    // active only for static linking
#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif

    unsigned int width = screen.width(), height = screen.height();
    FIBITMAP *bitmap = FreeImage_Allocate(width, height, 32); // RGBA

    int k = 0;
    tuple<int, int, int> rgb;
    for (int j = screen.yMin(); j < screen.yMax(); ++j) {
        for (int i = screen.xMin(); i < screen.xMax(); ++i) {
            int n = colors[k];

            if (!smoothColor) {
                rgb = getRgbPiecewiseLinear(n, iterMax);
            }
            else {
                rgb = getRgbSmooth(n, iterMax);
            }

            RGBQUAD col;
            col.rgbRed = get<0>(rgb);
            col.rgbGreen = get<1>(rgb);
            col.rgbBlue = get<2>(rgb);
            col.rgbReserved = 255;

            FreeImage_SetPixelColor(bitmap, i, j, &col);
            k++;
        }
    }

    // Save the image in a PNG file
    FreeImage_Save(FIF_PNG, bitmap, fname, PNG_DEFAULT);

    // Clean bitmap;
    FreeImage_Unload(bitmap);

    // active only for static linking
#ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
#endif
}
