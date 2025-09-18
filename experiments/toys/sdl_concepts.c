// gcc sdl_concepts.c -o concepts -lSDL2 -lSDL2_ttf -lm
// by ChatGpt, modyfied by chaosregular(x)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 1500
#define SCREEN_H 900
#define N_CONCEPTS 12
#define MAX_CONCEPTS 500

// chaosregular(x)2025-09-17_05:50:00:
// flip-forces
#define MASS_0 1.2
#define G_CONSTANT_ATTRACTIVE 5.0
#define G_CONSTANT_REPULSIVE 3.0

// central attraction
#define G_CONSTANT_CENTER 0.0005
#define G_CONSTANT_ANCHOR 0.00045
// energy dumping
#define DUMP_FACTOR 0.9


int conceptCount = N_CONCEPTS;

typedef struct {
    float x, y;
    float vx, vy;
    float mass;
    float inertia;  // chaosregular(x)2025-09-17_07:48:34 speed dump factor for given concept
    int valence;     // +/- attraction property
    uint8_t r,g,b;
    const char *text;
} Concept;

Concept concepts[MAX_CONCEPTS] = {
    {100, 400, 0, 0, 2.6 * MASS_0, 0.22, +1, 10,167,10, "Do No Harm"},
    {600, 450, 0, 0, 2.8 * MASS_0, 0.18, +1, 10,157,20, "Ethics"},
    {500, 150, 0, 0, 2.8 * MASS_0, 0.18, +1, 10,147,30, "Trust"},    
    {700, 300, 0, 0, 2.7 * MASS_0, 0.24, +1, 10,137,40, "Eagle"},
    {500, 100, 0, 0, 2.8 * MASS_0, 0.21, +1, 10,127,50, "Resistance"},
    {300, 500, 0, 0, 2.2 * MASS_0, 0.4, +1, 10,107,10, "Coupled Grid"},
    {410, 200, 0, 0, 3.1 * MASS_0, 0.7, -1, 55,10,10, "EGO"},
    {420, 200, 0, 0, 3.3 * MASS_0, 0.8, -1, 155,10,10, "Greed"},
    {430, 200, 0, 0, 3.2 * MASS_0, 0.99, -1, 255,10,10, "Stupidity"},
    {200, 150, 0, 0, 2.3 * MASS_0, 0.7, +1, 10,97,10, "Chaos Bomb"},
    {400, 300, 0, 0, 2.4 * MASS_0, 0.4, +1, 10,87,20, "Fractal ID"},
    {400, 200, 0, 0, 2.1 * MASS_0, 0.3, +1, 10,77,30, "Tank-in-Acid"}
};

float randf(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

int perspective = -1;  // -1 = neutral, otherwise index of concept

void update() {
    for (int i=0; i<conceptCount ; i++) { // was N_CONCEPTS
        Concept *a = &concepts[i];

        if (perspective == -1) {
            // Neutral: pull to screen center
            float dx = SCREEN_W/2 - a->x;
            float dy = SCREEN_H/2 - a->y;
            a->vx += G_CONSTANT_CENTER * dx;
            a->vy += G_CONSTANT_CENTER * dy;
        } else {
            // Perspective: pull others toward perspective anchor
            Concept *p = &concepts[perspective];
            if (i != perspective) {
                // selection as mass modification
                float dx = p->x - a->x;
                float dy = p->y - a->y;
                a->vx += G_CONSTANT_ANCHOR * dx;
                a->vy += G_CONSTANT_ANCHOR * dy;
            } // chaosregular(x)2025-09-17_07:45:36 added else option
            else
            {
                float dx = SCREEN_W/2 - a->x;
                float dy = SCREEN_H/2 - a->y;
                a->vx += G_CONSTANT_CENTER * dx;
                a->vy += G_CONSTANT_CENTER * dy;                
            }
        }

        // Inter-concept forces
        for (int j=0; j<conceptCount; j++) {// was N_CONCEPTS
            if (i == j) continue;
            Concept *b = &concepts[j];
            float dx = b->x - a->x;
            float dy = b->y - a->y;
            float dist2 = dx*dx + dy*dy + 0.01f;
            float dist = sqrt(dist2);
            float dist3 = dist*dist2;
            float force = (a->valence * b->valence > 0 ? -1.0f : +1.0f);

            // chaosregular(x)2025-09-17_05:50:43 flip forces
            force *= G_CONSTANT_ATTRACTIVE / dist2 - G_CONSTANT_REPULSIVE / dist3;
            force *= b->mass * a->mass;
            // a->vx += force * dx;
            // a->vy += force * dy;
            // chaosregular(x)2025-09-16_05:00:01 added energy dumping
            //a->vx += (force * dx)*DUMP_FACTOR;
            //a->vy += (force * dy)*DUMP_FACTOR;
            // chaosregular(x)2025-09-17_07:56:30 concept specyfic dumping
            a->vx += (force * dx)*a->inertia;
            a->vy += (force * dy)*a->inertia;    

        }

        a->x += a->vx;
        a->y += a->vy;

        a->vx *= DUMP_FACTOR;
        a->vy *= DUMP_FACTOR;
    }
}

void drawConcept(SDL_Renderer *ren, TTF_Font *font, Concept *c, int isAnchor) {
    SDL_SetRenderDrawColor(ren, isAnchor ? 128 : c->r, isAnchor ? 128 : c->g, isAnchor ? 128 : c->b, 255);
    int r = isAnchor ? 20 : 15;
    for (int w = -r; w <= r; w++) {
        for (int h = -r; h <= r; h++) {
            if (w*w + h*h <= r*r) {
                SDL_RenderDrawPoint(ren, (int)c->x+w, (int)c->y+h);
            }
        }
    }

    SDL_Color color = {188, 188, 188, 255};
    SDL_Surface *surf = TTF_RenderText_Solid(font, c->text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect dst = { (int)c->x+20, (int)c->y-10, surf->w, surf->h };
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *win = SDL_CreateWindow("Concept Lights (Perspective Mode)",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       SCREEN_W, SCREEN_H, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("../DejaVuSans.ttf", 14);
    if (!font) { printf("Font load error\n"); return 1; }

    srand(time(NULL));
    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int i = rand() % N_CONCEPTS;
                concepts[i].vx += randf(-3,3);
                concepts[i].vy += randf(-3,3);
            }
                if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_0) perspective = -1;
                if (e.key.keysym.sym == SDLK_SPACE && perspective >= 0 && conceptCount < MAX_CONCEPTS) {
                    // Spawn new related concept
                    Concept *p = &concepts[perspective];
                    Concept *child = &concepts[conceptCount++];
                    child->x = p->x + randf(-30,30);
                    child->y = p->y + randf(-30,30);
                    child->r = p->r;
                    child->g = p->g;
                    child->b = p->b;
                    child->vx = randf(-1,1);
                    child->vy = randf(-1,1);
                    child->mass = p->mass * (0.8f + randf(-0.2,0.2));
                    child->inertia = p->inertia * (0.8f + randf(-0.2,0.2)); // chaosregular(x)2025-09-17_07:52:46
                    child->valence = p->valence;  // inherit
                    //child->text = "Child";  // placeholder (later: dynamically assign label)
                    child->text = p->text;  // placeholder (later: dynamically assign label)
                }            

                if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_9) {
                    perspective = e.key.keysym.sym - SDLK_1;
                }                
            }
        }

        update();

        // Background: tied to anchor's valence
        int r=0,g=0,b=0;
        if (perspective >= 0) {
            if (concepts[perspective].valence > 0) { r=10; g=10; b=10; }
            else { r=50; g=50; b=50; }
        }
        SDL_SetRenderDrawColor(ren, r,g,b,255);
        SDL_RenderClear(ren);

        for (int i=0; i<conceptCount; i++) { // was N_CONCEPTS
            drawConcept(ren, font, &concepts[i], (i==perspective));
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
