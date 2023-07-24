#include "Framebuffer.h"
#include <fstream>
#include <cstdint>
#include <cstring>
#include <algorithm>

Framebuffer::Framebuffer(int width, int height, const Color &clearColor)
    : m_width(width), m_height(height), m_pixels(width * height, clearColor)
{
}

Color &Framebuffer::getPixel(int x, int y)
{
    return m_pixels[y * m_width + x];
}

const std::vector<Color> Framebuffer::getPixelData() const
{
    return m_pixels;
}

void Framebuffer::point(int x, int y, const Color &currentColor)
{
    if (isValidCoordinate(x, y))
    {
        int index = y * m_width + x;
        m_pixels[index] = currentColor;
    }
}

void Framebuffer::point(const Vertex2 &vertex, const Color &currentColor)
{
    if (isValidCoordinate(vertex.x, vertex.y))
    {
        int index = vertex.y * m_width + vertex.x;
        m_pixels[index] = currentColor;
    }
}

void Framebuffer::clear(const Color &clearColor)
{
    std::fill(m_pixels.begin(), m_pixels.end(), clearColor);
}

void Framebuffer::renderBuffer()
{
    std::ofstream file("out.bmp", std::ios::binary);

    // Write BMP header
    writeBmpHeader(file);

    // Write pixel data
    for (const auto &color : m_pixels)
    {
        // Write color components in BGR order
        file.put(color.getBlue());
        file.put(color.getGreen());
        file.put(color.getRed());
    }

    file.close();
}

void Framebuffer::line(const Vertex2 &p0, const Vertex2 &p1, const Color &currentColor)
{
    int x0 = p0.x;
    int y0 = p0.y;
    int x1 = p1.x;
    int y1 = p1.y;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (x0 != x1 || y0 != y1)
    {
        Vertex2 current{x0, y0};
        point(current, currentColor);

        int err2 = 2 * err;
        if (err2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (err2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Framebuffer::polygon(const std::vector<Vertex2> &vertices, const Color &currentColor)
{
    for (int i = 0; i < vertices.size() - 1; i++)
    {
        line(vertices[i], vertices[i + 1], currentColor);
    }
    line(vertices[vertices.size() - 1], vertices[0], currentColor);
}

void Framebuffer::fillPolygon(const std::vector<Vertex2> &vertices, const Color &fillColor)
{
    // Find the bounding box of the polygon
    int minX = m_width;
    int maxX = 0;
    int minY = m_height;
    int maxY = 0;

    for (const auto &vertex : vertices)
    {
        minX = std::min(minX, vertex.x);
        maxX = std::max(maxX, vertex.x);
        minY = std::min(minY, vertex.y);
        maxY = std::max(maxY, vertex.y);
    }

    // Iterate over each pixel within the bounding box
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Check if the current pixel is inside the polygon
            if (isInsidePolygon(x, y, vertices))
            {
                point({x, y}, fillColor);
            }
        }
    }
}

bool Framebuffer::isInsidePolygon(int x, int y, const std::vector<Vertex2> &vertices)
{
    int numVertices = vertices.size();
    bool inside = false;

    for (int i = 0, j = numVertices - 1; i < numVertices; j = i++)
    {
        if (((vertices[i].y > y) != (vertices[j].y > y)) &&
            (x < (vertices[j].x - vertices[i].x) * (y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x))
        {
            inside = !inside;
        }
    }

    return inside;
}

bool Framebuffer::isValidCoordinate(int x, int y)
{
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

void Framebuffer::writeBmpHeader(std::ofstream &file)
{
    // BMP header structure
#pragma pack(push, 1) // Ensure struct is tightly packed
    struct BitmapHeader
    {
        uint16_t type;
        uint32_t size;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t offset;
        uint32_t headerSize;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitDepth;
        uint32_t compression;
        uint32_t imageSize;
        int32_t xPixelsPerMeter;
        int32_t yPixelsPerMeter;
        uint32_t totalColors;
        uint32_t importantColors;
    };
#pragma pack(pop) // Restore previous packing setting

    BitmapHeader header;
    std::memset(&header, 0, sizeof(BitmapHeader));

    header.type = 0x4D42;                                     // BM
    header.size = sizeof(BitmapHeader) + m_pixels.size() * 3; // 3 bytes per pixel
    header.offset = sizeof(BitmapHeader);
    header.headerSize = sizeof(BitmapHeader) - 14; // subtracting the size of the common fields
    header.width = m_width;
    header.height = m_height;
    header.planes = 1;
    header.bitDepth = 24;
    header.compression = 0;
    header.imageSize = m_width * m_height * 3; // 3 bytes per pixel
    header.xPixelsPerMeter = 2835;             // 72 dpi
    header.yPixelsPerMeter = 2835;             // 72 dpi
    header.totalColors = 0;
    header.importantColors = 0;

    file.write(reinterpret_cast<const char *>(&header), sizeof(BitmapHeader));
}
