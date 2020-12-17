#include <stdio.h>
#include <raylib.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define SCREENWIDTH 675
#define SCREENHEIGHT 810
#define OLD_ROSE (Color){255, 0, 0 , 255}
#define CORAL (Color){255, 140, 0, 255}
#define AMBER (Color){255, 215, 0, 255}
#define FOREST (Color){58, 174, 89, 255}
#define AMARANTH (Color){255, 167, 179, 255}
#define MAYA (Color){116, 185, 255, 255}
#define WHITEGRAY (Color){233, 237, 245, 255}
#define NAMELENGTH 10

// color peg struct
struct Piece
{
    int status; // 0 for start 1 for placed
    Vector2 position;
    Color col;
};

// hole and key peg struct
struct Hole
{
    Vector2 position;
    Color col;
};

// struct use to read highscore file
struct ScoreList{
    char name[20];
    int min;
    float sec;
} scoreRecord[5];

// global structure array of 6 color pegs
struct Piece piece[6] = {{0,{50,765},OLD_ROSE},
                       {0,{130,765},CORAL},
                       {0,{210,765},GOLD},
                       {0,{290,765},FOREST},
                       {0,{370,765},MAYA},
                       {0,{450,765},AMARANTH}};
// global structure array of all key pegs
struct Hole check[8][4] = { { {{400,30}, GRAY}, {{430,30},GRAY}, {{400,60},GRAY}, {{430,60},GRAY} },
                           { {{400,120}, GRAY}, {{430,120},GRAY}, {{400,150},GRAY}, {{430,150},GRAY} },
                           { {{400,210}, GRAY}, {{430,210},GRAY}, {{400,240},GRAY}, {{430,240},GRAY} },
                           { {{400,300}, GRAY}, {{430,300},GRAY}, {{400,330},GRAY}, {{430,330},GRAY} },
                           { {{400,390}, GRAY}, {{430,390},GRAY}, {{400,420},GRAY}, {{430,420},GRAY} },
                           { {{400,480}, GRAY}, {{430,480},GRAY}, {{400,510},GRAY}, {{430,510},GRAY} },
                           { {{400,570}, GRAY}, {{430,570},GRAY}, {{400,600},GRAY}, {{430,600},GRAY} },
                           { {{400,660}, GRAY}, {{430,660},GRAY}, {{400,690},GRAY}, {{430,690},GRAY} }};
// global structure array of hole to insert the color pegs
struct Hole hole[8][4] = { { {{50,45}, GRAY}, {{130,45},GRAY}, {{210,45},GRAY}, {{290,45},GRAY} },
                          { {{50,135}, GRAY}, {{130,135},GRAY}, {{210,135},GRAY}, {{290,135},GRAY} },
                          { {{50,225}, GRAY}, {{130,225},GRAY}, {{210,225},GRAY}, {{290,225},GRAY} },
                          { {{50,315}, GRAY}, {{130,315},GRAY}, {{210,315},GRAY}, {{290,315},GRAY} },
                          { {{50,405}, GRAY}, {{130,405},GRAY}, {{210,405},GRAY}, {{290,405},GRAY} },
                          { {{50,495}, GRAY}, {{130,495},GRAY}, {{210,495},GRAY}, {{290,495},GRAY} },
                          { {{50,585}, GRAY}, {{130,585},GRAY}, {{210,585},GRAY}, {{290,585},GRAY} },
                          { {{50,675}, GRAY}, {{130,675},GRAY}, {{210,675},GRAY}, {{290,675},GRAY} }};

// global variable
int answered = 0, attempt = 1, recorded = 0, place = 0;

Color pieceColor[6] = {OLD_ROSE,CORAL,AMBER,FOREST,MAYA,AMARANTH};  // colors of color pegs in order
Sound pop, swing;
Texture2D menu, namebg, logoTexture, instruction, wintext, losetext;
Font inFont;
Image icon;


void drawElements(void);            // draw every elements in the game scene 2
void pickAnswer(int, int []);       // player choose an answer
void randomAnswer(int []);          // random the correct answer when the game start
int checkAnswer(int [], int []);    // check the answer and show the black and white key pegs
void undoAnswer(int [], int);       // remove the most recent answer
void restartGame(int [], int []);   // reset all variable and random new correct answer
int highscore(char[],int,float);    // to overwrite the highscore file
void loadHighscore(void);
void unloadAll(void);

int main()
{
    char playerName[NAMELENGTH + 1] = "\0";
    float alpha1 = 0.0f, alpha2 = 0.0f, count = 0.0f, countF = 0.0f;
    int key, getHighscore = 0, gameScene = 0, min = 0;
    int win = 0, letterCount = 0, showinstruct = 0, fade1 = 1, fade2 = 0;
    int CorrectAnswer[4] = {0,0,0,0};   // array that store the correct code
    int userAnswer[4] = {0,0,0,0};      // array that store the answer that user chose
    
    // position of the text on the game screen
    Vector2 namepos = {SCREENWIDTH / 2 - 150, SCREENHEIGHT / 2 - 20};
    Vector2 enterNamepos = {SCREENWIDTH / 2 - 200, SCREENHEIGHT / 2 - 70};
    Vector2 keymap[3] = {{SCREENWIDTH * 5 / 7 , 20,},
                        {SCREENWIDTH * 5 / 7 , 280},
                        {SCREENWIDTH * 5 / 7 , 315}};
    Vector2 time = {SCREENWIDTH * 5 / 7 , 400};
    Vector2 logo = {SCREENWIDTH * 5 / 7 + 10 , 600};
    Vector2 toprec = {SCREENWIDTH * 5 / 7 + 10 , 580};
    Vector2 namelist[5] = {{SCREENWIDTH * 5 / 7 + 25 , 580},
                        {SCREENWIDTH * 5 / 7 + 25 , 600},
                        {SCREENWIDTH * 5 / 7 + 25 , 620},
                        {SCREENWIDTH * 5 / 7 + 25 , 640},
                        {SCREENWIDTH * 5 / 7 + 25 , 660}};
    
    randomAnswer(CorrectAnswer);        // random the correct answer
    
    // open the highscore file to show the top recorders
    loadHighscore();
    
    // initialize window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "MASTERMIND GAME");
    icon = LoadImage("resources/logo.png");
    SetWindowIcon(icon);
    
    // load sound effects
    InitAudioDevice();
    pop = LoadSound("resources/popsound.ogg");
    swing = LoadSound("resources/swing.ogg");
    
    // import font from otf files
    inFont = LoadFont("resources/KMITLGO Bold ver 1.00.otf");
    
    // create texture for menu screen
    instruction = LoadTexture("resources/instruction.png");
    logoTexture = LoadTexture("resources/logo.png");
    menu = LoadTexture("resources/menu.png");
    namebg = LoadTexture("resources/namebg.png");
    wintext = LoadTexture("resources/win.png");
    losetext = LoadTexture("resources/lose.png");
    
    while (!WindowShouldClose())
    {
        // switch the scenes
        switch (gameScene){
            // starting scene
            case 0:
                // reset the player name
                letterCount = 0;
                for (int i = 0; i < NAMELENGTH; i++)    playerName[i] = '\0';
                getHighscore = 0;
                // press space to change to next scene
                if (IsKeyPressed(KEY_SPACE)){
                    PlaySound(pop);
                    if (showinstruct == 0)
                        gameScene = 1;
                    else gameScene = 2;
                }
                // press H for show the instruction
                else if (IsKeyPressed(KEY_H)){
                    PlaySound(pop);
                    gameScene = 1;
                }
                break;
            // instruction scene
            case 1:
                // press space to continue
                if (IsKeyPressed(KEY_SPACE)){
                    PlaySound(pop);
                    showinstruct = 1;
                    gameScene = 2;
                }
                // press H to back to starting scene
                else if (IsKeyPressed(KEY_H)){
                    PlaySound(pop);
                    gameScene = 0;
                    showinstruct = 1;
                }
                break;
            // naming scene
            case 2:
                key = GetKeyPressed();
                // allow user to press only a-z and numbers
                if ((((key >= 48) && (key <= 57)) || ((key >= 97) && (key <= 122))) && letterCount < NAMELENGTH)
                {
                    playerName[letterCount] = toupper((char)key);
                    PlaySound(pop);
                    letterCount++;
                }
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    letterCount--;
                    playerName[letterCount] = '\0';
                    PlaySound(pop);

                    if (letterCount < 0) letterCount = 0;
                }
                // press enter to finish naming
                if (IsKeyPressed(KEY_ENTER))
                    if (letterCount >= 1){
                        PlaySound(pop);
                        gameScene = 3;
                    }
                break;
            // playing scene
            case 3:
                // game end , restart game Y/N??
                if ((!win && attempt > 8) || win){
                    if (win && getHighscore == 0)
                        getHighscore = highscore(playerName,min, count);
                    if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_N)){
                        restartGame(CorrectAnswer, userAnswer);
                        count = 0.0;
                        win = 0;
                        min = 0;
                        fade1 = 1;
                        fade2 = 0;
                        alpha1 = 0.0f;
                        alpha2 = 0.0f;
                        if (IsKeyPressed(KEY_N))    gameScene = 0;
                    }
                }
                // game still running
                if (!win && attempt <= 8){
                    // time counting
                    count += GetFrameTime();
                    if (59.989 <= count){
                        count = 0.0;
                        min++;
                    }
                    // press space bar to restart the game
                    if (IsKeyPressed(KEY_SPACE)){
                        restartGame(CorrectAnswer, userAnswer);
                        count = 0.0;
                        win = 0;
                        min = 0;
                    }
                    // choose answer
                    if (answered < 4){
                        if (IsKeyPressed(KEY_ONE) && piece[0].status == 0)
                            pickAnswer(0,userAnswer);
                        else if (IsKeyPressed(KEY_TWO) && piece[1].status == 0)
                            pickAnswer(1,userAnswer);
                        else if (IsKeyPressed(KEY_THREE) && piece[2].status == 0)
                            pickAnswer(2,userAnswer);
                        else if (IsKeyPressed(KEY_FOUR) && piece[3].status == 0)
                            pickAnswer(3,userAnswer);
                        else if (IsKeyPressed(KEY_FIVE) && piece[4].status == 0)
                            pickAnswer(4,userAnswer);
                        else if (IsKeyPressed(KEY_SIX) && piece[5].status == 0)
                            pickAnswer(5,userAnswer);
                    }
                    // if answered can be undo
                    if (answered > 0)
                        if (IsKeyPressed(KEY_BACKSPACE))
                        {
                            PlaySound(pop);
                            for (int i = 0; i < 6; i++)
                                if (hole[attempt-1][answered-1].position.x == piece[i].position.x &&
                                    hole[attempt-1][answered-1].position.y == piece[i].position.y){
                                    piece[i].position.x = 50 + (80 * i);
                                    piece[i].position.y = 765;
                                    piece[i].status = 0;
                                    userAnswer[answered-1] = 0;
                                    answered--;
                                    break;
                                }
                        }
                    // if answer all 4 pegs, answer can be check
                    if (answered == 4)
                        if (IsKeyPressed(KEY_ENTER))
                            win = checkAnswer(userAnswer,CorrectAnswer);
                }
                break;
        }
        // drawing section
        BeginDrawing();
        ClearBackground(WHITEGRAY);
        
        switch(gameScene){
            // starting scene
            case 0:
                DrawTexture(menu, 0, 0, RAYWHITE);
                break;
            // instruction scene
            case 1:
                DrawTexture(instruction, 0, 0, RAYWHITE);
                break;
            // naming scene
            case 2:
                DrawTexture(namebg, 0, 0, RAYWHITE);
                DrawTextEx(inFont, "ENTER YOUR NAME: ", enterNamepos, 50, 1, DARKBLUE);
                DrawRectangle(namepos.x - 8, namepos.y, 200, 40, WHITE);
                DrawTextEx(inFont,playerName, namepos, 40,1, BLACK);
                break;
            // playing scene
            case 3:
                drawElements();
                DrawTextEx(inFont,"KEYMAP", keymap[0],30,1, RED);
                DrawTextEx(inFont,"\n1 RED\n2 ORANGE\n3 YELLOW\n4 GREEN\n5 CYAN\n6 BLUE", keymap[0],25,1, BLACK);
                DrawTextEx(inFont,"Enter CHECK", keymap[1],25,1, BLACK);
                DrawTextEx(inFont,"BACKSPACE UNDO\nSPACE RESTART", keymap[2],25,1, BLACK);
                DrawTextEx(inFont,FormatText("Time : \n%02d:%02.0f sec",min,floor(count)),time,40,1,RED);
                DrawTextEx(inFont,FormatText("\n\n%s",playerName),time,40,1,BLUE);
                
                // logo and top five shifting
                if (fade1){
                    if (fade1 == 1)  alpha1 += GetFrameTime() / 2.0;
                    else if (fade1 == 2){
                        alpha1 = 1;
                        countF += GetFrameTime() / 2.0;
                    }
                    else if (fade1 == 0){
                        alpha1 = 0;
                        countF += GetFrameTime() / 2.0;
                    }
                    else if (fade1 == -1)   alpha1 -= GetFrameTime() / 2.0;
                    
                    if (alpha1 >= 1.0 && fade1 != 2){
                        fade1 = 2;
                        countF = 0.0;
                    }
                    else if (alpha1 <= 0.0 && fade1 == -1){
                        fade1 = 0;
                        countF = 0.0;
                        fade2 = 1;
                        alpha1 = 0.0f;
                    }
                    else if (countF >= 1.0){
                        fade1 = -1;
                    }
                    DrawTexture(logoTexture, logo.x, logo.y, Fade(RAYWHITE, alpha1));
                }
                else if (fade2){
                    if (fade2 == 1)  alpha2 += GetFrameTime() / 2.0;
                    else if (fade2 == 2){
                        alpha2 = 1;
                        countF += GetFrameTime() / 2.0;
                    }
                    else if (fade2 == 0){
                        alpha2 = 0;
                        countF += GetFrameTime() / 2.0;
                    }
                    else if (fade2 == -1)   alpha2 -= GetFrameTime() / 2.0;
                    if (alpha2 >= 1.0 && fade2 != 2){
                        fade2 = 2;
                        countF = 0.0;
                    }
                    else if (alpha2 <= 0.0 && fade2 == -1){
                        fade2 = 0;
                        countF = 0.0;
                        fade1 = 1;
                        alpha2 = 0.0f;
                    }
                    else if (countF >= 1.0){
                        fade2 = -1;
                    }
                    DrawTextEx(inFont, "TOP FIVE", toprec, 30, 1, Fade(BLACK, alpha2));
                    for (int i = 0; i < 5; i++)
                        DrawTextEx(inFont, FormatText("\n%d. %s\n", i+1, scoreRecord[i].name),namelist[i] , 30, 1, Fade(FOREST,alpha2));
                }
                
                if (win || attempt == 9){
                    if (win){
                        highscore(playerName, min, count);
                        DrawTexture(wintext, (SCREENWIDTH - 500) / 2, (SCREENHEIGHT - 512) / 2, RAYWHITE);
                    }
                    else if (attempt == 9){
                        DrawTexture(losetext, (SCREENWIDTH - 500) / 2, (SCREENHEIGHT - 490) / 2, RAYWHITE);
                    }
                    for (int i = 0; i < 4; i++)
                        piece[i].col = pieceColor[CorrectAnswer[i]-1];
                    piece[4].col = WHITEGRAY;
                    piece[5].col = WHITEGRAY;
                }
                for (int i = 0; i < 6; i++){
                    Vector2 colornum = {43 + (i * 80),705};
                    DrawTextEx(inFont, FormatText("%d",i+1) , colornum , 30, 1, BLACK);
                }
                break;
            }
        EndDrawing();
    }
    unloadAll();
    CloseWindow();
}

void randomAnswer(int ans[])
{
    int different = 0;
    srand(time(NULL));
    for (int i = 0; i < 4; i++)
        ans[i] = 1 + rand() % 6;
    while (!different){         // check that all 4 correct answer cant be the same answer
        different = 1;
        for (int i = 0; i < 3; i++)
            for (int j = i+1; j < 4; j++)
                if (ans[i] == ans[j]){
                    different = 0;
                    ans[j] = 1 + rand() % 6;
                }
    }
}

void loadHighscore()
{
    FILE *inFile;
    inFile = fopen("highscore.txt", "r");
    if (inFile == NULL) printf("File cant be opened\n");
    else
        for (int i = 0; i < 5; i++)
            fscanf(inFile, "%s %d %f\n", scoreRecord[i].name, &scoreRecord[i].min, &scoreRecord[i].sec);
    fclose(inFile);
}

void drawElements()
{
    // draw key pegs and holes
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++){
            DrawCircle(check[i][j].position.x, check[i][j].position.y, 10, check[i][j].col);
            DrawCircle(hole[i][j].position.x, hole[i][j].position.y, 25, hole[i][j].col);
        }
    // draw all 6 color pegs
    for (int i = 0;i < 6; i++)
        DrawCircle(piece[i].position.x, piece[i].position.y, 30, piece[i].col);
    
}

void pickAnswer(int num, int ans[])
{
    // move the chosen piece to the hole position
    piece[num].position = hole[attempt-1][answered].position;
    PlaySound(pop);
    ans[answered] = num + 1;
    answered++;
    piece[num].status = 1;
}



int checkAnswer(int ans[], int cor[])
{
    //      ------------- Show the correct answer ----------------------------------
    printf("Correct answer is: ");
    for (int i = 0; i < 4; i++) printf("%d\t", cor[i]);
    printf("\n");
    //      ------------------------------------------------------------------------
    
    PlaySound(swing);
    int black = 0, white = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (ans[i] == cor[j]){
                if (i == j) black++;
                else white++;
                continue;
            }
    for (int i = 0; i < black; i++)         // draw black key pegs
        check[attempt-1][i].col = BLACK;
    for (int i = 0; i < white; i++)         // draw white key pegs
        check[attempt-1][i+black].col = WHITE;
    for (int i = 0; i < 4; i++)             // change the hole into the answered colors
        hole[attempt-1][i].col = piece[ans[i]-1].col;
    for (int i = 0; i < 6; i++){            // reset the position of the color pegs
        piece[i].position.x = 50 + 80 * i;
        piece[i].position.y = 765;
        piece[i].status = 0;
    }
    if (black == 4) return 1;       // if 4 key pegs is black then player win the game
    answered = 0;
    attempt++;
    return 0;
}



int highscore(char playername[], int minute, float second)
{
    int placement = 0;
    FILE *File;
    File = fopen("highscore.txt", "w");
    for (int i = 0; i < 5; i++){
        if ((strcmp(playername, scoreRecord[i].name) == 0) && recorded == 0)
            recorded = 1;
        if ((minute <= scoreRecord[i].min) && recorded == 0)
            if (second < scoreRecord[i].sec){
                strcpy(scoreRecord[i].name, playername);
                scoreRecord[i].min = minute;
                scoreRecord[i].sec = second;
                recorded = 1;
            }
        fprintf(File, "%s %d %.2f\n",scoreRecord[i].name, scoreRecord[i].min, scoreRecord[i].sec);
    }
    fclose(File);
    return placement;
}



void restartGame(int CorrectAnswer[], int userAnswer[])
{
    // reset all answers and every variables
    for (int i = 0; i < 4; i++){
        userAnswer[i] = 0;
        CorrectAnswer[i] = 0;
    }
    answered = 0;
    randomAnswer(CorrectAnswer);
    attempt = 1;
    for (int i = 0;i < 8; i++)
        for (int j = 0; j < 4; j++){
            hole[i][j].col = GRAY;
            check[i][j].col = GRAY;
        }
    for (int i = 0; i < 6; i++){
        piece[i].position.x = 50 + (80 * i);
        piece[i].position.y = 765;
        piece[i].status = 0;
        piece[i].col = pieceColor[i];
    }
    recorded = 0;
}

// unload all resources
void unloadAll()
{
    UnloadFont(inFont);
    UnloadSound(pop);
    UnloadSound(swing);
    UnloadTexture(menu);
    UnloadTexture(namebg);
    UnloadTexture(logoTexture);
    UnloadTexture(instruction);
    UnloadTexture(wintext);
    UnloadTexture(losetext);
    UnloadImage(icon);
}
