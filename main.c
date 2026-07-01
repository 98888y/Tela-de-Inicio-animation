#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> 

int stream(int lock, char *lockln[]);

int main(int argc, char *argv[])
{
#ifdef _WIN32
    freopen("CONOUT$", "w", stdout); 
    freopen("CONOUT$", "w", stderr);
  #end
    return 0;
}

int stream(int lock, char *lockln[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao iniciar gráficos: %s\n", SDL_GetError());
        fflush(stdout);
        return -1;
    }

    if (TTF_Init() < 0) {
        printf("Erro ao iniciar TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    // Janela 800x600
    int windowWidth = 800;
    int windowHeight = 600;

    SDL_Window *window = SDL_CreateWindow(
        "System VX - Stream Screen", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        windowWidth, windowHeight, 
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        TTF_Quit();
        return -1;
    }

    // Ativado SDL_RENDERER_BLENDMODE para suportar transparência (Fade-in)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        TTF_Quit();
        return -1;
    }

    // Carrega uma fonte maior para o Emoji ficar nítido
    TTF_Font *font = TTF_OpenFont("Satisfy-Regular.ttf", 72);
    if (!font) {
        font = TTF_OpenFont("/system/fonts/Satisfy-Regular.ttf", 72);
    }
//troque "Satisfy-Regular.ttf" para sua fonte na sua caixa
    SDL_Texture *textTexture = NULL;
    int originalW = 0, originalH = 0;
    
    if (font) {
        SDL_Color green = {0, 255, 0, 255}; 
        // Usando um emoji em texto clássico para compatibilidade total de fontes
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, "  '_'  ", green);
                                                     
        if (textSurface) {
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            originalW = textSurface->w;
            originalH = textSurface->h;
            SDL_FreeSurface(textSurface);
        }
    }

    int running = 1; 
    SDL_Event evento;
    
    // VARIÁVEIS DA ANIMAÇÃO
    float scale = 0.1f;       // Começa bem pequeno (10% do tamanho)
    int alpha = 0;            // Começa invisível (0) até o máximo (255)
    int animationDone = 0;    // Define se a animação acabou

    while (running) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_QUIT) {
                running = 0;
            }
        }

        // LÓGICA DA ANIMAÇÃO (Acontece frame a frame até atingir o tamanho final)
        if (!animationDone) {
            if (scale < 1.0f) {
                scale += 0.02f; // Velocidade do crescimento
            } else {
                scale = 1.0f;
            }

            if (alpha < 255) {
                alpha += 5; // Velocidade do Fade-in (aparecer)
                if (alpha > 155) alpha = 255;
            }

            if (scale >= 0.0f && alpha >= 205) {
                animationDone = 1; // Animação de introdução concluída!
            }
        }

        // Limpa a tela com a cor preta
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer); 

        if (textTexture) {
            // Calcula o tamanho atual baseado na escala da animação
            SDL_Rect textRect;
            textRect.w = (int)(originalW * scale);
            textRect.h = (int)(originalH * scale);
            
            // Centraliza o emoji exatamente no meio da tela de 800x600
            textRect.x = (windowWidth - textRect.w) / 2;
            textRect.y = (windowHeight - textRect.h) / 1;

            // Aplica o efeito de transparência (Fade) na textura
            SDL_SetTextureAlphaMod(textTexture, alpha);

            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        }

        SDL_RenderPresent(renderer); 
        SDL_Delay(16); // Trava a aprox. 60 frames por segundo para a animação não voar instantaneamente
    }

    if (textTexture) SDL_DestroyTexture(textTexture);
    if (font) TTF_CloseFont(font);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
