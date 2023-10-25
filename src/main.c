#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LEVEL_SIZE 30
#define MAX_SIZE 900

#define UP 65
#define DOWN 66
#define LEFT 68
#define RIGHT 67

#define FRAMETIME 0.016
#define TICKRATE 1000

// position-related
typedef struct position
{
    int x, y;
} POSITION;

POSITION position_get_random(int min, int max)
{
    int rx = min + (rand() % (max - min + 1));
    int ry = min + (rand() % (max - min + 1));

    POSITION position = {rx, ry};

    return position;
}

int position_equal(POSITION p1, POSITION p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

//

// snake-related
typedef struct body
{
    POSITION position;
    int filled;
} BODY;

typedef struct snake
{
    int size;
    int dir;
    BODY body[MAX_SIZE];
} SNAKE;

SNAKE *snake_new(int x, int y)
{
    SNAKE *s = malloc(sizeof(SNAKE));
    s->size = 1;
    s->body[0].position.x = x;
    s->body[0].position.y = y;
    s->dir = UP;
    return s;
}

POSITION snake_get_next_position(SNAKE *snake)
{
    POSITION position = {0, 0};

    switch (snake->dir)
    {
    case UP:
    {
        position.x = snake->body[0].position.x;
        position.y = snake->body[0].position.y - 1;
        break;
    }
    case DOWN:
    {
        position.x = snake->body[0].position.x;
        position.y = snake->body[0].position.y + 1;
        break;
    }
    case LEFT:
    {
        position.x = snake->body[0].position.x - 1;
        position.y = snake->body[0].position.y;
        break;
    }
    case RIGHT:
    {
        position.x = snake->body[0].position.x + 1;
        position.y = snake->body[0].position.y;
        break;
    }
    }

    return position;
}

void snake_step(SNAKE *snake, POSITION next, POSITION *food)
{
    int i;
    POSITION last_pos = snake->body[snake->size - 1].position;
    for (i = snake->size - 1; i >= 1; i--)
    {
        snake->body[i].position = snake->body[i - 1].position;
    }

    snake->body[0].position = next;

    if (position_equal(next, *food))
    {
        snake->size += 1;
        snake->body[snake->size - 1].position = last_pos;
        *food = position_get_random(2, LEVEL_SIZE - 2);
    }

    if (next.x == 0 || next.y == 0 || next.x == LEVEL_SIZE - 1 || next.y == LEVEL_SIZE - 1)
    {
        snake->size = 1;
        snake->body[0].position.x = LEVEL_SIZE / 2;
        snake->body[0].position.y = LEVEL_SIZE / 2;
    }

    for (i = 1; i < snake->size; i++)
    {
        if (position_equal(next, snake->body[i].position))
        {
            snake->size = 1;
            snake->body[0].position.x = LEVEL_SIZE / 2;
            snake->body[0].position.y = LEVEL_SIZE / 2;
        }
    }
}

//

// level-related

//

// render-related
void render_level()
{
    int i, j;
    for (i = 0; i < LEVEL_SIZE; i++)
        for (j = 0; j < LEVEL_SIZE; j++)
        {
            if (i == 0 || j == 0 || i == LEVEL_SIZE - 1 || j == LEVEL_SIZE - 1)
            {
                mvaddch(j, i, '+');
            }
            else
            {
                mvaddch(j, i, ' ');
            }
        }
}

void render_snake(SNAKE *snake)
{
    int i;
    for (i = 0; i < snake->size; i++)
    {
        POSITION position = snake->body[i].position;
        mvaddch(position.y, position.x, 'X');
    }
}

void render(SNAKE *snake, POSITION food)
{
    render_level();
    mvaddch(food.y, food.x, '@');
    render_snake(snake);
}

//

// input-related
int input_handle()
{
    getch();
    getch();
    int key = getch();
    return key;
}
//

int main()
{
    srand(time(NULL));
    initscr();
    timeout(0);
    noecho();
    curs_set(0);

    SNAKE *snake = snake_new(LEVEL_SIZE / 2, LEVEL_SIZE / 2);
    POSITION food = position_get_random(2, LEVEL_SIZE - 2);
    int counter = 0;

    while (true)
    {
        render(snake, food);

        int key = input_handle();
        if (key != -1)
            snake->dir = key;

        counter++;
        if (counter >= TICKRATE)
        {
            counter = 0;
            POSITION next = snake_get_next_position(snake);
            snake_step(snake, next, &food);
        }

        refresh();
        sleep(FRAMETIME);
    }

    return 0;
}
