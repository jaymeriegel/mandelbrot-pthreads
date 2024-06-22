#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <complex.h>
#include <math.h>
#include <pthread.h>

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 2000
#define NUM_THREADS 32

typedef struct {
    SDL_Renderer *renderer;
    int thread_id;
    int num_threads;
} ThreadData;

volatile int all_threads_done = 0;
pthread_mutex_t mutex;

// Função para mapear coordenadas de tela para o plano complexo
double complex map_to_complex(int x, int y, int width, int height) {
    double real = (x - width / 2.0) * 4.0 / width;
    double imag = (y - height / 2.0) * 4.0 / height;
    return real + imag * I;
}

// Função para calcular o conjunto de Mandelbrot para um ponto complexo c
int mandelbrot(double complex c) {
    double complex z = 0;
    int n = 0;
    while (cabs(z) <= 2 && n < MAX_ITER) {
        z = z*z + c;
        n++;
    }
    return n;
}

// Função para obter a cor com base no número de iterações
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

// Função para desenhar uma parte do fractal de Mandelbrot em um bloco quadrado
void *render_mandelbrot_part(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    SDL_Renderer *renderer = data->renderer;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;

    // Definir o tamanho do bloco (quadrado) e calcular as dimensões
    int block_size = WIDTH / sqrt(NUM_THREADS); // A raiz quadrada do NUM_THREADS dá o número de blocos por linha/coluna
    int blocks_per_row = WIDTH / block_size;

    // Calcular o índice inicial e final do bloco para esta thread
    int start_block = thread_id;
    int end_block = NUM_THREADS;

    // Renderizar cada bloco na região atribuída à thread
    for (int block_index = start_block; block_index < end_block; block_index += num_threads) {
        // Calcular as coordenadas do bloco
        int block_x = (block_index % blocks_per_row) * block_size;
        int block_y = (block_index / blocks_per_row) * block_size;

        // Renderizar cada pixel dentro do bloco
        for (int y = block_y; y < block_y + block_size; y++) {
            for (int x = block_x; x < block_x + block_size; x++) {
                double complex c = map_to_complex(x, y, WIDTH, HEIGHT);
                int n = mandelbrot(c);
                SDL_Color color = get_color(n);
                
                // Bloqueio do mutex para garantir acesso seguro ao renderer
                pthread_mutex_lock(&mutex);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
                pthread_mutex_unlock(&mutex);
            }
        }
    }

    // Indicar que esta thread terminou
    pthread_mutex_lock(&mutex);
    all_threads_done++;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Fractal de Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Unable to create window: %s\n", SDL_GetError());
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

    // Inicialização do mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Failed to initialize mutex\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Inicialização das threads
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].renderer = renderer;
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = NUM_THREADS;
        pthread_create(&threads[i], NULL, render_mandelbrot_part, (void *)&thread_data[i]);
    }

    // Loop principal de eventos
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        if (all_threads_done == NUM_THREADS) {
            quit = 1;
        }

        pthread_mutex_lock(&mutex);
        SDL_RenderPresent(renderer);
        pthread_mutex_unlock(&mutex);

        SDL_Delay(100);  // Pequena pausa para reduzir uso de CPU
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruir mutex
    pthread_mutex_destroy(&mutex);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}