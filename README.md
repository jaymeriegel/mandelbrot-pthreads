# Renderizador de Fractal Mandelbrot

Este projeto é um renderizador de fractal Mandelbrot multithreaded escrito em C usando a biblioteca SDL2.

## Recursos

- Renderiza o conjunto de Mandelbrot como uma imagem fractal.
- Usa várias threads para acelerar o processo de renderização.
- Mapeia coordenadas de tela para o plano complexo para geração de fractal.
- Usa SDL2 para renderização gráfica.

## Estrutura do Código

Os principais componentes do código são:

- `map_to_complex`: Esta função mapeia coordenadas de tela para o plano complexo.
- `mandelbrot`: Esta função calcula o conjunto de Mandelbrot para um determinado número complexo.
- `get_color`: Esta função determina a cor de um pixel com base no número de iterações.
- `render_mandelbrot_part`: Esta função é responsável por renderizar uma parte do fractal de Mandelbrot.
- `main`: Esta função inicializa o SDL, cria threads para renderização e lida com o loop de eventos principal.

## Construção e Execução

Este projeto requer a biblioteca SDL2. Se você estiver usando um gerenciador de pacotes como `apt` no Ubuntu, você pode instalar o SDL2 com o seguinte comando:

```bash
sudo apt-get install libsdl2-dev
```

Após instalar o SDL2, você pode compilar o projeto usando um compilador C. Por exemplo, usando gcc:

```bash
gcc mandelbrot.c -o mandelbrot -lSDL2 -lm
```

Então, você pode executar o programa com:

```bash
./mandelbrot
```

Autores:
```
Jayme Riegel e Chrystian Rocha
```
