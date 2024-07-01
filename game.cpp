#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>
#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key
#define RightKeyArrow 261 // ASCII code of the right arrow key
#define leftKeyA 97// ASCII code of A
#define RightKeyD 100 // ASCII code of D
#define ESC 27 // // ASCII code of the ESC key
#define ENTER 10 // ASCII code of the ENTER key
#define KEYPUP 259 // ASCII code of the up arrow key
#define KEYDOWN 258 // ASCII code of the down arrow key
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player
#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written
#define MENUX 10 // x coordinate for the starting row of the menus
#define MENUY 5 // y coordinate for the starting row of the menus
#define MENUDIF 2 // difference between menu rows 
#define MENUDIFX 20 // difference between menu columns
#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars
#define settingMenuItem 2 // number of options in the setting menu
#define mainMenuItem 6 // number of options in the main menu
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
Game playingGame; // Global variable used for new game
const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";
//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};
//Array with options for the Main menu
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};

void drawCar(Car c, int type, int direction); //prints or remove the given car on the screen
void printWindow(); //Draws the road on the screen
void *newGame(void *); // manages new game
void initGame(); // Assigns initial values to all control parameters for the new game
void initWindow(); //Creates a new window and sets I/O settings
void* generateEnemyCars(void*arg); // aysenur
void* runCar(void*); // dogukan
Car randomGenerateCar(); // aysenur
void addQueue(Car car); // dogukan
void* isCollide(void*); // aysenur
void drawTrees(); // aysenur
int firstprintWindow(); // dogukan
void enterSelection(int currentLocation); // dogukan
void drawMenu(int currentLocation); // aysenur
void drawSettings(int state); // dogukan
void settings(); // dogukan
void printInstruction(); // aysenur
void points(); // dogukan
void save(); // dogukan
int  load(); // dogukan
void writePoint(); // aysenur
void writeCar(Car car); // aysenur
void writeGame(); // aysenur
void* runLoadedCar(void*arg); // dogukan

int main()
{
    playingGame.leftKey = leftKeyArrow;
    playingGame.rightKey = RightKeyArrow;
    initGame();
    int state = firstprintWindow();
    if (state == -1) {
        return 0;3
    }
    if(state == 1) {
        return 0;
    }

    initWindow();

    pthread_t th1; //create new thread
    pthread_t carTh;

    pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
    pthread_create(&carTh , NULL , generateEnemyCars , NULL);

    pthread_join(th1, NULL); //Wait for the thread to finish, when the newGame function finishes, the thread will also finish.
    pthread_join(carTh , NULL);

  //  pthread_join(treesTh , NULL);
  
    return 0;
}

void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER; //assigns the initial value for the mutex
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1;
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}

void *newGame(void *)
{
    printWindow(); 
    drawCar(playingGame.current,2,1); // Draw the car the player is driving on the screen
    drawTrees();
    int key;
    while (playingGame.IsGameRunning) { //continue until the game is over
            key = getch(); //Get input for the player to press the arrow keys
            if (key != KEYERROR) {
                 if (key == playingGame.leftKey) { // If the left  key is pressed
                        if (playingGame.current.x > 2) {
                            drawCar(playingGame.current,1,1); // removes player's car from screen
                            playingGame.current.x-=playingGame.current.speed; // update position
                            drawCar(playingGame.current,2,1); // draw player's car with new position
                        }
                }
                else if (key == playingGame.rightKey) { // If the left  key is pressed
                        if (playingGame.current.x < (wWidth - 7)) {
                            drawCar(playingGame.current,1,1); // removes player's car from screen
                            playingGame.current.x+=playingGame.current.speed; // update position
                            drawCar(playingGame.current,2,1); // draw player's car with new position
                        }
                }
                else if (key == SAVEKEY) {
                        save();
                        return NULL;
                }
            }
            char buffer[50];
            sprintf(buffer, "%d", playingGame.points);
            mvprintw(42, 91, buffer); //left side of the road
         usleep(GAMESLEEPRATE); // sleep
    }
    mvprintw(42, 91, "GAME FINISH!"); //left side of the road
    return NULL;
}

//x = 91, y = 42.
void initWindow()
{
	initscr();            // initialize the ncurses window
	start_color();        // enable color manipulation
	keypad(stdscr, true); // enable the keypad for the screen
	nodelay(stdscr, true);// set the getch() function to non-blocking mode
	curs_set(0);          // hide the cursor
	cbreak();             // disable line buffering
	noecho();             // don't echo characters entered by the user
	clear();              // clear the screen
    sleep(1);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
}

void printWindow()
{
    for (int i = 1; i < wHeight - 1; ++i) {
		//mvprintw: Used to print text on the window, paramters order: y , x , string
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*"); 
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        mvprintw(i, lineX, "#");
    }
}

int firstprintWindow()
{
  initWindow(); 
  int key = 0;
  int currentLocation = 0;
 bool flag = true;
 int r = 0;
  while (true)
  {
    
    key = getch();
    if (key == KEYPUP)
    {
        clear();
        if (currentLocation<=0)
        {
           currentLocation=5;
        }
        else{
            currentLocation--;
        }
        
    }
    else if (key == KEYDOWN)
    {
        clear();
        if (currentLocation>=5)
        {
           currentLocation=0;
        }
        else{
            currentLocation++;
        }
    }

    drawMenu(currentLocation);

    if (key == ENTER)
    {
        clear();
        if (currentLocation == 5) {
            return -1;
            //exit(EXIT_SUCCESS);
        }
        else if (currentLocation == 0) {
            return 0;
        }
        else if (currentLocation == 1) {
           r = load();
           if (r == -1) {
                return -1;
           }
           return 1;
        }
        enterSelection(currentLocation);
    }

  }
    return 0;
}

void save() {
    writePoint();
    writeGame();
    playingGame.IsSaveCliked = true;
    playingGame.IsGameRunning = false;
}

void writeCar(Car car){
   FILE *file;
   file=fopen(CarsTxt, "ab+");
   fwrite(&car, sizeof(car), 1, file);
   fclose(file);
}

void writeGame(){
    FILE *outfile = fopen(gameTxt, "wb");
    Game* game;
    game = &playingGame;

    if (!outfile) {
        fprintf(stderr, "Error opening file for saving: %s\n", gameTxt);
        return;
    }

    // Game yapısını kaydedin
    fwrite(&game->leftKey, sizeof(game->leftKey), 1, outfile);
    fwrite(&game->rightKey, sizeof(game->rightKey), 1, outfile);
    fwrite(&game->IsGameRunning, sizeof(game->IsGameRunning), 1, outfile);
    fwrite(&game->IsSaveCliked, sizeof(game->IsSaveCliked), 1, outfile);
    fwrite(&game->counter, sizeof(game->counter), 1, outfile);
    fwrite(&game->level, sizeof(game->level), 1, outfile);
    fwrite(&game->moveSpeed, sizeof(game->moveSpeed), 1, outfile);
    fwrite(&game->points, sizeof(game->points), 1, outfile);
    fwrite(&game->current, sizeof(game->current), 1, outfile);
    // Arabaları kaydedin
    for (size_t i = 0; i < game->cars.size(); ++i) {
        fwrite(&game->cars.front(), sizeof(Car), 1, outfile);
        game->cars.pop();
    }

    fclose(outfile);
}

void writePoint(){
    FILE *file;
    int point=playingGame.points;
    file=fopen(pointsTxt,"a+");
	fprintf(file,"%d\n",point);
	fclose(file);
}

void enterSelection(int currentLocation) { 
    switch (currentLocation)
    {
    case 2: // inst
    printInstruction();
    break;
    case 3: // settings
    settings();
    break;
    case 4: // point
    points();
    break;
    default:
        break;
    }
}

int load() {
    FILE* gameFile = fopen("game.txt", "rb");
    FILE* carsFile = fopen("cars.txt", "rb+");

    if (gameFile == NULL || carsFile == NULL) {
        perror("Error opening files");
        return -1; // Or handle error differently
    }

    if (!gameFile) {
        fprintf(stderr, "Error opening file for loading: %s\n", "game.txt");
        return -1;
    }

    Game* game;
    game = &playingGame;

    // Game yapısını yükleyin
    fread(&game->leftKey, sizeof(game->leftKey), 1, gameFile);
    fread(&game->rightKey, sizeof(game->rightKey), 1, gameFile);
    fread(&game->IsGameRunning, sizeof(game->IsGameRunning), 1, gameFile);
    fread(&game->IsSaveCliked, sizeof(game->IsSaveCliked), 1, gameFile);
    fread(&game->counter, sizeof(game->counter), 1, gameFile);
    fread(&game->level, sizeof(game->level), 1, gameFile);
    fread(&game->moveSpeed, sizeof(game->moveSpeed), 1, gameFile);
    fread(&game->points, sizeof(game->points), 1, gameFile);
    fread(&game->current, sizeof(game->current), 1, gameFile);

    game->IsGameRunning = true;
    // Arabaları yükleyin
    while (!feof(gameFile)) {
        Car car;
        fread(&car, sizeof(car), 1, gameFile);
        if (!feof(gameFile)) {
            game->cars.push(car);
        }
    }

    pthread_mutex_init(&game->mutexFile, NULL); // Mutex'i yeniden başlat

    pthread_t carTh;
    pthread_t cars[10];
    pthread_t loadedGameTh;

    initWindow();

    pthread_create(&loadedGameTh , NULL , newGame , NULL);

    int i = 0;
    while (!feof(carsFile)) {
        Car car;
        fread(&car, sizeof(car), 1, carsFile);
        if (!feof(carsFile)) {
            pthread_create(&cars[i],NULL,runLoadedCar , (void*)&car);
            i++;
        }
    }

    pthread_create(&carTh , NULL , generateEnemyCars , NULL);


    for (int j = 0; j < i; j++) {
        pthread_join(cars[j],NULL);
    }

    pthread_join(carTh , NULL);
    pthread_join(loadedGameTh , NULL);

    fclose(gameFile);
    fclose(carsFile);

    return 0;
}

void* runLoadedCar(void*arg) {
    Car enemy = *(Car*)arg;
    if (playingGame.cars.size()>0)
    {
        pthread_t collision;
        pthread_create(&collision , NULL , isCollide , (Car*)arg);
        int enemyBottom = 0;
        while (enemy.y <= wHeight)
        {
            sleep(1);
            enemyBottom = enemy.y + enemy.height;
            drawCar(enemy , 1 , 0);
            enemy.y += 1 + (rand() % enemy.speed); 
            drawCar(enemy , 2 , 0);
        }
        playingGame.points += (enemy.height * enemy.width);
        drawCar(enemy , 1 , 0);
        pthread_join(collision , NULL);
        return NULL;
    }
    return NULL;
}

void points() {
    initWindow();
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    int counter = 0;
    int key;
    int row = 0;
    attron(COLOR_PAIR(2));
    char buffer[50];
    char game[15];
    while (counter <= 5) {
        key = getch();
        FILE* pointsFile = fopen(pointsTxt , "r+");
        int row = 5;
        int col = 5;
        int c = 0;
        while(!feof(pointsFile)) {     
            fscanf(pointsFile , "%s" , buffer);
            sprintf(game, "Game %d:",c);
            strcat(game , buffer);
            mvprintw(MENUY + col,MENUX + row,  game);
            if (c >= 10) {
                row += 20;
                col = 5;
                c = 0;
            }
            col += 3;
            c++;
        }
        fclose(pointsFile);
        sleep(1);
        counter++;
    }
    attroff(COLOR_PAIR(2));
    clear();
}

void printInstruction(){
    initWindow();
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    int key = 0;
    int counter = 0;
    const char *instructions[60] = {"< or A: moves the car to the left" , "> or D: moves the car to the right" , "ESC: exits the game without saving" , "s: saves and exits game"};
    while(counter <= 5) {
            key = getch();
            attron(COLOR_PAIR(2));
            mvprintw(MENUY,MENUX,  instructions[0]);
            mvprintw(MENUY+2,MENUX,  instructions[1]);
            mvprintw(MENUY+4,MENUX, instructions[2]);
            mvprintw(MENUY+6,MENUX, instructions[3]);
            attroff(COLOR_PAIR(2));
            sleep(1);
            counter++;
    }
    clear();
}

void settings() {
    initWindow();
    int key;
    int state = 0;
    while (true) {
        key = getch();
        
        if (key == KEYPUP) {
            clear();
            if (state == 0) {
                state = 1;
            }
            else {
                state--;
            }
        }
        else if (key == KEYDOWN) {
            clear();
            if (state == 1) {
                state = 0;
            }
            else {
                state++;
            }
        }

        if (key == ENTER) {
            if (state == 0) {
                playingGame.rightKey = RightKeyArrow;
                playingGame.leftKey = leftKeyArrow;
                return;
            }
            else if (state == 1) {
                playingGame.rightKey = RightKeyD;
                playingGame.leftKey = leftKeyA;
                return;
            }
        }

        drawSettings(state);
    }
    clear();
}

void drawSettings(int state) {
    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    const char *settingMenu2[50] = {"->Play with < and > arrow keys","->Play with A and D keys"};
    if (state == 0) {
        attron(COLOR_PAIR(1));
        mvprintw(MENUY , MENUX, settingMenu2[0]);
        attroff(COLOR_PAIR(1)); 
        attron(COLOR_PAIR(2));
        mvprintw(MENUY+3 , MENUX, settingMenu[1]); 
        attroff(COLOR_PAIR(2));
    }
    else if (state == 1) {
        attron(COLOR_PAIR(2));
        mvprintw(MENUY , MENUX, settingMenu[0]);
        attroff(COLOR_PAIR(2)); 
        attron(COLOR_PAIR(1));
        mvprintw(MENUY+3 , MENUX, settingMenu2[1]); 
        attroff(COLOR_PAIR(1));
    }
    fflush(stdin);
    fflush(stdout);
}

void drawMenu(int currentLocation) {
    const char *arrowMenu[50] = {"->New Game","->Load the last game","->Instructions", "->Settings","->Points","->Exit"};
    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    switch (currentLocation)
    {
    case 0:
			attron(COLOR_PAIR(1));
            mvprintw( MENUY,MENUX, arrowMenu[0]);
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            mvprintw(MENUY+2,MENUX,  mainMenu[1]);
            mvprintw(MENUY+4,MENUX,  mainMenu[2]);
            mvprintw(MENUY+6,MENUX, mainMenu[3]);
            mvprintw(MENUY+8,MENUX, mainMenu[4]);
            mvprintw(MENUY+10,MENUX,  mainMenu[5]);
            attroff(COLOR_PAIR(2));
        break;
    case 1:
            attron(COLOR_PAIR(2));
            mvprintw( MENUY,MENUX, mainMenu[0]);
            attroff(COLOR_PAIR(2));
			attron(COLOR_PAIR(1));
            mvprintw(MENUY+2,MENUX,  arrowMenu[1]);
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            mvprintw(MENUY+4,MENUX,  mainMenu[2]);
            mvprintw( MENUY+6,MENUX, mainMenu[3]);
            mvprintw( MENUY+8,MENUX, mainMenu[4]);
            mvprintw(MENUY+10,MENUX,  mainMenu[5]);
            attroff(COLOR_PAIR(2));
    case 2:
			attron(COLOR_PAIR(2));
            mvprintw( MENUY,MENUX, mainMenu[0]);
            mvprintw(MENUY+2,MENUX,  mainMenu[1]);
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            mvprintw(MENUY+4,MENUX,  arrowMenu[2]);
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            mvprintw( MENUY+6,MENUX, mainMenu[3]);
            mvprintw( MENUY+8,MENUX, mainMenu[4]);
            mvprintw(MENUY+10,MENUX,  mainMenu[5]);
            attroff(COLOR_PAIR(2));
    break;
    case 3:
			attron(COLOR_PAIR(2));
            mvprintw( MENUY,MENUX, mainMenu[0]);
            mvprintw(MENUY+2,MENUX,  mainMenu[1]);
            mvprintw(MENUY+4,MENUX,  mainMenu[2]);
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            mvprintw( MENUY+6,MENUX, arrowMenu[3]);
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            mvprintw( MENUY+8,MENUX, mainMenu[4]);
            mvprintw(MENUY+10,MENUX,  mainMenu[5]);
            attroff(COLOR_PAIR(2));
    break;
    case 4:
			attron(COLOR_PAIR(2));
            mvprintw( MENUY,MENUX, mainMenu[0]);
            mvprintw(MENUY+2,MENUX,  mainMenu[1]);
            mvprintw(MENUY+4,MENUX,  mainMenu[2]);
            mvprintw( MENUY+6,MENUX, mainMenu[3]);
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            mvprintw( MENUY+8,MENUX, arrowMenu[4]);
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            mvprintw(MENUY+10,MENUX,  mainMenu[5]);
            attroff(COLOR_PAIR(2));
    break;
    case 5:
			attron(COLOR_PAIR(2));
            mvprintw( MENUY,MENUX, mainMenu[0]);
            mvprintw(MENUY+2,MENUX,  mainMenu[1]);
            mvprintw(MENUY+4,MENUX,  mainMenu[2]);
            mvprintw( MENUY+6,MENUX, mainMenu[3]);
            mvprintw( MENUY+8,MENUX, mainMenu[4]);
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            mvprintw(MENUY+10,MENUX,  arrowMenu[5]);
            attroff(COLOR_PAIR(1));
    break;
    default:
        break;
    }

}

void drawCar(Car c, int type, int direction )
{
	//If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
            init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
            //0: Black (COLOR_BLACK)
			//1: Red (COLOR_RED)
			//2: Green (COLOR_GREEN)
			//3: Yellow (COLOR_YELLOW)
			//4: Blue (COLOR_BLUE)
			attron(COLOR_PAIR(c.ID));//enable color pair
            char drawnChar;
            if (type == 1 ) 
               drawnChar = ' '; // to remove car
            else
               drawnChar= c.chr; //  to draw char
		    //mvhline: used to draw a horizontal line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
            mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
            if(direction == 0) // If it is any car on the road
                mvhline(c.y + c.height, c.x, drawnChar, c.width); 
            else //player's card
                mvhline(c.y -1, c.x, drawnChar, c.width);
		    //mvvline: used to draw a vertical line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
            char text[5];
            if (type == 1 )
                sprintf(text,"  "); //to remove point 
            else
                 sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
            mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
            attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}

void* generateEnemyCars(void*) {
    int ID = IDSTART;
    pthread_t threads[10];
    int thID = 0;
    while (true) {
            sleep(1);
            Car car = randomGenerateCar();
            if (ID >= IDMAX) {
                ID = 10;
            }
            else {
                ID++;
            }
            car.ID = ID;
            addQueue(car);
            if (playingGame.cars.size()<=5 && thID < 5) {
                int random = rand() % (4 - 2 + 1) + 2;
                sleep(random);
                pthread_t newCarThread;
                pthread_create(&newCarThread , NULL , runCar , NULL);
                threads[thID] = newCarThread;
                thID++;
            }
    }
    for(int i = 0; i < 5 ; i++) 
    {
        pthread_join(threads[i] , NULL);
    }     
}


Car randomGenerateCar(){
    // random number
    srand(time(NULL));
    int x, y,height,widht,clr;
    
    do {
        x = rand() % (90 - 5 + 1) + 5;
        widht=rand() % (7 - 5 + 1) + 5;
    } while(x == 45 || (x + widht) == 45);

    y= rand() % 11;  y=y-10;
    height=rand() % (7 - 5 + 1) + 5;
    clr = rand() % (4 - 1 + 1) + 1;
    char chr;
    int numbertForChr =rand() % 3;
 switch (numbertForChr) {
    case 0:
      chr = '*';
      break;
    case 1:
      chr = '#';
      break;
    case 2:
      chr = '+';
      break;
  }
    //car generate
    Car car;
    car.x=x;
    car.y=y;
    car.height=height;
    car.width=widht;
    car.speed=(height/2);
    car.clr=clr;
    car.chr=chr;
    car.isExist=false;

    return car;
}

void addQueue(Car car) {
    if (playingGame.cars.size()<=5)
    {
      playingGame.cars.push(car);
    } 
}

void* runCar(void*) {
    if (playingGame.cars.size()>0)
    {
        Car enemy = playingGame.cars.front();
        pthread_t collision;
        pthread_create(&collision , NULL , isCollide , &enemy);
        playingGame.cars.pop();
        int enemyBottom = 0;
        while (enemy.y <= wHeight && !playingGame.IsSaveCliked)
        {
            sleep(1);
            enemyBottom = enemy.y + enemy.height;
            drawCar(enemy , 1 , 0);
            enemy.y += 1 + (rand() % enemy.speed); 
            drawCar(enemy , 2 , 0);
        }
        drawCar(enemy , 1 , 0);
        pthread_join(collision , NULL);
        return NULL;
    }
    return NULL;
}

void* isCollide(void*arg) {
    Car* enemyP = (Car*)(arg);
    Car enemy = *enemyP;
    while (enemy.y <= wHeight) {

        
        if (playingGame.IsSaveCliked) {
            if(pthread_mutex_lock(&playingGame.mutexFile) == 0) {
                writeCar(enemy);
                pthread_mutex_unlock(&playingGame.mutexFile);
            }
            return NULL;
        }

        enemy = *enemyP;

        int playerRight = playingGame.current.x + playingGame.current.width;
        int playerBottom = playingGame.current.y + playingGame.current.height;

        int enemyRight = enemy.x + enemy.width;
        int enemyBottom = enemy.y + enemy.height;


    if (enemy.x < playerRight && enemyRight > playingGame.current.x && enemy.y < playerBottom && enemyBottom > playingGame.current.y)
        {
            playingGame.IsGameRunning = false;
            break;
        }
    }
    playingGame.points += (enemy.height * enemy.width);
    return NULL;
}

void drawTrees() {
// 5px tree - 10px red 2px 3 2 1 green px : 3
    int treeStartX = wWidth + 5;
    int treeStartY = 16;

        treeStartX = wWidth + 5;
        treeStartY = 16;
        for (int i = 0; i < 3; i++) // 3 trees
        {
            mvprintw(treeStartY, treeStartX, "  #  ");
            mvprintw(treeStartY - 1, treeStartX , "  #  ");
            mvprintw(treeStartY - 2, treeStartX , "* * *");
            mvprintw(treeStartY - 3, treeStartX , " * * ");
            mvprintw(treeStartY - 4, treeStartX , "  *  ");
            treeStartY += 10;
        }

}
