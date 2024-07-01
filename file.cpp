#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <queue>
#include <queue>
#include <string.h>

using namespace std;

typedef struct Car{ //
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;
    bool isExist;
    char chr;
}Car;
typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;

void printCar(Car car) {
    printf("Car Information:\n");
    printf("  ID: %d\n", car.ID);
    printf("  Position: (%d, %d)\n", car.x, car.y);
    printf("  Dimensions: %dx%d\n", car.width, car.height);
    printf("  Speed: %d\n", car.speed);
    printf("  Color (integer): %d\n", car.clr); // May need conversion for color representation
    printf("  Is Exist: %s\n", car.isExist ? "true" : "false");
    printf("  Character: %c\n", car.chr);
}

void loadGame(Game *game) {
    FILE *infile = fopen("game.txt", "rb");

    if (!infile) {
        fprintf(stderr, "Error opening file for loading: %s\n", "game.txt");
        return;
    }

    // Game yapısını yükleyin
    fread(&game->leftKey, sizeof(game->leftKey), 1, infile);
    fread(&game->rightKey, sizeof(game->rightKey), 1, infile);
    fread(&game->IsGameRunning, sizeof(game->IsGameRunning), 1, infile);
    fread(&game->IsSaveCliked, sizeof(game->IsSaveCliked), 1, infile);
    fread(&game->counter, sizeof(game->counter), 1, infile);
    fread(&game->level, sizeof(game->level), 1, infile);
    fread(&game->moveSpeed, sizeof(game->moveSpeed), 1, infile);
    fread(&game->points, sizeof(game->points), 1, infile);

    // Arabaları yükleyin
    while (!feof(infile)) {
        Car car;
        fread(&car, sizeof(car), 1, infile);
        if (!feof(infile)) {
            game->cars.push(car);
        }
    }

    pthread_mutex_init(&game->mutexFile, NULL); // Mutex'i yeniden başlat
    fclose(infile);
}

void printGame(Game *game) {
    printf("Game State:\n");
    printf("Left Key: %d\n", game->leftKey);
    printf("Right Key: %d\n", game->rightKey);
    printf("Is Game Running: %d\n", game->IsGameRunning);
    printf("Is Save Clicked: %d\n", game->IsSaveCliked);
    printf("Counter: %d\n", game->counter);
    printf("Level: %d\n", game->level);
    printf("Move Speed: %d\n", game->moveSpeed);
    printf("Points: %d\n", game->points);

    printf("Cars in Queue:\n");
    for (size_t i = 0; i < game->cars.size(); ++i) {
        Car car = game->cars.front();
        game->cars.pop();
        printf("  Car %zu: ID=%d, PositionX=%d, PositionY=%d\n", i, car.ID, car.x, car.y);
    }
}
int main() {

    FILE* cars = fopen("cars.txt" , "rb+");
    //FILE* file = fopen("game.txt" , "rb+");

    Car c;
    fread(&c , sizeof(c) , 1 , cars);
    while(!feof(cars)) {
        printCar(c);
        fread(&c , sizeof(c) , 1 , cars);
    }

    printf("GAME----------------------------------\n");
    Game game;
    loadGame(&game);
    printGame(&game);

    fclose(cars);
    //fclose(file);

    return 0;
}
