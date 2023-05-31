#include <iostream>
#include <time.h> //для работы с датой и временем.
#include <windows.h> //включает функции WinAPI sleep(), beep() и т.д
#include <conio.h> //библиотека для создания текстового интерфейса пользователя

using namespace std;

enum Color { Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGray, DarkGray, LightBlue = 9, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White, Orange };

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); //Дескриптор активного окна

enum Napravlenie { Up = 72, Left = 75, Right = 77, Down = 80, Enter = 13, escepushka = 27, BackSpace = 8, Q = 113 };

void setColor(Color text, Color background) { //функция изменения цвета текста
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text)); // параметры текста
}

void SetCursor(int x, int y) { //функция для того чтобы устанавливать позицию курсора в консоли по оси x и y
    COORD myCoords = { x, y }; //инициализация координат
    SetConsoleCursorPosition(hStdOut, myCoords); //Способ перемещения курсора на нужные координаты
}

void hidecursor() //функция, которая скрывает мигающий курсор консоли
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}


void Avake() { //функция вывода названия игры
    setColor(White, Black);
    SetCursor(24, 9);
    cout << "МОРСКОЙ БОЙ";
}

struct Ships { //структура характеристик кораблика
    int deck = { 0 }; //количество палуб
    bool horizontal = true;
    bool alive = true;
    int x, y; //Позиции кораблика по "x" и "y" (первая палуба), необходима для правильного уничтожения кораблика
};

void hit(Ships* characteristic, int ship_pos[12][24], int plus, int& end) { //уничтожение корабликов и определение победителя в игре
    int A = 0; //для прорисовки границ убитого корабля
    int B = 13; //указывает на нужный кораблик
    int C = 0; //количество уничтоженных корабликов (если = 10, то игра завершается)
    bool alive = false;
    for (size_t n = 13; n > 3; n--) //проверка на наличие кораблей в массиве
    {
        for (size_t i = 1; i < 11; i++)
        {
            for (size_t r = 2; r < 22; r++)
            {
                if (ship_pos[i][r] == n)
                {
                    alive = true;
                    i = 11;
                    break;
                }
            }
        }
        if (alive == false)
        {
            characteristic[B - n + plus].alive = false;
        }
        else
        {
            alive = false;
        }
    }
    for (size_t i = plus; i < 10 + plus; i++) //проверка всех кораблей на уничтожение
    {
        if (i == 0 || i == 10) //для 4-х палубных
        {
            A = 0;
        }
        else if (i == 1 || i == 2 || i == 11 || i == 12) //для 3-х палубных
        {
            A = 1;
        }
        else if (i > 2 && i < 6 || i > 12 && i < 16)  //для 2-х палубных
        {
            A = 2;
        }
        else  //для 1-о палубных
        {
            A = 3;
        }
        if (characteristic[i].alive == false)
        {
            if (characteristic[i].horizontal == true)
            {
                for (size_t q = 0; q < 3; q++) //границы корабля, чтобы не стояли рядом
                {
                    for (size_t w = 0; w < characteristic[i].deck * 3 + A; w++)
                    {
                        ship_pos[characteristic[i].y - 2 + q][characteristic[i].x - 4 + w] = 2;
                    }
                }
                for (size_t g = 0; g < 2 * characteristic[i].deck; g++) //цикл для обведения уничтоженного корабля
                {
                    ship_pos[characteristic[i].y - 1][characteristic[i].x - 2 + g] = 3;
                }
            }
            else if (characteristic[i].horizontal == false)  //если он имеет вертикальное положение
            {
                for (size_t q = 0; q < characteristic[i].deck + 2; q++)
                {
                    for (int w = -4; w < 2; w++)
                    {
                        ship_pos[characteristic[i].y - 2 + q][characteristic[i].x + w] = 2;
                    }
                }
                for (int g = 0; g < characteristic[i].deck; g++)
                {
                    for (int n = -2; n < 0; n++)
                    {
                        ship_pos[characteristic[i].y - 1 + g][characteristic[i].x + n] = 3;
                    }
                }
            }
            C++; // увеличиваем значение счётчика уничтоженных корабликов на 1
        }
    }
    if (C == 10 && plus == 0)
    {
        end = 1;//поражение
    }
    else if (C == 10 && plus == 10)
    {
        end = 2; //победа
    }
    else
    {
        C = 0; //онулируем счетчик уничтоженных корабликов
    }
}

void LoadCharactShips(Ships* characteristic) //присвоение палуб в соответствии с номером кораблика
{
    for (int i = 0; i <= 19; i++)
    {
        if (i == 0 || i == 10) //для 4-х палубных
        {
            characteristic[i].deck = 4;
        }
        else if ((i > 0 && i <= 2) || (i > 10 && i <= 12)) //для 3-х палубных
        {
            characteristic[i].deck = 3;
        }
        else if ((i > 2 && i <= 5) || (i > 12 && i <= 15)) //для 2-х палубных
        {
            characteristic[i].deck = 2;
        }
        else if ((i > 5 && i <= 9) || (i > 15 && i <= 19)) ///для 1-о палубных
        {
            characteristic[i].deck = 1;
        }
    }
}

void auto_arrangement(Ships* characteristic, int ship_pos[12][24], int plus)//расстановка корабликов
{
    int y, x, ship = plus; //переменная для координат и номера кораблика 
    while (ship < plus + 10)
    {
        characteristic[ship].horizontal = 0 + rand() % 2;
        y = 2 + rand() % 9, x = (1 + rand() % 9) * 2;
        if (characteristic[ship].horizontal == false && y + characteristic[ship].deck >= 13)//проверяем заденит ли кораблик границу по вертикале при перевороте, если его не отодвинуть
        {
            y -= y + characteristic[ship].deck - 12; //если да, то изменям его положение чтобы он не прошел сковзь границы поля по вертикали
        }
        else if (characteristic[ship].horizontal == true && x + characteristic[ship].deck * 2 >= 24)//проверяем заденит ли кораблик границу по вертикале при перевороте, если его не отодвинуть
        {
            x -= (x + characteristic[ship].deck * 2) - 22; //если да, то изменям его положение чтобы он не прошел сковзь границы поля по вертикали
        }
        if (characteristic[ship].horizontal == true && ship_pos[y - 1][x] == 0 && ship_pos[y - 1][x + characteristic[ship].deck * 2 - 2] == 0)
        {
            characteristic[ship].x = x + 2, characteristic[ship].y = y; //запоминаем координаты первой палубы у каждого кораблика, чтобы в дальнейшем правильно их взрывать
            for (size_t q = 0; q < 3; q++)
            {
                for (size_t w = 1; w < characteristic[ship].deck * 3 + 1; w++)
                {
                    ship_pos[y - 2 + q][x - 3 + w] = 1;
                }
            }
            for (int g = 0; g < 2 * characteristic[ship].deck; g++)
            {
                ship_pos[y - 1][x + g] = 13 - ship + plus;
            }
            ship++; //переходим к следующему кораблику
        }
        if (characteristic[ship].horizontal == false && ship_pos[y - 1][x] == 0 && ship_pos[y + characteristic[ship].deck - 2][x] == 0)
        {
            characteristic[ship].x = x + 2, characteristic[ship].y = y;
            for (size_t q = 0; q < characteristic[ship].deck + 2; q++)
            {
                for (size_t w = 0; w < 6; w++)
                {
                    ship_pos[y - 2 + q][x - 2 + w] = 1;
                }
            }
            for (int g = 0; g < characteristic[ship].deck; g++)
            {
                for (int n = 0; n < 2; n++)
                {
                    ship_pos[y - 1 + g][x + n] = 13 - ship + plus;
                }
            }
            ship++; //переходим к следующему кораблику
        }
    }
}


void CreatField(int speed, int X, int num_color) //Функция благодоря которой будет отрисовываться игровое поле
{
    SetCursor(X, 1);
    char mas[10] = { 'А','Б','В','Г','Д','Е','Ж','З','И','К' };
    setColor(White, Color(num_color));
    int A = 0, B = 0; //переменные необходимые для правильной отрисовки игрового поля
    cout << "  " << mas[0];
    for (size_t i = 0; i < 12; i++)
    {
        if (B == 0)
        {
            SetCursor(X + 3, 1 + i);
            B++;
        }
        else
        {
            SetCursor(X, 1 + i);
        }
        for (size_t g = 0; g < 23; g++)
        {
            if (i == 0 && g == 22)
            {
                setColor(Color(num_color), Color(num_color));
                Sleep(speed);
                cout << "+";
            }
            if (i == 0 && g > 0 && g < 10) //выполняем ниэеуказанный код, если мы на первой строке, и в диапозоне с 0-го по 10-ый элемент
            {
                setColor(White, Color(num_color));
                Sleep(speed);
                cout << " " << mas[g];
            }
            else if (i > 0 && i < 11 && g == 0) //если мы находимся с 0-ой по 11-ую строку на 0-м элементе
            {
                setColor(White, Color(num_color));
                if (i < 10)
                {
                    Sleep(speed);
                    cout << i << ' ';
                }
                else
                {
                    Sleep(speed);
                    cout << i;
                }
            }
            else if (i == 11 || g > 20 && g < 23) //Иначе если мы находимся на 11-ой строке, на 21-22-ых элементах
            {
                if (A == 0 && i == 11)
                {
                    Sleep(speed);
                    cout << "+";
                    A++;
                }
                setColor(Color(num_color), Color(num_color));
                Sleep(speed);
                cout << "+";
            }
            else if (i > 0) //если мы находимся на строке номер которой не равен "0"
            {
                Sleep(speed);
                setColor(White, White);
                cout << "+";
            }
        }
    }
    setColor(Black, Black);
}

void PaintPos(int mas[12][24], int x, int num_color) //прорисовка игровой карты в соответсвии с положением поставленных корабликов
{
    for (size_t i = 1; i < 11; i++)
    {
        int y = 1 + i;
        SetCursor(x, y);

        for (size_t g = 2; g < 22; g++)
        {
            if (mas[i][g] == 2)
            {
                setColor(LightGray, LightGray);
            }
            else if (mas[i][g] == 3)
            {
                setColor(Magenta, Magenta);
            }
            else if (mas[i][g] >= 4)
            {
                setColor(Color(num_color), Color(num_color));
            }
            else
            {
                setColor(White, White);
            }
            cout << "+";
        }
        cout << "\n";
    }
}

void Paint(char variant_1[2][30], int T) //меню выбора количества игроков
{
    system("cls");
    for (size_t i = 0; i < 2; i++)
    {
        SetCursor(30, 6 + i);
        for (size_t p = 0; p < 30; p++) 
        {
            if (i == T)
            {
                setColor(Black, White);
            }
            else
            {
                setColor(White, Black);
            }
            cout << variant_1[i][p];
        }
    }
    CreatField(0, 0, 9);
}

void Meny(int num)//вывод реплик
{
    char variant_2[14][40] = { "ВРАГ ДУМАЕТ КУДА УДАРИТЬ...","ВРАГ ПРОМАЗАЛ!", "ВРАГ ПОПАЛ!", "ВРАГ ПОБЕДИЛ... НЕ ГРУСТИ...", "ТВОЙ ХОД." , "ТЫ ПРОМАЗАЛ...", "ТЫ ПОПАЛ!", "ТЫ ПОБЕДИЛ, ПОЗДРАВЛЯЮ!", "+++++++++++++++++++++++++++++++++++++++", "ТЫ ПОПАЛ, ХОДИ СНОВА!", "ЮХУ, ТЫ УНИЧТОЖИЛ ВРАЖЕСКИЙ КОРАБЛИК!", "ИГРОК 1 ПОБЕДИЛ!", "ИГРОК 2 ПОБЕДИЛ!",""};
    SetCursor(22, 15);
    setColor(Black, Black);
    for (size_t i = 0; i < 25; i++)//стирание предыдущего текста
    {
        cout << variant_2[10][i];
        cout << variant_2[10][i];
    }
    SetCursor(22, 15);
    if (num > 0 && num < 4) // проверка на то к кому относятся реплики
    {
        setColor(Red, Black);
    }
    else //если репоики относятся к нам
    {
        setColor(Green, Black);
    }
    for (size_t i = 0; i < 30; i++) 
    {
        cout << variant_2[num][i];
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));
    hidecursor();
    system("mode con cols=60 lines=20");
    HWND hWnd;
    HMENU hm;
    hWnd = GetConsoleWindow();
    hm = GetSystemMenu(hWnd, FALSE);
    RemoveMenu(hm, SC_SIZE, MF_BYCOMMAND | MF_REMOVE);
    RemoveMenu(hm, SC_MAXIMIZE, MF_BYCOMMAND | MF_REMOVE);
    DrawMenuBar(hWnd);

    char variant_1[2][30] = { "ИГРА ДЛЯ ДВОИХ", "ИГРА ДЛЯ ОДНОГО" };
    int ship_pos[12][24] = { 0 };
    int enemy_ship_pos[12][24] = { 0 };
    Ships* characteristic = new Ships[20]; //создаём 10-ть корабликов
    LoadCharactShips(characteristic); 
    int x = 2, y = 2, key = 1, change = 1, num = 0, num2 = 0, A = 0, xx = 37, yy = 2, over = 0, xxx = 2, yyy = 2, xv = 22, yv = 2;
    bool number_players = true, start = false, your_turn = true, turn_first = true, alive = false;
    Avake();
    Sleep(1500); //ждём 1.5 секунды
    system("cls");
    Paint(variant_1, change);
    do
    {
        key = _getch(); //получаем номер нажатой клавиши
        switch (key)
        {
        case Up:
            if (y > 0)
            {
                y--;
                change = y;
                Paint(variant_1, change);
            }
            break;
        case Down:
            if (y < 1)
            {
                y++;
                change = y;
                Paint(variant_1, change);
            }
            break;
        case Enter:
            do
            {
                switch (change)
                {
                case 0:
                    number_players = false; // игра для двоих
                    A++; //чтобы заврешить цикл
                    break;
                case 1:
                    number_players = true; //игра для 1
                    A++; //чтобы заврешить цикл
                    y++; //прибавляем к значению по "y" 1
                    break;
                }
            } while (key != Enter);
        }
    } while (A < 1);

    system("cls");
    CreatField(0, 0, 9);
    x = 2, y = 2;
    if (number_players == true) {
       
        auto_arrangement(characteristic, ship_pos, 0);
        PaintPos(ship_pos, 2, 0);
        start = true;
        Sleep(20);
        CreatField(1, 35, 4);
        auto_arrangement(characteristic, enemy_ship_pos, 10);
        PaintPos(enemy_ship_pos, 37, 15);
        do
        {
            if (your_turn == true) //если наша очередь атаковать
            {
                do
                {
                    x = xx, y = yy; //запоминаем гед курсор
                    PaintPos(enemy_ship_pos, 37, 15);
                    setColor(DarkGray, DarkGray);
                    SetCursor(x, y);
                    cout << "++";
                    key = _getch();
                    switch (key)
                    {
                    case Left:
                        if (x > 38)
                        {
                            x -= 2;
                            xx = x; 
                        }
                        break;
                    case Right:
                        if (x < 55)
                        {
                            x += 2;
                            xx = x;
                        }
                        break;
                    case Up:
                        if (y > 2)
                        {
                            y--;
                            yy = y;
                        }
                        break;
                    case Down:
                        if (y < 11)
                        {
                            y++;
                            yy = y;
                        }
                        break;
                    case Enter:
                        if (enemy_ship_pos[y - 1][x - 35] >= 4)
                        {
                            for (size_t i = 0; i < 2; i++)
                            {
                                enemy_ship_pos[y - 1][x - 35 + i] = 3;
                                hit(characteristic, enemy_ship_pos, 10, over);
                            }
                            Meny(9);
                            Sleep(500);
                        }
                        else if (enemy_ship_pos[y - 1][x - 35] < 2)
                        {
                            for (size_t i = 0; i < 2; i++)
                            {
                                enemy_ship_pos[y - 1][x - 35 + i] = 2;
                            }
                            Sleep(500);
                            Meny(5);
                            your_turn = false; //очередь атаки противника
                        }
                        break;
                    }
                } while (key != Enter);
            }
            if (your_turn == false) //если ход противника
            {
                Sleep(0 + rand() % 400);
                Meny(0);
                PaintPos(ship_pos, 2, 0);
                Sleep(0 + rand() % 400);
                xv = (2 + rand() % 10) * 2, yv = (2 + rand() % 10);
                if (ship_pos[yv - 1][xv - 1] >= 4) {
                    for (size_t i = 0; i < 2; i++) {
                        ship_pos[yv - 1][xv - 2 + i] = 3;
                        hit(characteristic, ship_pos, 0, over);
                    }
                    Meny(2);
                    Sleep(0 + rand() % 750);
                }
                else if (ship_pos[yv - 1][xv - 1] <= 2) 
                {
                    for (size_t i = 0; i < 2; i++)
                    {
                        ship_pos[yv - 1][xv - 2 + i] = 2;
                    }
                    Meny(1);
                    PaintPos(ship_pos, 2, 0);
                    Sleep(0 + rand() % 750);
                    your_turn = true;
                }
            }
        } while (over == 0);
        PaintPos(ship_pos, 2, 0);
        PaintPos(enemy_ship_pos, 37, 15);
        if (over == 2)
        {
            Meny(7);//победа
        }
        else
        {
            Meny(3);
        }
        Sleep(500000);
    }
    else if (number_players == false) {
        Sleep(2000);
        auto_arrangement(characteristic, ship_pos, 0);
        PaintPos(ship_pos, 2, 0);
        start = true;
        Sleep(2000);
        PaintPos(ship_pos, 2, 15);
        CreatField(0, 35, 4);
        Sleep(2000);
        auto_arrangement(characteristic, enemy_ship_pos, 10);
        PaintPos(enemy_ship_pos, 37, 0);
        start = true;
        Sleep(2000);
        PaintPos(enemy_ship_pos, 37, 15);
        Sleep(2000);
        do
        {
            if (turn_first == true) //атака первого игрока
            {
                PaintPos(enemy_ship_pos, 37, 15);
                Sleep(2000);
                do
                {
                    x = xx, y = yy;

                    PaintPos(ship_pos, 2, 0);
                    PaintPos(enemy_ship_pos, 37, 15);
                    setColor(DarkGray, DarkGray);
                    SetCursor(x, y);
                    cout << "++";
                    key = _getch();
                    switch (key)
                    {
                    case Left:
                        if (x > 38)
                        {
                            x -= 2;
                            xx = x;
                        }
                        break;
                    case Right:
                        if (x < 55)
                        {
                            x += 2;
                            xx = x;
                        }
                        break;
                    case Up:
                        if (y > 2)
                        {
                            y--;
                            yy = y;
                        }
                        break;
                    case Down:
                        if (y < 11)
                        {
                            y++;
                            yy = y;
                        }
                        break;
                    case Enter:
                        if (enemy_ship_pos[y - 1][x - 35] >= 4)
                        {
                            for (size_t i = 0; i < 2; i++)
                            {
                                enemy_ship_pos[y - 1][x - 35 + i] = 3;
                                hit(characteristic, enemy_ship_pos, 10, over);
                            }
                            Sleep(500);
                        }

                        else if (enemy_ship_pos[y - 1][x - 35] < 2) 
                        {
                            for (size_t i = 0; i < 2; i++)
                            {
                                enemy_ship_pos[y - 1][x - 35 + i] = 2;
                            }
                            Sleep(500);
                            Meny(5);
                            Sleep(500);
                            Meny(13);
                            turn_first = false;
                        }
                        break;
                    }
                } while (key != Enter);
            }
            if (turn_first == false) //ход второго игрока
            {
                PaintPos(ship_pos, 2, 15);
                Sleep(2000);
                do 
                {
                    x = xxx, y = yyy;
                    PaintPos(enemy_ship_pos, 37, 0);
                    PaintPos(ship_pos, 2, 15);
                    setColor(DarkGray, DarkGray);
                    SetCursor(x, y); 
                    cout << "++";
                    key = _getch();
                    switch (key) 
                    {
                    case Left: 
                        if (x > 3)
                        {
                            x -= 2;
                            xxx = x;
                        }
                        break;
                    case Right: 
                        if (x < 20)
                        {
                            x += 2;
                            xxx = x;
                        }
                        break;
                    case Up: 
                        if (y > 2) 
                        {
                            y--;  
                            yyy = y; 
                        }
                        break; 
                    case Down: 
                        if (y < 11) 
                        {
                            y++;  
                            yyy = y; 
                        }
                        break;
                    case Enter:
                        x = x + 2;
                        if (ship_pos[y - 1][x - 1] >= 4) 
                        {
                            for (size_t i = 0; i < 2; i++)
                            {
                                ship_pos[y - 1][x - 2 + i] = 3; 
                                hit(characteristic, ship_pos, 0, over);
                            } 
                            Sleep(0 + rand() % 750);
                        }
                        else if (ship_pos[y - 1][x - 1] <= 2)  
                        {
                            for (size_t i = 0; i < 2; i++) 
                            {
                                ship_pos[y - 1][x - 2 + i] = 2; 
                            }
                            Meny(5); 
                            Sleep(0 + rand() % 750); 
                            Meny(13);
                            turn_first = true; 
                        }
                        break;
                    }
                } while (key != Enter);
            }
        } while (over == 0);
        PaintPos(ship_pos, 2, 0);
        PaintPos(enemy_ship_pos, 37, 15); 
        if (over == 2) //игрока 1
        {
            Meny(11);
        }
        else
        {
            Meny(12);
        }
        Sleep(500000);
    }
}
