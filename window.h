#pragma once

template <typename T>
class window {
    T _xMin, _xMax, _yMin, _yMax;

public:
    window(T xMin, T xMax, T yMin, T yMax)
    : _xMin(xMin), _xMax(xMax), _yMin(yMin), _yMax(yMax) { }

    // Utility functions for getting the size, width, and height of the window
    T size() {
        return width()*height();
    }

    T width() {
        return _xMax - _xMin;
    }

    T height() {
        return _yMax - _yMin;
    }

    // Getters and setters for the window elements
    T xMin() {return _xMin;}
    T xMax() {return _xMax;}
    T yMin() {return _yMin;}
    T yMax() {return _yMax;}

    void xMin(T newXMin) {_xMin = newXMin;}
    void xMax(T newXMax) {_xMax = newXMax;}
    void yMin(T newYMin) {_yMin = newYMin;}
    void yMax(T newYMax) {_yMax = newYMax;}
    
    // Reset all values
    void reset(T xMin, T xMax, T yMin, T yMax) {
        _xMin = xMin;
        _xMax = xMax;
        _yMin = yMin;
        _yMax = yMax;
    }
};
