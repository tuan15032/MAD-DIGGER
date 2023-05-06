#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <fstream>

using namespace std ;

// Kích thước của ảnh nhỏ
const int IMAGE_WIDTH = 49;
const int IMAGE_HEIGHT = 49;
const int TILE_SIZE = 49;

// Kích thước của Button
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 90;

// Kích thước frame
const int frameWidth = 120;
const int frameHeight = 120;

// Kích thước của background
const int BACKGROUND_WIDTH = 931 ;
const int BACKGROUND_HEIGHT = 539 ;

// Tọa độ nhân vật
int characterX = 441 ;
int characterY = 98 ;

// Tọa độ Monster trên mảng
int MonsterX ;
int MonsterY ;

// Kích thước mảng
const int ROW = 8;
const int COL = 19;

// Tọa độ nhân vật trên mảng
int x = 2 ;
int y = 10 ;

// Tài nguyên
int NUM_GOLD = 10;

int gold_found = 0 ;

// Trạng thái nhân vật
char status = 's' ;

int gold_map1[ROW+4][COL+2];

// Mảng chứa trạng thái của các ô trong MAP có vật cản hay ko
bool gold_map2[ROW+4][COL+2];

// Khởi tạo tài nguyên
void generate_gold_map()
{
    ifstream file ("MAP\\Goldmap1.txt") ;
    int m=0 , n=0 ;
    while (file >> gold_map1[n][m]) {
        m++; // tăng chỉ số cột
        if (file.peek() == '\n') { // nếu gặp dấu xuống dòng, chuyển sang hàng mới
        n++; // tăng chỉ số hàng
        m = 0; // reset chỉ số cột về 0
        }
    }
    file.close();
    int count = 0 ;
    srand(time(NULL));

    while (count < NUM_GOLD) {
        int i = rand() % 8 + 3 ;
        int j = rand() % COL + 1;
        if (gold_map1[i][j] != 2 && gold_map1[i][j] != 4 && gold_map1[i][j] != -1 && gold_map1[i][j] != 8) {
            gold_map1[i][j] = 2;
            count ++ ;
        }
        if (gold_map1[i][j] == 4)
        {
            gold_map1[i][j] = 8 ;
            count ++ ;
        }
    }

    for ( int i = 0 ; i<ROW+4; i++)
    {
        for ( int j = 0 ; j<COL+2 ; j++)
        {
            if (gold_map1[i][j]==5 || gold_map1[i][j]==-1) {gold_map2[i][j]= true ;}
            else {gold_map2[i][j]=false ; }
        }
    }
}

// Tạo cửa sổ SDL
SDL_Window* window = SDL_CreateWindow("MAD DIGGER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, SDL_WINDOW_SHOWN);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

// Tạo texture
SDL_Texture* tx (string s , SDL_Renderer * renderer)
{
    SDL_Surface* sf = IMG_Load(s.c_str()) ;
    SDL_Texture* x = SDL_CreateTextureFromSurface(renderer , sf) ;
    SDL_FreeSurface(sf) ;
    return x ;
}

// Load background texture
SDL_Texture * background = tx("Background\\background.png",renderer) ;

// Load GameOver texture
SDL_Texture * menu = tx("Background\\Menu.png",renderer) ;

// Load Win texture
SDL_Texture * Win = tx("End\\Win.png",renderer) ;

// Load background texture
SDL_Texture * Coin = tx("MAP\\Coin.png",renderer) ;

// Load GameOver texture
SDL_Texture * gameover = tx("End\\GameOver.png",renderer) ;

// Load small image texture
SDL_Texture * Land = tx("MAP\\Land.png",renderer) ;

// Load Lava texture
SDL_Texture * Lava = tx("MAP\\Lava.png",renderer) ;

// Load Move texture
SDL_Texture * UP = tx("Move\\UP.png",renderer) ;
SDL_Texture * DOWN = tx("Move\\DOWN.png",renderer) ;
SDL_Texture * LEFT = tx("Move\\LEFT.png",renderer) ;
SDL_Texture * RIGHT = tx("Move\\RIGHT.png",renderer) ;

// Load ROCK texture
SDL_Texture * ROCK = tx("MAP\\ROCK.png",renderer) ;
SDL_Texture * ROCK1 = tx("MAP\\ROCK1.png",renderer) ;
SDL_Texture * ROCK2 = tx("MAP\\ROCK2.png",renderer) ;

// Load Fire texture
SDL_Texture * Fire = tx("MAP\\Fire.png",renderer) ;

// Load Break texture
SDL_Texture * one = tx("Space\\1.png",renderer) ;
SDL_Texture * two = tx("Space\\2.png",renderer) ;
SDL_Texture * three = tx("Space\\3.png",renderer) ;
SDL_Texture * four = tx("Space\\4.png",renderer) ;

// Load Instruction texture
SDL_Texture * Ins1 = tx("Instruction\\Instruction1.png",renderer) ;
SDL_Texture * Ins2 = tx("Instruction\\Instruction2.png",renderer) ;
SDL_Texture * Ins3 = tx("Instruction\\Instruction3.png",renderer) ;

// Load Monster texture
SDL_Texture * Monster = tx("Monster.png",renderer) ;

// Khai báo biến đếm ngược
int countdown = 61;

// Khai báo biến thời gian
Uint32 last_time = SDL_GetTicks();

void printcd()
{
    // Tạo phông
    TTF_Font* font = TTF_OpenFont("BebasNeue-Regular.ttf", 28);

    // Tạo văn bản
    SDL_Color color = { 255, 255, 255 , 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font,to_string(countdown).c_str(), color);
    // Tạo texture
    SDL_Texture* cd = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect1 = { 441 , 30 , surface->w, surface->h };
    SDL_RenderCopy(renderer, cd, NULL, &dstrect1);
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(cd);
}


void printsc()
{
    // Tạo phông
    TTF_Font* font = TTF_OpenFont("BebasNeue-Regular.ttf", 28);

    // Tạo văn bản
    SDL_Color color = { 255, 255, 255 , 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Gold:", color);
    SDL_Surface* sc = TTF_RenderText_Solid(font, to_string(gold_found).c_str(), color);
    // Tạo texture
    SDL_Texture* gold = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture* Score = SDL_CreateTextureFromSurface(renderer, sc);
    SDL_Rect dstrect1 = { 800 , 30 , surface->w, surface->h };
    SDL_Rect dstrect2 = { 860 , 30 , 10, 34 };
    SDL_RenderCopy(renderer, gold, NULL, &dstrect1);
    SDL_RenderCopy(renderer, Score, NULL, &dstrect2);
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(Score);
}

// Cập nhật tọa độ nhân vật và trạng thái
void print1 ( SDL_Texture * status )
{
    SDL_Rect statusRect ={characterX, characterY, 49, 49};
    SDL_Rect Rec1 = { 0 , 0 , frameWidth , frameHeight } ;
    SDL_RenderCopy(renderer, status, &Rec1, &statusRect);
}

// In ra tài nguyên
void print2 ()
{
    for (int i = 1; i < 20; i++) {
        for (int j = 3; j < 11; j++) {
            // Tính vị trí của ảnh nhỏ
            int xPos = i * IMAGE_WIDTH - 49;
            int yPos = j * IMAGE_HEIGHT;

            // Tạo đối tượng SDL_Rect để định vị trí và kích thước của ảnh nhỏ
            SDL_Rect destRect = { xPos, yPos, IMAGE_WIDTH, IMAGE_HEIGHT};
            if (xPos!=characterX || yPos!=characterY)
            {
                if (gold_map2[j][i]==false)
                {
                    // Render ảnh nhỏ lên background tại vị trí hiện tại
                    if (gold_map1[j][i]==1 || gold_map1[j][i]==2) SDL_RenderCopy(renderer, Land, NULL, &destRect);
                    if (gold_map1[j][i]==4 || gold_map1[j][i]==8) SDL_RenderCopy(renderer, ROCK, NULL, &destRect);
                    if (gold_map1[j][i]==3) SDL_RenderCopy(renderer, ROCK1, NULL, &destRect);
                    if (gold_map1[j][i]==7) SDL_RenderCopy(renderer, ROCK2, NULL, &destRect);
                }
                if (gold_map2[j][i]== true && gold_map1[j][i]==2) SDL_RenderCopy(renderer, Coin, NULL, &destRect);
            }
            if (gold_map1[j][i] == -1) SDL_RenderCopy(renderer, Lava, NULL, &destRect);
        }
    }
}

SDL_Texture * funcion ()
{
    switch (status)
    {
        case 's' :
        return DOWN ;
        case 'w' :
        return UP ;
        case 'a' :
        return LEFT ;
        case 'd' :
        return RIGHT ;
    }
    return DOWN ;
}

void Update (SDL_Rect & Rect1)
{
    switch (status)
    {
        case 's' :
        characterY += 7 ;
        break ;
        case 'w' :
        characterY -= 7 ;
        break ;
        case 'a' :
        characterX-= 7 ;
        break ;
        case 'd' :
        characterX += 7 ;
        break ;
    }
}

void Load_break(SDL_Texture * st)
{
    for ( int i = 0 ; i<7 ; i++)
    {
        // Xóa màn hình
        SDL_RenderClear(renderer);
        // Vẽ frame hiện tại lên màn hình
        SDL_Rect frameRect = {i*120, 0, 120, 120};
        SDL_Rect Rect1 = {characterX, characterY , 49, 49};
        SDL_RenderCopy(renderer, background , NULL, NULL);
        printcd() ;
        printsc() ;
        print2() ;
        SDL_RenderCopy(renderer, st, &frameRect, &Rect1);
        // Hiển thị lên màn hình
        SDL_RenderPresent(renderer);
        // Tạm dừng 100ms trước khi vẽ frame tiếp theo
        SDL_Delay(10);
    }
}

void Load_mover(SDL_Texture * st)
{
    for ( int i = 0 ; i<7 ; i++)
    {
        // Xóa màn hình
        SDL_RenderClear(renderer);
        // Vẽ frame hiện tại lên màn hình
        SDL_Rect frameRect = {i*120, 0, 120, 120};
        SDL_Rect Rect1 = {characterX, characterY , 49, 49};
        SDL_RenderCopy(renderer, background , NULL, NULL);
        printcd() ;
        printsc() ;
        print2() ;
        SDL_RenderCopy(renderer, st, &frameRect, &Rect1);
        Update(Rect1) ;
        // Hiển thị lên màn hình
        SDL_RenderPresent(renderer);
        // Tạm dừng 100ms trước khi vẽ frame tiếp theo
        SDL_Delay(10);
    }

}

SDL_Event event;

// Di chuyển của nhân vật
void moverplayer ()
{
    switch (event.key.keysym.sym) {
        case SDLK_SPACE: {
            switch (status)
            {
                case 'w' :
                    if (gold_map2[x - 1][y] == true && gold_map1[x - 1][y] == 2) {
                        gold_map1[x - 1][y] = 0 ;
                        NUM_GOLD-- ;
                    }
                    if (gold_map1[x - 1][y]==1 || gold_map1[x - 1][y]==3 || gold_map1[x - 1][y]==7 || gold_map1[x - 1][y]==2) {
                        gold_map2[x - 1][y] = true;
                    }
                    if (gold_map1[x - 1][y]==7) gold_map1[x - 1][y]=2 ;
                    if (gold_map1[x - 1][y]==8) gold_map1[x - 1][y]=7 ;
                    if (gold_map1[x - 1][y]==4) gold_map1[x - 1][y]=3 ;
                    Load_break(two) ;
                break ;
                case 's' :
                    if (gold_map2[x + 1][y] == true && gold_map1[x + 1][y] == 2) {
                        gold_map1[x + 1][y] = 0 ;
                        NUM_GOLD-- ;
                    }
                    if (gold_map1[x + 1][y]==1 || gold_map1[x + 1][y]==3 || gold_map1[x + 1][y]==7 || gold_map1[x + 1][y]==2) {
                        gold_map2[x + 1][y] = true;
                    }
                    if (gold_map1[x + 1][y]==7) gold_map1[x + 1][y]=2 ;
                    if (gold_map1[x + 1][y]==8) gold_map1[x + 1][y]=7 ;
                    if (gold_map1[x + 1][y]==4) gold_map1[x + 1][y]=3 ;
                    Load_break(one) ;
                break ;
                case 'a' :
                    if (gold_map2[x][y - 1] == true && gold_map1[x][y - 1] == 2 ) {
                        gold_map1[x][y - 1] = 0 ;
                        NUM_GOLD-- ;
                    }
                    if (gold_map1[x][y - 1]==1 || gold_map1[x][y - 1]==3 || gold_map1[x][y - 1]==7 || gold_map1[x][y - 1]==2) {
                        gold_map2[x][y - 1] = true;
                    }
                    if (gold_map1[x][y - 1]==7) gold_map1[x][y - 1]=2 ;
                    if (gold_map1[x][y - 1]==8) gold_map1[x][y - 1]=7 ;
                    if (gold_map1[x][y - 1]==4) gold_map1[x][y - 1]=3 ;
                    Load_break(four) ;
                break ;
                case 'd':
                    if (gold_map2[x][y + 1] == true && gold_map1[x][y + 1] == 2) {
                        gold_map1[x][y + 1] = 0 ;
                        NUM_GOLD-- ;
                    }
                    if (gold_map1[x][y + 1]==1 || gold_map1[x][y + 1]==3 || gold_map1[x][y + 1]==7 || gold_map1[x][y + 1]==2) {
                        gold_map2[x][y + 1] = true;
                    }
                    if (gold_map1[x][y + 1]==7) gold_map1[x][y + 1]=2 ;
                    if (gold_map1[x][y + 1]==8) gold_map1[x][y + 1]=7 ;
                    if (gold_map1[x][y + 1]==4) gold_map1[x][y + 1]=3 ;
                    Load_break(three) ;
                break ;
            }
            printcd() ;
            print2() ;
            print1(funcion()) ;
            break ;
        }
        case SDLK_LEFT: {
            status = 'a' ;
            if (gold_map2[x][y-1] == true)
            {
                y-- ;
                Load_mover(LEFT) ;
            }
            printcd() ;
            print2() ;
            print1(funcion()) ;
            break;
        }
        case SDLK_RIGHT:{
            status = 'd' ;
            if (gold_map2[x][y+1] == true)
            {
                y++ ;
                Load_mover(RIGHT) ;
            }
            printcd() ;
            print2() ;
            print1(funcion()) ;
            break;
        }
        case SDLK_UP : {
            status = 'w' ;
            if (gold_map2[x - 1][y] == true)
            {
                x-- ;
                Load_mover(UP) ;
            }
            printcd() ;
            print2() ;
            print1(funcion()) ;
            break;
        }
        case SDLK_DOWN: {
            status = 's' ;
            if (gold_map2[x + 1][y] == true)
            {
                x++ ;
                Load_mover(DOWN) ;
            }
            printcd() ;
            print2() ;
            print1(funcion()) ;
            break ;
        }
        default :
        printcd() ;
        print2() ;
        print1(funcion()) ;
        break ;
    }
}

void Destroy ()
{
    // Giải phóng bộ nhớ và đóng SDL

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(menu);
    SDL_DestroyTexture(gameover);
    SDL_DestroyTexture(Coin);
    SDL_DestroyTexture(Fire);
    SDL_DestroyTexture(Win);
    SDL_DestroyTexture(Land);
    SDL_DestroyTexture(Lava);
    SDL_DestroyTexture(ROCK);
    SDL_DestroyTexture(ROCK1);
    SDL_DestroyTexture(ROCK2);
    SDL_DestroyTexture(UP) ;
    SDL_DestroyTexture(DOWN);
    SDL_DestroyTexture(RIGHT);
    SDL_DestroyTexture(LEFT);
    SDL_DestroyTexture(one);
    SDL_DestroyTexture(two);
    SDL_DestroyTexture(three);
    SDL_DestroyTexture(four);
    SDL_DestroyTexture(Ins1);
    SDL_DestroyTexture(Ins2);
    SDL_DestroyTexture(Ins3);
    SDL_DestroyTexture(Monster);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

Mix_Chunk * Over , *Gain , *win;

void Gameplay ()
{
    SDL_RenderClear(renderer) ;
    bool quit = false;
    while (!quit) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
        printcd() ;
        printsc() ;
        print2() ;
        print1(funcion()) ;
        // Cập nhật renderer
        SDL_RenderPresent(renderer);
        // Tính thời gian kể từ lần cuối cập nhật biến thời gian
        Uint32 current_time = SDL_GetTicks();
        Uint32 elapsed_time = current_time - last_time;

        // Nếu đã đủ thời gian, cập nhật biến đếm ngược và cập nhật biến thời gian
        if (elapsed_time >= 1000) {
            countdown--;
            last_time = current_time;
        }

        // Kiểm tra nếu đếm ngược bằng 0 thì dừng chương trình
        if (countdown == 0) {
            SDL_RenderCopy(renderer, background, NULL, NULL);
            printcd() ;
            printsc() ;
            print2() ;
            print1(funcion()) ;
            SDL_RenderCopy(renderer, gameover, NULL, NULL);
            // Cập nhật renderer
            SDL_RenderPresent(renderer);
            Mix_PlayChannel(-1, Over, 0);
            while (Mix_Playing(-1) != 0) {
                // Chờ...
            }
            return ;
        }
        if (elapsed_time < 1000) {
        // Xử lý sự kiện
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                 Destroy();
            } else if (event.type == SDL_KEYDOWN) {
                SDL_RenderCopy(renderer, background, NULL, NULL);
                printsc() ;
                printcd() ;
                // Vòng lặp in ảnh trên background
                moverplayer();
                // Cập nhật renderer
                SDL_RenderPresent(renderer);
                if (gold_map1[x][y]==2)
                {
                    gold_found++ ;
                    gold_map1[x][y]=1 ;
                    // Xóa Renderer
                    SDL_RenderClear(renderer) ;
                    SDL_RenderCopy(renderer, background, NULL, NULL);
                    printcd() ;
                    printsc() ;
                    print2() ;
                    print1(funcion()) ;
                    // Cập nhật renderer
                    SDL_RenderPresent(renderer);
                    Mix_PlayChannel(-1, Gain, 0);
                }
                if (gold_map1[x][y]==-1)
                {

                    SDL_Rect statusRect ={characterX, characterY, 49, 49};
                    SDL_RenderCopy(renderer, Fire, NULL, &statusRect);
                    SDL_RenderCopy(renderer, gameover, NULL, NULL);
                    printcd() ;
                    // Cập nhật renderer
                    SDL_RenderPresent(renderer);
                    Mix_PlayChannel(-1, Over, 0);
                    while (Mix_Playing(-1) != 0) {
                    }
                    return ;
                }
                if (gold_found==10)
                {
                    SDL_Delay(1000);
                    SDL_RenderCopy(renderer, Win, NULL, NULL);
                    // Cập nhật renderer
                    SDL_RenderPresent(renderer);
                    Mix_PlayChannel(-1, win, 0);
                    while (Mix_Playing(-1) != 0) {
                    return  ;
                    }
                }
            }
        }
        // Xóa Renderer
        SDL_RenderClear(renderer) ;
    }
    // Xóa Renderer
    SDL_RenderClear(renderer) ;
}
}

SDL_Texture * ins = Ins1 ;

void Instruction()
{
    SDL_RenderCopy(renderer,ins,NULL,NULL) ;
    SDL_RenderPresent(renderer) ;
    SDL_Event event1 ;
    if (ins == Ins1)
    {
        while (true)
        {
            while (SDL_PollEvent(&event1))
            {
                if (event1.type == SDL_KEYDOWN)
                {
                    switch (event1.key.keysym.sym)
                    {
                        case SDLK_RIGHT :
                        ins = Ins2 ;
                        SDL_RenderCopy(renderer,ins,NULL,NULL) ;
                        SDL_RenderPresent(renderer) ;
                        Instruction() ;
                        return ;
                        case SDLK_LEFT :
                        return ;
                    }
                }
            }
        }
    }
    else if (ins == Ins2)
    {
        while (true)
        {
            while (SDL_PollEvent(&event1))
            {
                if (event1.type == SDL_KEYDOWN)
                {
                    switch (event1.key.keysym.sym)
                    {
                        case SDLK_RIGHT :
                        ins = Ins3 ;
                        SDL_RenderCopy(renderer,ins,NULL,NULL) ;
                        SDL_RenderPresent(renderer);
                        Instruction() ;
                        return ;
                        case SDLK_LEFT :
                        ins = Ins1 ;
                        SDL_RenderCopy(renderer,ins,NULL,NULL) ;
                        SDL_RenderPresent(renderer);
                        Instruction() ;
                        return ;
                    }
                }
            }
        }
    }
    else if (ins == Ins3)
    {
        while (true)
        {
            while (SDL_PollEvent(&event1))
            {
                if (event1.type == SDL_KEYDOWN)
                {
                    switch (event1.key.keysym.sym)
                    {
                        case SDLK_RIGHT :
                        ins = Ins1 ;
                        return ;
                        case SDLK_LEFT :
                        ins = Ins2 ;
                        SDL_RenderCopy(renderer,ins,NULL,NULL) ;
                        SDL_RenderPresent(renderer) ;
                        Instruction() ;
                        return ;
                    }
                }
            }
        }
    }
}

void Reset()
{
    // Tọa độ nhân vật
    characterX = 441 ;

    characterY = 98 ;

    // Tọa độ nhân vật trên mảng
    x = 2 ;
    y = 10 ;

    // Tài nguyên
    NUM_GOLD = 10;

    gold_found = 0 ;

    // Trạng thái nhân vật
    status = 's' ;

    countdown = 61;

}
void Menu()
{
    generate_gold_map();
    SDL_RenderCopy(renderer, menu, NULL, NULL);
    SDL_RenderPresent(renderer) ;
    SDL_Event event1 ;
    bool quit = false ;
    while (!quit) {
    while (SDL_PollEvent(&event1)) {
        // lấy các sự kiện mới nhất từ hàng đợi sự kiện
        switch (event1.type) {
            case SDL_QUIT:
                Destroy() ;
                return ;
            case SDL_MOUSEBUTTONDOWN:
                int a, b;
                SDL_GetMouseState(&a, &b);
                if (a >= 355 && a < 355 + BUTTON_WIDTH && b >= 172 && b < 172 + BUTTON_HEIGHT) {
                    // Chọn Play
                    Gameplay() ;
                    Reset() ;
                    Menu() ;
                    Destroy() ;
                    return ;
                }
                else if (a >= 355 && a < 355 + BUTTON_WIDTH && b >= 302 && b < 302 + BUTTON_HEIGHT) {
                    Instruction() ;
                    Reset() ;
                    Menu() ;
                    Destroy() ;
                    return ;
                }
                else if (a >= 355 && a < 355 + BUTTON_WIDTH && b >= 432 &&  b < 432 + BUTTON_HEIGHT) {
                    Destroy() ;
                    return ;
                }
            break;
            }
        }
    }
}

int main(int argc, char* argv[]){

    // Khởi tạo SDL2
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Over = Mix_LoadWAV("GameOver.wav");
    Gain = Mix_LoadWAV("GainCoin.wav");
    win = Mix_LoadWAV("win.wav");
    Menu() ;

    Mix_FreeChunk(Over);
    Destroy();
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
