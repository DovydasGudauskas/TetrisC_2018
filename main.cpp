#include <iostream>
#include <string>
#include <stdlib.h>//For clearing console and "srand" and "rand" and program exit
#include <time.h>//For console sleeping (and or FPS counter)
#include <windows.h>
#include <stdio.h> //For NULL

using namespace std;

struct Tile
{
    int X;
    int Y;
    int TileType;
    bool IsNumber;
    int ClockNumber;
};

struct Figure
{
    int X;
    int Y;
    int DisplaySymbol;
    bool FigureData[4][4];
};

bool FigureTypeData[7][4][4] =
{
        { {1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0} },
        { {0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0} },
        { {1,0,0,0},{1,0,0,0},{1,1,0,0},{0,0,0,0} },
        { {1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0} },
        { {1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0} },
        { {1,0,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0} },
        { {0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0} }
};

void DrawMap();
void MakeSpaces(int HowMany);
void MakeSpacesString(int HowMany, string &str);
void DrawWelcomeText(string Text,char OuterRingChar);
void DrawLineWithChars(int HowMany,char LineChar);
void InitializeMap();
void sleepcp(int milliseconds);
bool Randoming(int Chance);
void TimeShow();
void LevelShow();
void SpawnAFigure();
void ChangeDisplayFigure();
int RndNumber(int From,int To);
void MoveMovableFigureDown();
void MoveMovableFigureHorizontally(int X);
bool CanFigureFit(int X,int Y);
void MoveFigureDown();
void GenerateNewRNDBatch();
void RotateAFigure();
void CheckIfMadeAFullLine();
void MoveTilesDownOnY(int Y);

char SymbolDatabase[] = {'#',' ',':','C','B','O','Y','G','P','R','L','V'};
char NumberDatabase[] = {'0','1','2','3','4','5','6','7','8','9'};
int FiguresSymbolNumbers[] = {3,4,5,6,7,8,9};

bool Lost = false;//Did lose the game

float FramesPerSecond = 25;//FPS MUST be more or equal ONE
int FramesAfterMoveDown = 0;
float FigureMoveDownModifier = 0.25;

bool HasSpawnedFigure = false;
bool IsFirstFigure = true;
int CurrentBatchIncrement = 0;
int WhichRNDFigureBatch[7];

int ControllingFigureIncrement = 0;
int MaxFramesWaitToSpawnFigure = 2;
int FramesWaitToSpawnFigure = 0;

int MaxFramesWaitToRotateFigure = 3;
int FramesWaitToRotateFigure = 0;

int CurrentLevel = 1;

Figure ControllableFigure;
Figure DisplayFigure;

time_t now;

Tile Tiles[17][22];

int Score = 0;
int LineCombo = 0;
int TotalLinesDeleted = 0;

int main()//TODO Pause feature
{
    DrawWelcomeText("Welcome to Teris!",'-');

    cout << "ENTER anything + enter to continue..." << endl;
    int TEMP = 0;
    cin >> TEMP;

    now = time(NULL);

    system("cls");

    srand(time(NULL));

    HANDLE hStdout;//Cursor initialize
    COORD destCoord;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    destCoord.X = 0;
    destCoord.Y = 0;

    InitializeMap();

    while(true)
    {
    SetConsoleCursorPosition(hStdout, destCoord);//Set cursor pos

    if(Lost == false)
    {
    DrawMap();
    if(HasSpawnedFigure == false && FramesWaitToSpawnFigure <=0)
    {
        SpawnAFigure();
        HasSpawnedFigure = true;
    }

    if(GetAsyncKeyState('W') && 0x8000)//Movement Input
    {
        if(FramesWaitToRotateFigure <= 0)
        {
        RotateAFigure();
        }
    }
    else if(GetAsyncKeyState('S') && 0x8000)
    {
        if(FramesWaitToSpawnFigure <= 0)
        {
            MoveFigureDown();
        }
    }
    else if(GetAsyncKeyState('D') && 0x8000)
    {
        MoveMovableFigureHorizontally(1);
    }
    else if(GetAsyncKeyState('A') && 0x8000)
    {
        MoveMovableFigureHorizontally(-1);
    }

    LevelShow();//Shows current level
    TimeShow();//Manages time

    MoveMovableFigureDown();

    if(FramesWaitToSpawnFigure > 0)
    {
        FramesWaitToSpawnFigure--;
    }
    if(FramesWaitToRotateFigure > 0)
    {
        FramesWaitToRotateFigure--;
    }

    CheckIfMadeAFullLine();

    sleepcp((1/FramesPerSecond)*1000);
    }
    else
    {
    DrawWelcomeText("You have lost!",'-');
    MakeSpaces(31);
    cout << "Your score: " << Score << endl << endl;
    cout << "Do you want to restart? (N/Y)" << endl;
    sleepcp(500);
    if(GetAsyncKeyState('N') && 0x8000)
    {
        exit (EXIT_FAILURE);
    }
    else if(GetAsyncKeyState('Y') && 0x8000)
    {
        InitializeMap();
        Lost = false;
    }
    }
    }

    return 0;
}

void RotateAFigure()
{
    FramesWaitToRotateFigure = MaxFramesWaitToRotateFigure;
    int FoundX=0;
    for(int x = 3;x >= 0;x--)
    {
        for(int y = 0;y < 4;y++)
        {
            if(ControllableFigure.FigureData[x][y] == true)
            {
                FoundX = x;
                x = 0;
                y = 4;
            }
        }
    }

    bool TempFigureData[4][4];
    for(int y = 0;y < 4;y++)
    {
        for(int x = 0;x < 4;x++)
        {
            TempFigureData[x][y] = false;
        }
    }

    int Y = 0;
    for(int x = FoundX;x >= 0;x--)
    {
        int X = 0;
        for(int y = 0;y < 4;y++)
        {
            if(ControllableFigure.FigureData[x][y]==true)
            {
                TempFigureData[X][Y] = true;
            }
            X++;
        }
        Y++;
    }

    bool ReverseFigureData[4][4];

    for(int y = 0;y < 4;y++)
    {
        for(int x = 0;x < 4;x++)
        {
            ReverseFigureData[x][y] = ControllableFigure.FigureData[x][y];
            ControllableFigure.FigureData[x][y] = TempFigureData[x][y];
        }
    }

    if(CanFigureFit(0,0)==false)
    {
        for(int y = 0;y < 4;y++)
        {
            for(int x = 0;x < 4;x++)
            {
                ControllableFigure.FigureData[x][y] = ReverseFigureData[x][y];
            }
        }
    }
}

void CheckIfMadeAFullLine()
{
    bool Exit = true;
    do
    {
    Exit = true;
    for(int y = 20;y > 1;y--)
    {
        bool IsFull = true;
        for(int x = 1;x < 11;x++)
        {
            if(Tiles[x][y].TileType == 1)
            {
                IsFull = false;
            }
        }
        if(IsFull == true)
        {
            LineCombo++;
            TotalLinesDeleted++;
            MoveTilesDownOnY(y);
            Exit = false;
        }
    }
    }while(Exit == false);

    switch(LineCombo)
    {
        case 1:
        {
            Score+=40*(CurrentLevel);break;
        }
        case 2:
        {
            Score+=100*(CurrentLevel);break;
        }
        case 3:
        {
            Score+=300*(CurrentLevel);break;
        }
        case 4:
        {
            Score+=1200*(CurrentLevel);break;
        }
    }
    LineCombo = 0;

    if(TotalLinesDeleted >= 10)
    {
        TotalLinesDeleted-=10;
        CurrentLevel++;
    }
}

void MoveTilesDownOnY(int Y)
{
    for(int y = Y;y > 1;y--)
    {
        for(int x = 1;x < 11;x++)
        {
            Tiles[x][y].TileType = Tiles[x][y-1].TileType;
        }
    }
}

bool CanFigureFit(int X,int Y)
{
    for(int y=0;y<=3;y++)
    {
        for(int x=0;x<=3;x++)
        {
            if(ControllableFigure.FigureData[x][y] == true && Tiles[x+ControllableFigure.X+X][y+ControllableFigure.Y+Y].TileType != 1)
            {
                return false;
            }
        }
    }
    return true;
}

void MoveMovableFigureDown()
{
    FramesAfterMoveDown++;
    if(FramesAfterMoveDown >= FigureMoveDownModifier*FramesPerSecond)
    {
        MoveFigureDown();
    }
}

void MoveFigureDown()
{
    FramesAfterMoveDown = 0;
    if(CanFigureFit(0,1)){ControllableFigure.Y++;}
    else{HasSpawnedFigure=false;FramesWaitToSpawnFigure=MaxFramesWaitToSpawnFigure;}
}

void MoveMovableFigureHorizontally(int X)
{
    if(X == 1 && ControllableFigure.X < 10)
    {
        if(CanFigureFit(1,0))ControllableFigure.X++;
    }
    else if(X == -1 && ControllableFigure.X > 1)
    {
        if(CanFigureFit(-1,0))ControllableFigure.X--;
    }
}

int RndNumber(int From,int To)//Imtinai
{
    return rand() % (To+1) + From;
}

void SpawnAFigure()
{
    if(IsFirstFigure == true)
    {
        IsFirstFigure = false;
    }
    else
    {
        for(int y = 0;y < 4;y++)
        {
            for(int x = 0;x < 4;x++)
            {
                if(ControllableFigure.FigureData[x][y] == true)
                {
                    Tiles[x+ControllableFigure.X][y+ControllableFigure.Y].TileType = ControllableFigure.DisplaySymbol;
                }
            }
        }
    }
    ControllableFigure = DisplayFigure;
    ControllableFigure.X = 5;
    ControllableFigure.Y = 1;
    if(CanFigureFit(0,0) == false)
    {
        Lost = true;
        system("cls");
    }
    ChangeDisplayFigure();
}

void GenerateNewRNDBatch()
{
    for(int i = 0;i < 7;i++)
    {
       WhichRNDFigureBatch[i] = -1;
    }

    for(int i = 0;i < 6;i++)
    {
       bool FoundPlace = true;
       do
       {
        FoundPlace = true;
        int Increment = RndNumber(0,6);
        if(WhichRNDFigureBatch[Increment] == -1)
        {
            WhichRNDFigureBatch[Increment] = i;
        }
        else
        {
            FoundPlace = false;
        }
       }while(FoundPlace == false);
    }

    for(int i = 0;i < 7;i++)
    {
        if(WhichRNDFigureBatch[i] == -1)
        {
            WhichRNDFigureBatch[i] = 6;
        }
    }
}

void ChangeDisplayFigure()
{
    if(CurrentBatchIncrement >= 6)
    {
        GenerateNewRNDBatch();
        CurrentBatchIncrement = 0;
    }
    int WhichFigure = WhichRNDFigureBatch[CurrentBatchIncrement];
    CurrentBatchIncrement++;

    for(int y = 0;y <= 3;y++)
    {
        for(int x = 0;x <= 3;x++)
        {
            DisplayFigure.X = 13;
            DisplayFigure.Y = 2;
            DisplayFigure.DisplaySymbol = WhichFigure+3;
            DisplayFigure.FigureData[x][y] = FigureTypeData[WhichFigure][y][x];
        }
    }
}

void LevelShow()
{
    if(CurrentLevel < 10)
    {
        Tiles[15][11].ClockNumber = 0;
        Tiles[16][11].ClockNumber = CurrentLevel;
    }
    else
    {
        int FirstNumber = (int)(CurrentLevel/10);
        int SecondNumber = (int)(CurrentLevel%10);
        Tiles[15][11].ClockNumber = FirstNumber;
        Tiles[16][11].ClockNumber = SecondNumber;
    }
}

void TimeShow()
{
    int Seconds = difftime(time(NULL),now);
    int Minutes = 0;

    while(Seconds >= 60)
    {
       Seconds-=60;
       Minutes++;
    }

    if(Seconds < 10)
    {
        Tiles[15][9].ClockNumber = 0;
        Tiles[16][9].ClockNumber = Seconds;
    }
    else
    {
        int FirstNumber = (int)(Seconds/10);
        int SecondNumber = (int)(Seconds%10);
        Tiles[15][9].ClockNumber = FirstNumber;
        Tiles[16][9].ClockNumber = SecondNumber;
    }
    if(Minutes < 10)
    {
        Tiles[12][9].ClockNumber = 0;
        Tiles[13][9].ClockNumber = Minutes;
    }
    else
    {
        int FirstNumber = (int)(Minutes/10);
        int SecondNumber = (int)(Minutes%10);
        Tiles[12][9].ClockNumber = FirstNumber;
        Tiles[13][9].ClockNumber = SecondNumber;
    }
}

bool Randoming(int Chance)
{
int Temp = rand() % 100;
if(Temp < Chance)
{
    return true;
}
return false;
}

void InitializeMap()
{
    GenerateNewRNDBatch();
    IsFirstFigure = true;
    Score = 0;
    CurrentLevel = 1;

    for(int y = 0;y < 22;y++)
    {
        for(int x = 0;x < 17;x++)
        {
            Tiles[x][y].TileType = 1;
            Tiles[x][y].IsNumber = false;
            Tiles[x][y].ClockNumber = 0;
        }
    }

    Tiles[15][9].IsNumber = true; Tiles[16][9].IsNumber = true; Tiles[12][9].IsNumber = true;
    Tiles[13][9].IsNumber = true;Tiles[15][11].IsNumber = true;Tiles[16][11].IsNumber = true;
    Tiles[12][11].TileType = 10;   Tiles[13][11].TileType = 11;  Tiles[14][11].TileType = 10;

    for(int y = 0;y < 22;y++)
    {
        for(int x = 0;x < 17;x++)
        {
            if((y == 21 && x <12) || y == 0 || x == 11 || x == 0 ||( y == 7 && x > 11) ||( x == 16 && y < 8))//TODO merge every if to one
            {
                Tiles[x][y].TileType = 0;
            }
            else if(x==14 && y==9)
            {
                Tiles[x][y].TileType = 2;
            }
        }
    }
    ChangeDisplayFigure();
}

void DrawMap()
{
    string Batch = "";
    Tile TilesWFigures[17][22];

    for(int y=0;y<22;y++)
    {
        for(int x=0;x<17;x++)
        {
            TilesWFigures[x][y].X = Tiles[x][y].X;
            TilesWFigures[x][y].Y = Tiles[x][y].Y;
            TilesWFigures[x][y].TileType = Tiles[x][y].TileType;
            TilesWFigures[x][y].IsNumber = Tiles[x][y].IsNumber;
            TilesWFigures[x][y].ClockNumber = Tiles[x][y].ClockNumber;
        }
    }

    for(int y=0;y<=3;y++)
    {
        for(int x=0;x<=3;x++)
        {
            if(DisplayFigure.FigureData[x][y] == true)
            {
                TilesWFigures[x+DisplayFigure.X][y+DisplayFigure.Y].TileType = DisplayFigure.DisplaySymbol;
            }
            if(ControllableFigure.FigureData[x][y] == true)
            {
                TilesWFigures[x+ControllableFigure.X][y+ControllableFigure.Y].TileType = ControllableFigure.DisplaySymbol;
            }
        }
    }
    Batch += '\n';
    for(int y = 0;y < 22;y++)
    {
        MakeSpacesString(30,Batch);
        for(int x = 0;x < 17;x++)
        {
            if(TilesWFigures[x][y].IsNumber == false)
            {
                Batch += SymbolDatabase[TilesWFigures[x][y].TileType];
            }
            else
            {
                Batch += NumberDatabase[TilesWFigures[x][y].ClockNumber];
            }
        }
        Batch += '\n';
    }

    Batch += '\n';
    MakeSpacesString(30,Batch);
    Batch += "Score: " + Score;
    cout << Batch;
}

void MakeSpacesString(int HowMany, string &str)
{
    for(int i = 0;i < HowMany;i++)str += " ";
}

void MakeSpaces(int HowMany)
{
    for(int i = 0;i < HowMany;i++)cout <<" ";
}

void DrawWelcomeText(string Text,char OuterRingChar)
{
     cout << endl << endl;
     DrawLineWithChars(Text.length(),OuterRingChar);
     MakeSpaces(30);
     cout << OuterRingChar << Text << OuterRingChar << endl;
     DrawLineWithChars(Text.length(),OuterRingChar);
     cout << endl << endl;
}

void DrawLineWithChars(int HowMany,char LineChar)
{
    MakeSpaces(30);
    for(int i = 0;i < HowMany+2;i++)
    {
        cout << LineChar;
    }
    cout << endl;
}

void sleepcp(int milliseconds) // Cross-platform sleep function
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end){}
}
