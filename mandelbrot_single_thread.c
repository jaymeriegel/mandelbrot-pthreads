#include <SDL2/SDL.h>
#include <complex.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

double complex map_to_complex(int x, int y, int width, int height) {
    double real = (x - width / 2.0) * 4.0 / width;
    double imag = (y - height / 2.0) * 4.0 / height;
    return real + imag * I;
}

int mandelbrot(double complex c) {
    double complex z = 0;
    int n = 0;
    while (cabs(z) <= 2 && n < MAX_ITER) {
        z = z*z + c;
        n++;
    }
    return n;
}

SDL_Color get_color(int n) {
    SDL_Color color;
    if (n == MAX_ITER) {
        color.r = color.g = color.b = 0;  // Preto
    } else {
        color.r = n % 256;
        color.g = (n * 5) % 256;
        color.b = (n * 10) % 256;
    }
    return color;
}


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Fractal de Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Ubable to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Unable to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                //point in the complex plain ex(1,4)
                double complex c = map_to_complex(x, y, WIDTH, HEIGHT);
                int n = mandelbrot(c);
                SDL_Color color = get_color(n);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(100);  // Pequena pausa para reduzir uso de CPU
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}