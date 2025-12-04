//sr 58
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define WIDTH 15
#define PLAY_ROWS 9
#define LANES 5
#define MAX_OBS 12
#define MAX_BUL 8

char player[64] = "PLAYER";
char bestName[64] = "NONE";
int bestScore = 0;

int lives = 3;
int score = 0;
int shipLane = 2;
int speedMs = 140;

const char *BEST_FILE = "best_score.txt";

typedef struct { int lane; int y; int alive; } Ob;
typedef struct { int lane; int y; int alive; } Bul;

Ob obs[MAX_OBS];
Bul bul[MAX_BUL];

int laneCol(int ln){
    int cols[LANES] = {2,4,6,8,10};
    if(ln<0) return cols[0];
    if(ln>=LANES) return cols[LANES-1];
    return cols[ln];
}

void setColor(int c){ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(WORD)c); }
void clearAll(){ int i; for(i=0;i<MAX_OBS;i++) obs[i].alive=0; for(i=0;i<MAX_BUL;i++) bul[i].alive=0; }

void loadBest(){
    FILE *f = fopen(BEST_FILE,"r");
    if(!f) return;
    fscanf(f,"%d\n",&bestScore);
    fgets(bestName, sizeof(bestName), f);
    int n = strlen(bestName);
    if(n && bestName[n-1]=='\n') bestName[n-1]=0;
    fclose(f);
}
void saveBest(){
    FILE *f = fopen(BEST_FILE,"w");
    if(!f) return;
    fprintf(f,"%d\n%s\n", bestScore, bestName);
    fclose(f);
}

void drawScreen(){
    int r,i;
    setColor(11); printf("  <<< SPACE SURVIVAL >>>\n");
    setColor(7);
    printf("Player: %s  Lives:", player);
    setColor(12); printf("%d ", lives);
    setColor(7); printf(" Score: "); setColor(10); printf("%d ", score);
    setColor(7); printf(" Best: "); setColor(14); printf("%d (%s)\n", bestScore, bestName);
    setColor(11); printf("+-------------+\n");
    for(r=0;r<PLAY_ROWS;r++){
        char line[WIDTH+1];
        for(i=0;i<WIDTH;i++) line[i] = ' ';
        line[0] = '|'; line[WIDTH-1] = '|'; line[WIDTH] = 0;
        for(i=0;i<MAX_OBS;i++){
            if(obs[i].alive && obs[i].y == r){
                line[laneCol(obs[i].lane)] = 'X';
            }
        }
        for(i=0;i<MAX_BUL;i++){
            if(bul[i].alive && bul[i].y == r){
                line[laneCol(bul[i].lane)] = '*';
            }
        }
        setColor(7); printf("%s\n", line);
    }
    for(i=0;i<WIDTH;i++){
        if(i==0 || i==WIDTH-1){ setColor(11); putchar('|'); }
        else if(i==laneCol(shipLane)){ setColor(9); putchar('^'); }
        else putchar(' ');
    }
    putchar('\n');
    setColor(11); printf("+-------------+\n");
    setColor(7); printf("Arrows Move | Space Shoot | P Pause | Q Quit\n");
}

void spawnObstacle(){
    int i;
    for(i=0;i<MAX_OBS;i++){
        if(!obs[i].alive){
            obs[i].alive = 1;
            obs[i].lane = rand() % LANES;
            obs[i].y = 0;
            break;
        }
    }
}

void spawnBullet(){
    int i;
    for(i=0;i<MAX_BUL;i++){
        if(!bul[i].alive){
            bul[i].alive = 1;
            bul[i].lane = shipLane;
            bul[i].y = PLAY_ROWS - 1;   // bottom of play area
            Beep(900,30);
            break;
        }
    }
}

int checkCollisionBeforeMove(){
    int i,b;
    for(b=0;b<MAX_BUL;b++){
        if(!bul[b].alive) continue;
        for(i=0;i<MAX_OBS;i++){
            if(!obs[i].alive) continue;
            if(bul[b].lane == obs[i].lane){
                if(bul[b].y == obs[i].y || bul[b].y == obs[i].y + 1){
                    obs[i].alive = 0;
                    bul[b].alive = 0;
                    score += 2;
                    Beep(1200,40);
                    return 1;
                }
            }
        }
    }
    return 0;
}

void moveEntities(){
    int i,b;
    for(b=0;b<MAX_BUL;b++){
        if(bul[b].alive){
            bul[b].y -= 1;
            if(bul[b].y < 0) bul[b].alive = 0;
        }
    }
    for(i=0;i<MAX_OBS;i++){
        if(obs[i].alive){
            obs[i].y += 1;
        }
    }
}

void checkReachedPlayer(){
    int i;
    for(i=0;i<MAX_OBS;i++){
        if(!obs[i].alive) continue;
        if(obs[i].y >= PLAY_ROWS){
            if(obs[i].lane == shipLane){
                lives--;
                Beep(400,120);
            } else {
                score++;
            }
            obs[i].alive = 0;
        }
    }
}

void checkCollisionAfterMove(){
    int i,b;
    for(b=0;b<MAX_BUL;b++){
        if(!bul[b].alive) continue;
        for(i=0;i<MAX_OBS;i++){
            if(!obs[i].alive) continue;
            if(bul[b].lane == obs[i].lane && bul[b].y == obs[i].y){
                bul[b].alive = 0;
                obs[i].alive = 0;
                score += 2;
                Beep(1200,40);
            }
        }
    }
}

void updateAll(){
    if(checkCollisionBeforeMove()) return;
    moveEntities();
    checkReachedPlayer();
    checkCollisionAfterMove();
}

int max(int a,int b){ return (a>b)?a:b; }

void runGame(){
    int spawnCounter = 0;
    int paused = 0;
    lives = 3; score = 0; shipLane = LANES/2; speedMs = 140;
    clearAll();

    while(lives > 0){
        if(_kbhit()){
            int c = getch();
            if(c == 0 || c == 224){
                c = getch();
                if(c == 75 && shipLane > 0) shipLane--;
                else if(c == 77 && shipLane < LANES-1) shipLane++;
            } else {
                if(c == ' ') spawnBullet();
                else if(c == 'p' || c == 'P') paused = !paused;
                else if(c == 'q' || c == 'Q' || c == 27) break;
            }
        }
        if(paused){ Sleep(150); continue; }

        if(spawnCounter <= 0){
            spawnObstacle();
            spawnCounter = max(1, 6 - score/4);
        } else spawnCounter--;

        updateAll();

        int newLevel = 1 + score / 8;
        if(newLevel > 1 && speedMs > 40) speedMs = 140 - (newLevel-1)*10;

        system("cls");
        drawScreen();
        Sleep(speedMs);
    }

    if(score > bestScore){
        bestScore = score;
        strncpy(bestName, player, sizeof(bestName)-1);
        bestName[sizeof(bestName)-1] = 0;
        saveBest();
    }

    system("cls");
    setColor(12); printf("=== GAME OVER ===\n"); setColor(7);
    printf("Player: %s\nScore: %d\nBest: %d (%s)\n", player, score, bestScore, bestName);
    printf("Press any key to continue...\n"); getch();
}

void showInstructions(){
    system("cls");
    printf("Controls:\n");
    printf("Left/Right arrows -> Move ship\n");
    printf("Space -> Shoot\n");
    printf("P -> Pause\n");
    printf("Q or ESC -> Quit to menu\n\n");
    printf("Press any key...\n"); getch();
}

int main(){
    srand((unsigned)time(NULL));
    loadBest();
    system("cls");
    printf("Enter your name: ");
    fgets(player, sizeof(player), stdin);
    int ln = strlen(player);
    if(ln && player[ln-1]=='\n') player[ln-1]=0;
    if(strlen(player)==0) strcpy(player,"PLAYER");

    while(1){
        system("cls");
        setColor(14); printf("=== SPACE SURVIVAL ===\n"); setColor(7);
        printf("1. Start Game\n2. Instructions\n3. Change Name\n4. Reset Best\n5. Exit\n");
        printf("Choose: ");
        int c = getch();
        if(c == '1') runGame();
        else if(c == '2') showInstructions();
        else if(c == '3'){
            system("cls"); printf("New name: ");
            fgets(player, sizeof(player), stdin); ln=strlen(player);
            if(ln && player[ln-1]=='\n') player[ln-1]=0;
        }
        else if(c == '4'){ bestScore = 0; strcpy(bestName,"NONE"); saveBest(); printf("Reset done\n"); getch(); }
        else if(c == '5') break;
    }
    return 0;
}

