#pragma once

#include <vector>
#include "Color.h"
#include "Vertex2.h"
#include <fstream> // Include the <fstream> header

class Framebuffer
{
public:
    Framebuffer(int width, int height, const Color &clearColor);
    const std::vector<Color> getPixelData() const;
    Color &getPixel(int x, int y);

    void point(int x, int y, const Color &currentColor);
    void point(const Vertex2 &vertex, const Color &currentColor);
    void clear(const Color &clearColor);
    void renderBuffer();
    void line(const Vertex2 &p0, const Vertex2 &p1, const Color &currentColor);
    void polygon(const std::vector<Vertex2> &vertices, const Color &currentColor);
    void fillPolygon(const std::vector<Vertex2> &vertices, const Color &currentColor);

private:
    int m_width;
    int m_height;
    std::vector<Color> m_pixels;

    bool isValidCoordinate(int x, int y);
    bool isInsidePolygon(int x, int y, const std::vector<Vertex2> &vertices);
    void writeBmpHeader(std::ofstream &file);
};
