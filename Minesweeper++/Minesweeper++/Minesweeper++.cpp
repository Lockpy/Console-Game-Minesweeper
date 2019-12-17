#include <iostream>
#include <time.h>
#include <windows.h>
#include "conio.h"
#include <ctime>

using namespace std;

void wellcome(HANDLE hConsole); //Заставка игры

void kyrsor_menu(HANDLE hConsole, int x, int z); //Отображения курсора в меню

void visual_menu(HANDLE hConsole, int x); //Интерфейс меню

void visual_set(HANDLE hConsole, int x); //Интерфейс настроек

int menu(HANDLE hConsole);  //Меню

bool mine(int i, int j, int** field, int field_height, int field_weight); //Проверка на мину

void input(int** field, bool** open, int field_height, int field_weight, int mine_count); //Заполнения поля

void kyrsor(HANDLE hConsole, int x, int y, int i, int j); //Отображения курсора на поле

bool checklose(int** field, bool** open, int field_height, int field_weight); //Отображения курсора на поле

bool checkwin(int** field, bool** open, int field_height, int field_weight, int mine_count); //Проверка на победу

void visual_field(HANDLE hConsole, int x, int y, int chet_min, int** field, bool** open, int field_height, int field_weight, int mine_count); //Проверка на проигрыш

void clean(int x, int y, int** field, bool** open, int field_height, int field_weight); //Рекурсивная ф-ция по очистке поля

bool otmena_flaga(int x, int y, int** field); //Снятие флага с клетки

void gameplay(HANDLE hConsole, int** field, bool** open, int field_height, int field_weight, int mine_count); //Главная игровая ф-ция

void main() {
	srand(time(0));
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	setlocale(LC_ALL, "rus");
	wellcome(hConsole);
	int field_height = 9, field_weight = 9;//Обвъявление параметров размера поля
	int mine_count = 10;//Обвъявление количества мин
	int** field = nullptr;//Обвъявление игрового поля
	bool** open = nullptr;//Обвъявление логического поля
	for (;;) {   //Игровой цикл
		int menu_c = menu(hConsole);
		if (menu_c == 2)break; //Выход из игры
		field_height = 9, field_weight = 9, mine_count = 10;//Параметры поля по умолчанию
		if (menu_c == 3) {  //Режим Любитель
			field_height = 16, field_weight = 16, mine_count = 40;
		}
		if (menu_c == 4) {  //Режим Профессионал
			field_height = 16, field_weight = 30, mine_count = 90;
		}
		if (menu_c == 5) {  //Кастомный Режим
			do {
				system("cls");
				cout << "Введите высоту поля: ";
				cin >> field_height;
				cout << "Введите ширину поля: ";
				cin >> field_weight;
				cout << "Введите количество мин: ";
				cin >> mine_count;
			} while ((field_height * field_weight) < mine_count);
		}
		unsigned int start_time = clock();//Начало отчета времени
		field = new int* [field_height]; //Иницилизация игрового поля
		for (int i = 0; i < field_height; i++)
			field[i] = new int[field_weight];
		open = new bool* [field_height]; //Иницилизация логического поля
		for (int i = 0; i < field_height; i++)
			open[i] = new bool[field_weight];

		input(field, open, field_height, field_weight, mine_count);       //Функция заполнения
		gameplay(hConsole, field, open, field_height, field_weight, mine_count);  //Главная игровая функция
		SetConsoleOutputCP(1251);
		if (!checkwin(field, open, field_height, field_weight, mine_count))if (checklose(field, open, field_height, field_weight))cout << "\t\t\t   ВЫ ПРОИГРАЛИ :(\n";  //Проверка на победу
		if (checkwin(field, open, field_height, field_weight, mine_count)) {
			cout << "\t\t\t   ВЫ ВЫИГРАЛИ :)\n";
			unsigned int end_time = clock();
			unsigned int search_time = (end_time - start_time) / 1000;
			int min = search_time / 60;
			int sec = search_time - (60 * min);
			cout << "\t\t\t   Ваше время - " << min << ":" << sec << endl;
		}//Проверка на проигрыш
		_getch();
	}
	for (int i = 0; i < field_height; i++) { //Очистка памяти от игрового поля
		delete[] field[i];
	}
	delete[] field;
	for (int i = 0; i < field_height; i++) { //Очистка памяти от логического поля
		delete[] open[i];
	}
	delete[] open;
}

void gameplay(HANDLE hConsole, int** field, bool** open, int field_height, int field_weight, int mine_count) {     //Главная игровая функция
	int x = 0, y = 0, k, chet_min = mine_count;//Инициализация координат и переменой хода
	bool flag;//Инициализация флага
	for (;;) {
		system("cls");
		visual_field(hConsole, x, y, chet_min, field, open, field_height, field_weight, mine_count);
		if (checklose(field, open, field_height, field_weight))break;
		if (checkwin(field, open, field_height, field_weight, mine_count))break;
		SetConsoleOutputCP(1251);
		for (;;) {
			k = _getch();    //Ввод стрелки с клавиатуры
			if (k == 0 || k == 224)k = _getch();
			if (k == 72 && (y - 1 >= 0) && (y - 1 < field_height)) { y -= 1;break; }//Стрелка Вверх
			if (k == 80 && (y + 1 >= 0) && (y + 1 < field_height)) { y += 1;break; }//Стрелка Вниз
			if (k == 75 && (x - 1 >= 0) && (x - 1 < field_weight)) { x -= 1;break; }//Стрелка Влево
			if (k == 77 && (x + 1 >= 0) && (x + 1 < field_weight)) { x += 1;break; }//Стрелка Вправо
			if (k == 32) {
				if (otmena_flaga(x, y, field)) {
					chet_min++;
					break;
				}
				clean(x, y, field, open, field_height, field_weight);
				break;
			}//Пробел - Выстрел
			flag = false;
			if (k == 13) {  //Enter - Флаг на мину
				if (otmena_flaga(x, y, field))
				{
					chet_min++;
					break;
				}
				else chet_min--;
				flag = otmena_flaga(x, y, field);
				//Отмена флага
				if (field[y][x] == 9 && flag == false) field[y][x] = 10;//Если флаг установлен на мину,ставим на её место 10
				if (field[y][x] >= 0 && field[y][x] < 9 && flag == false)field[y][x] += 11; //Если флаг установлен ошибочно,ставим на место флага 11-19(0-8 соответственно)
				flag = false;
				break;
			}
		}
	}
}

void visual_field(HANDLE hConsole, int x, int y, int chet_min, int** field, bool** open, int field_height, int field_weight, int mine_count) {  //Функция отвечает за визуальную часть игры
	SetConsoleOutputCP(866);
	/*for (int i = 0;i < field_height;i++) {
	for (int j = 0;j < field_weight;j++)cout << field[i][j] << " ";     Тестовый вывод поля
	cout << endl;
	}*/
	cout << "\t\t       " << chet_min << endl;
	//Рисуем верх рамки
	cout << "\t\t       " << (char)201;
	for (int i = 0;i < (field_weight * 2) + 1;i++)cout << (char)205;
	cout << (char)187 << endl;
	//Закончили рисовать верх рамки
	for (int i = 0;i < field_height;i++) {
		cout << "\t\t       " << (char)186 << " ";
		for (int j = 0;j < field_weight;j++) {
			if (checklose(field, open, field_height, field_weight) || checkwin(field, open, field_height, field_weight, mine_count)) if (field[i][j] == 9 && !open[i][j])open[i][j] = true;
			if (open[i][j] && field[i][j] == 0) {
				SetConsoleTextAttribute(hConsole, 15);
				kyrsor(hConsole, x, y, i, j);
				cout << (char)249 << " ";
				continue;
			}
			if (open[i][j] && field[i][j] == 1) {
				SetConsoleTextAttribute(hConsole, 11);
				kyrsor(hConsole, x, y, i, j);
				cout << "1 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 2) {
				SetConsoleTextAttribute(hConsole, 2);
				kyrsor(hConsole, x, y, i, j);
				cout << "2 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 3) {
				SetConsoleTextAttribute(hConsole, 12);
				kyrsor(hConsole, x, y, i, j);
				cout << "3 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 4) {
				SetConsoleTextAttribute(hConsole, 1);
				kyrsor(hConsole, x, y, i, j);
				cout << "4 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 5) {
				SetConsoleTextAttribute(hConsole, 4);
				kyrsor(hConsole, x, y, i, j);
				cout << "5 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 6) {
				SetConsoleTextAttribute(hConsole, 3);
				kyrsor(hConsole, x, y, i, j);
				cout << "6 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 7) {
				SetConsoleTextAttribute(hConsole, 14);
				kyrsor(hConsole, x, y, i, j);
				cout << "7 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 8) {
				SetConsoleTextAttribute(hConsole, 13);
				kyrsor(hConsole, x, y, i, j);
				cout << "8 ";
				continue;
			}
			if (open[i][j] && field[i][j] == 9) {
				SetConsoleTextAttribute(hConsole, 12);
				kyrsor(hConsole, x, y, i, j);
				cout << "X ";
				continue;
			}
			if (!open[i][j] && field[i][j] == 10) {
				SetConsoleTextAttribute(hConsole, 10);
				kyrsor(hConsole, x, y, i, j);
				cout << "? ";
				continue;
			}
			if (field[i][j] > 10 && field[i][j] < 20) {
				if (!checklose(field, open, field_height, field_weight))SetConsoleTextAttribute(hConsole, 10);
				else SetConsoleTextAttribute(hConsole, 12);
				kyrsor(hConsole, x, y, i, j);
				cout << "? ";
				continue;
			}
			if (!open[i][j]) {
				SetConsoleTextAttribute(hConsole, 15);
				kyrsor(hConsole, x, y, i, j);
				cout << (char)254 << " ";
			}
		}
		SetConsoleTextAttribute(hConsole, 15);
		cout << (char)186 << endl;
	}
	//Рисуем низ рамки
	cout << "\t\t       " << (char)200;
	for (int i = 0;i < (field_weight * 2) + 1;i++)cout << (char)205;
	cout << (char)188 << endl;
	//Закончили рисовать них рамки
}

void clean(int x, int y, int** field, bool** open, int field_height, int field_weight) { //Рекурсивная функция по очистке поля
																						 //Проверка навыход за пределы массива
	if ((y >= 0) && (y < field_height)) {
		if ((x >= 0) && (x < field_weight)) {
			if (!open[y][x] && field[y][x] < 10) {
				open[y][x] = true;
				if (field[y][x] == 0) {
					clean(x - 1, y, field, open, field_height, field_weight);
					clean(x, y - 1, field, open, field_height, field_weight);
					clean(x, y + 1, field, open, field_height, field_weight);
					clean(x + 1, y, field, open, field_height, field_weight);
				}
			}
		}
	}

}

bool checklose(int** field, bool** open, int field_height, int field_weight) {  //Функция проверки на проигрыш 
	for (int i = 0;i < field_height;i++)
		for (int j = 0;j < field_weight;j++)if (field[i][j] == 9 && open[i][j]) return true;
	return false;
}

bool checkwin(int** field, bool** open, int field_height, int field_weight, int mine_count) {  //Функция проверки на победу
	int x = field_height * field_weight - mine_count;
	for (int i = 0;i < field_height;i++)
		for (int j = 0;j < field_weight;j++)if (field[i][j] != 9 && open[i][j])x--;
	if (x == 0)return true;
	return false;
}

bool otmena_flaga(int x, int y, int** field) {
	if (field[y][x] == 10) {  //Отмена флага с мины
		field[y][x] = 9;
		return  true;
	}
	if (field[y][x] > 10 && field[y][x] < 20) {  //Отмена флага с обычной клетки
		field[y][x] -= 11;
		return true;
	}
	return false;
}

void wellcome(HANDLE hConsole) {//Функция выводит зазставку игры

	int last = 0;
	for (int j = 0;j < 2;j++)
		for (int i = 7;i < 15;i++) {
			SetConsoleTextAttribute(hConsole, i + 1);
			if (i % 2 == 0)cout << "\n\n\n\n\n\n\n\n\n\n\n\t\t\t    -_-_-_-_-_-_-_-_-\n";
			else cout << "\n\n\n\n\n\n\n\n\n\n\n\t\t\t    _-_-_-_-_-_-_-_-_\n";
			if (i % 2 == 0)SetConsoleTextAttribute(hConsole, 4);
			else SetConsoleTextAttribute(hConsole, 0);
			cout << "\t\t\t    X   ";
			SetConsoleTextAttribute(hConsole, i);
			cout << "::САПЕР::";
			if (i % 2 == 0)SetConsoleTextAttribute(hConsole, 4);
			else SetConsoleTextAttribute(hConsole, 0);
			cout << "   X\n";
			SetConsoleTextAttribute(hConsole, i + 1);
			if (i % 2 == 0)cout << "\t\t\t    _-_-_-_-_-_-_-_-_\n\n\t\t      by KhomenkoGames v.1.0(27.05.2017)";
			else cout << "\t\t\t    -_-_-_-_-_-_-_-_-\n\n\t\t      by KhomenkoGames v.1.0(26.05.2017)";
			Sleep(100);
			last++;
			if (last < 16)system("cls");
		}
	cout << "\n\n\t\t\tНажмите чтобы продолжить...\n";
	SetConsoleTextAttribute(hConsole, 0);
	system("pause");
	system("cls");
	SetConsoleTextAttribute(hConsole, 15);
}

void kyrsor_menu(HANDLE hConsole, int x, int z) {
	SetConsoleTextAttribute(hConsole, 15);
	if (x == z)SetConsoleTextAttribute(hConsole, 13);
}

void visual_menu(HANDLE hConsole, int x) {
	kyrsor_menu(hConsole, x, 0);                            //Отображение меню
	cout << "\n\n\n\n\n\n\n\t\t       ИГРАТЬ\n" << endl;
	kyrsor_menu(hConsole, x, 1);
	cout << "\t\t       НАСТРОЙКИ\n" << endl;
	kyrsor_menu(hConsole, x, 2);
	cout << "\t\t       ВЫХОД" << endl;
	SetConsoleTextAttribute(hConsole, 15);
}

void visual_set(HANDLE hConsole, int x) {
	system("cls");
	cout << "\n\n\n\n\n\n\n\t\t       Выберите размер поля:\n" << endl;
	kyrsor_menu(hConsole, x, 0);                           //Отображение меню
	cout << "\t\t       Новичек(9x9,10 мин)\n" << endl;
	kyrsor_menu(hConsole, x, 1);
	cout << "\t\t       Любитель(16x16,40 мин)\n" << endl;
	kyrsor_menu(hConsole, x, 2);
	cout << "\t\t       Профессионал(16x30,90 мин)\n" << endl;
	kyrsor_menu(hConsole, x, 3);
	cout << "\t\t       Кастомный размер\n" << endl;
	kyrsor_menu(hConsole, x, 4);
	cout << "\t\t       НАЗАД\n" << endl;
	SetConsoleTextAttribute(hConsole, 15);
}

int menu(HANDLE hConsole) {
	int x = 0, k, mode = 0, gran = 2;//Координата меню/Переменная хода
	for (;;) {
		system("cls");
		if (mode == 0)visual_menu(hConsole, x);
		if (mode == 1) visual_set(hConsole, x);
		for (;;) {
			if (mode == 1)gran = 4;
			if (mode == 0)gran = 2;
			k = _getch();    //Ввод стрелки с клавиатуры
			if (k == 0 || k == 224)k = _getch();
			if (k == 72 && x - 1 >= 0) { x -= 1;break; }//Стрелка Вверх
			if (k == 80 && x + 1 <= gran) { x += 1;break; }//Стрелка Вниз
			if (k == 13 && mode == 0) {                      //Выбор пункта меню
				if (x == 0)return 0;
				if (x == 1) {
					mode = 1;
					x = 0;
					break;
				}
				if (x == 2)return 2;
			}
			if (k == 13 && mode == 1) {
				if (x == 0)return 0;
				if (x == 1)return 3;
				if (x == 2)return 4;
				if (x == 3)return 5;
				if (x == 4) {
					mode = 0;
					x = 0;
					break;
				}
			}
		}
	}
}

bool mine(int i, int j, int** field, int field_height, int field_weight) {
	if ((i >= 0) && (i < field_height)) {
		if ((j >= 0) && (j < field_weight)) {
			if (field[i][j] == 9) return true;
		}
	}
	return false;
}

void input(int** field, bool** open, int field_height, int field_weight, int mine_count) {  //Функция заполняет поле
	int bomb_x, bomb_y, k = 0;

	for (int i = 0;i < field_height;i++)for (int j = 0;j < field_weight;j++) {
		field[i][j] = 0;
		open[i][j] = false;
	}    //Заполнение поля нулями/false'м

	for (int i = 0;i < mine_count;) {                           //Выбор разположения *count_mine* бомб
		bomb_y = rand() % field_height;
		bomb_x = rand() % field_weight;
		if (field[bomb_y][bomb_x] == 0) {

			field[bomb_y][bomb_x] = 9;
			i++;
		}
	}                                                //Конец выбор
	for (int i = 0;i < field_height;i++) {     //Заполнение поля цифрами
		for (int j = 0;j < field_weight;j++) {
			if (field[i][j] != 9) {
				k = 0;
				if (mine(i - 1, j - 1, field, field_height, field_weight)) k++;
				if (mine(i - 1, j, field, field_height, field_weight)) k++;
				if (mine(i - 1, j + 1, field, field_height, field_weight)) k++;
				if (mine(i, j - 1, field, field_height, field_weight)) k++;
				if (mine(i, j + 1, field, field_height, field_weight)) k++;
				if (mine(i + 1, j - 1, field, field_height, field_weight)) k++;
				if (mine(i + 1, j, field, field_height, field_weight)) k++;
				if (mine(i + 1, j + 1, field, field_height, field_weight)) k++;
				field[i][j] = k;
			}
		}
	}       //Конец заполнения
}

void kyrsor(HANDLE hConsole, int x, int y, int i, int j) {
	if (y == i && x == j)SetConsoleTextAttribute(hConsole, 13);
}