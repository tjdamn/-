#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// ������Ļ��ߺͷ����С
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BLOCK_SIZE 15

// �ߵĽṹ
typedef struct {
    int x, y; // �ߵ�ͷ��λ��
} Point;

typedef struct {
    Point body[100]; // �ߵ����壬���100������
    int length;      // �ߵĳ���
    int dx, dy;      // �ߵ��ƶ�����
} Snake;

// ʳ��Ľṹ
typedef struct {
    Point position; // ʳ��λ��
} Food;

// �ϰ���Ľṹ
#define MAX_OBSTACLES 20
Point obstacles[MAX_OBSTACLES];
int obstacleCount=20;


// ��Ϸ״̬�͵÷�
int running = 1;
int score = 0;

// ȫ�ֱ���
Snake snake;
Food food;
SDL_Renderer* renderer;
Mix_Music* backgroundMusic;
Mix_Chunk* eatSound;
SDL_Window* window;

// �ж�ʳ������λ���Ƿ����ߵ�������ϰ����ص�
int isValidFoodPosition(int x, int y) {
    // �ж��Ƿ����ߵ������ص�
    for (int i = 0; i < snake.length; i++) {
        if (snake.body[i].x == x && snake.body[i].y == y) {
            return 0; // �ص���������Ч
        }
    }

    // �ж��Ƿ����ϰ����ص�
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].x == x && obstacles[i].y == y) {
            return 0; // �ص���������Ч
        }
    }

    return 1; // û���ص���������Ч
}

// �ж�ʳ������λ���Ƿ�����ͷ��Χ��5����λ��
int isNearSnakeHead(int x, int y) {
    int headX = snake.body[0].x;
    int headY = snake.body[0].y;
    int radius = 5; // �뾶5����λ

    // �ж����ɵ��Ƿ�����ͷ��Χ��5����λ��
    return (abs(x - headX) <= radius && abs(y - headY) <= radius);
}

// ���������Ч��ʳ��λ�ã��ܿ��ߵ����塢�ϰ������ͷ��Χ5����λ������
void generateFood() {
    int x, y;
    do {
        x = rand() % (SCREEN_WIDTH / BLOCK_SIZE);
        y = rand() % (SCREEN_HEIGHT / BLOCK_SIZE);
    } while (!isValidFoodPosition(x, y) || isNearSnakeHead(x, y)); // ֻҪ��������������������
    food.position.x = x;
    food.position.y = y;
}

// ��������ϰ����λ�ã��ܿ��ߵ����塢ʳ�����ͷ��Χ5����λ������
void generateObstacles() {
    for (int i = 0; i < obstacleCount; i++) {
        int x, y;
        do {
            x = rand() % (SCREEN_WIDTH / BLOCK_SIZE);
            y = rand() % (SCREEN_HEIGHT / BLOCK_SIZE);
        } while (!isValidFoodPosition(x, y) || isNearSnakeHead(x, y)); // �ܿ��ߵ����塢ʳ�����ͷ��Χ15����λ������
        obstacles[i].x = x;
        obstacles[i].y = y;
    }
}

// ��ʼ����Ϸ
void initGame() {
    // ��ʼ���ߵ�����
    snake.length = 2;
    snake.body[0].x = 10;  // ��ͷ��ʼλ��
    snake.body[0].y = 10;
    snake.body[1].x = 9;
    snake.body[1].y = 10;
    snake.body[2].x = 8;
    snake.body[2].y = 10;
    snake.dx = 1;  // ��ʼ����
    snake.dy = 0;

    // �������ʳ��
    srand((unsigned int)time(NULL));
    generateFood();

    // ��������ϰ���
    generateObstacles();

    score = 0;  // ��ʼ���÷�
}

// ���ƾ��Σ����ڻ����ߡ�ʳ����ϰ��
void drawRectangle(int x, int y, int size, SDL_Color color) {
    SDL_Rect rect = { x, y, size, size };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// ������
void drawSnake() {
    SDL_Color snakeColor = { 0, 255, 0, 255 }; // �ߵ���ɫ����ɫ��
    for (int i = 0; i < snake.length; i++) {
        drawRectangle(snake.body[i].x * BLOCK_SIZE, snake.body[i].y * BLOCK_SIZE, BLOCK_SIZE, snakeColor);
    }
}

// ����ʳ��
void drawFood() {
    SDL_Color foodColor = { 255, 0, 0, 255 }; // ʳ�����ɫ����ɫ��
    drawRectangle(food.position.x * BLOCK_SIZE, food.position.y * BLOCK_SIZE, BLOCK_SIZE, foodColor);
}

// �����ϰ���
void drawObstacles() {
    SDL_Color obstacleColor = { 128, 128, 128, 255 }; // �ϰ������ɫ����ɫ��
    for (int i = 0; i < obstacleCount; i++) {
        drawRectangle(obstacles[i].x * BLOCK_SIZE, obstacles[i].y * BLOCK_SIZE, BLOCK_SIZE, obstacleColor);
    }
}

// �����ߵ�λ��
void update() {
    // �����ߵ�����
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0].x += snake.dx;
    snake.body[0].y += snake.dy;

    // ����Ƿ�Ե�ʳ��
    if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y) {
        snake.length++;
        score += 10;
        generateFood(); // �����µ�ʳ��

        // ���ų�ʳ����Ч
        Mix_PlayChannel(-1, eatSound, 0);
    }

    // ����Ƿ�ײǽ
    if (snake.body[0].x < 0 || snake.body[0].x >= SCREEN_WIDTH / BLOCK_SIZE ||
        snake.body[0].y < 0 || snake.body[0].y >= SCREEN_HEIGHT / BLOCK_SIZE) {
        running = 0;
    }

    // ����Ƿ�ײ���Լ�
    for (int i = 1; i < snake.length; i++) {
        if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
            running = 0;
        }
    }

    // ����Ƿ�ײ���ϰ���
    for (int i = 0; i < obstacleCount; i++) {
        if (snake.body[0].x == obstacles[i].x && snake.body[0].y == obstacles[i].y) {
            running = 0;
        }
    }
}

// �����û�����
void handleInput(SDL_Event* event) {
    if (event->type == SDL_QUIT) {
        running = 0;
    }
    else if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
        case SDLK_UP:
            if (snake.dy == 0) {
                snake.dx = 0;
                snake.dy = -1;
            }
            break;
        case SDLK_DOWN:
            if (snake.dy == 0) {
                snake.dx = 0;
                snake.dy = 1;
            }
            break;
        case SDLK_LEFT:
            if (snake.dx == 0) {
                snake.dx = -1;
                snake.dy = 0;
            }
            break;
        case SDLK_RIGHT:
            if (snake.dx == 0) {
                snake.dx = 1;
                snake.dy = 0;
            }
            break;
        case SDLK_ESCAPE:
            running = 0; // �� ESC ���˳�
            break;
        }
    }
}

// ��ʼ�� SDL�������������Ч
void initSDL() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); // ��ʼ�� SDL ��Ƶ����Ƶ��ϵͳ
    IMG_Init(IMG_INIT_PNG); // ��ʼ�� SDL_image
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // ��ʼ����Ƶϵͳ

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // ������Ⱦ��

    // ���ر������ֺͳ�ʳ����Ч
    backgroundMusic = Mix_LoadMUS("background.mp3");
    eatSound = Mix_LoadWAV("eat.wav");

    // ���ű�������
    Mix_PlayMusic(backgroundMusic, -1);
}

// ������Դ
void cleanUp() {
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(eatSound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    Mix_Quit();
}

// ��Ϸ��ѭ��
int main(int argc, char* argv[]) {
    initSDL();   // ��ʼ�� SDL
    initGame();  // ��ʼ����Ϸ

    SDL_Event event;
    while (running) {
        // ���������¼�
        while (SDL_PollEvent(&event)) {
            handleInput(&event);
        }

        // ������Ϸ״̬
        update();

        // ����
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // ���ñ���ɫΪ��ɫ
        SDL_RenderClear(renderer);

        // ������ϷԪ��
        drawSnake();
        drawFood();
        drawObstacles();

        // ������Ļ
        SDL_RenderPresent(renderer);

        // ������Ϸ֡��
        SDL_Delay(100);
    }

    // ��ӡ�÷ֲ�������Դ
    printf("Game Over! Your score: %d\n", score);
    cleanUp();
    return 0;
}

