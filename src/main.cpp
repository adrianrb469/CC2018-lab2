#include <SDL2/SDL.h>
#include "Framebuffer.h"
#include "Color.h"
#include "Vertex2.h"
#include <iostream>
#include <random>

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;
int FRAMEBUFFER_WIDTH = 200;
int FRAMEBUFFER_HEIGHT = 200;

Framebuffer framebuffer(FRAMEBUFFER_HEIGHT, FRAMEBUFFER_HEIGHT, Color(0, 10, 10, 255));

// Function to count the live neighbors of a cell at position (x, y)
int countLiveNeighbors(const std::vector<Color> &pixels, int x, int y)
{
    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (!(i == 0 && j == 0))
            {
                int neighborX = x + i;
                int neighborY = y + j;

                if (neighborX >= 0 && neighborX < FRAMEBUFFER_WIDTH &&
                    neighborY >= 0 && neighborY < FRAMEBUFFER_HEIGHT &&
                    pixels.at(neighborY * FRAMEBUFFER_WIDTH + neighborX).getRed() == 255)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

void renderBuffer(SDL_Renderer *renderer)
{
    // Create a texture
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        FRAMEBUFFER_WIDTH,
        FRAMEBUFFER_HEIGHT);

    // Update the texture with the pixel data from the framebuffer
    SDL_UpdateTexture(
        texture,
        NULL,
        framebuffer.getPixelData().data(),
        FRAMEBUFFER_WIDTH * sizeof(Color));

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Destroy the texture
    SDL_DestroyTexture(texture);
}

void render(SDL_Renderer *renderer)
{
    // Set up the glider pattern at a more visible position

    const std::vector<Color> pixels = framebuffer.getPixelData();

    // Create a copy of the current framebuffer to hold the updated cell states
    Framebuffer updatedFramebuffer = framebuffer;

    // Loop through each pixel of the framebuffer and apply Conway's Game of Life rules
    for (int x = 0; x < FRAMEBUFFER_WIDTH; x++)
    {
        for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++)
        {
            if (pixels.at(y * FRAMEBUFFER_WIDTH + x).getRed() == 255)
            {
                // Alive cell
                int neighbors = countLiveNeighbors(pixels, x, y);

                if (neighbors < 2 || neighbors > 3)
                {
                    // Cell dies due to underpopulation or overpopulation
                    updatedFramebuffer.point(x, y, Color(0, 0, 0, 255));
                }
                else
                {
                    // Cell survives to the next generation
                    updatedFramebuffer.point(x, y, Color(255, 0, 0, 255));
                }
            }
            else
            {
                // Dead cell
                int neighbors = countLiveNeighbors(pixels, x, y);

                if (neighbors == 3)
                {
                    // Dead cell becomes alive due to reproduction
                    updatedFramebuffer.point(x, y, Color(255, 0, 0, 255));
                }
                else
                {
                    // Cell remains dead
                    updatedFramebuffer.point(x, y, Color(0, 0, 0, 255));
                }
            }
        }
    }

    // Update the original framebuffer with the updated cell states
    framebuffer = updatedFramebuffer;

    // Render the framebuffer to the renderer
    renderBuffer(renderer);
}

int main()
{

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("life", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event event;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    for (int y = 0; y < FRAMEBUFFER_HEIGHT; ++y)
    {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x)
        {
            if (dist(gen))
            {
                framebuffer.point(x, y, Color(255, 0, 0, 255));
            }
        }
    }

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Call our render function

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
        SDL_RenderClear(renderer);
        render(renderer);
        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 35);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}