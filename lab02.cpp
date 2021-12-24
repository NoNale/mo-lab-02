#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

class Simplex {
private:
	double table[4][4]; // Таблица
	int base_variable[3] = { 1, 2, 3 }; // Базисная переменная
	int free_variable[3] = { 4, 5, 6 }; // Свободная переменная
	double min = -1;
	int pivot_col = 0, pivot_row = 0; // Разрешающие столбец и строка
	bool flag = true; // Флаг для проверки существования следующей итерации

	void transpose() {
		for (size_t i = 0, j = 1; i < 4 && j < 4; i++, j++) {
			std::swap(table[i][0], table[3][j]);
			table[i][0] = -table[i][0];
			table[3][j] = -table[3][j];
		}
		double A[3][3];
		for (size_t i = 0, i_t = 0; i < 3 && i_t < 3; i++, i_t++) {
			for (size_t j = 0, j_t = 1; j < 3 && j_t < 4; j++, j_t++) {
				A[i][j] = -table[i_t][j_t];
			}
		}
		for (size_t i = 0, i_t = 0; i < 3 && i_t < 3; i++, i_t++) {
			for (size_t j = 0, j_t = 1; j < 3 && j_t < 4; j++, j_t++) {
				table[i_t][j_t] = A[j][i];
			}
		}
	}


	// Функция для загрузки значений из файла
	void load_from_file() {
		std::ifstream in("D:\\GitHub\\MO\\lab02\\lptask2.txt");
		for (int i = 0; i < 3; ++i) {
			in >> table[3][i + 1];
		}
		table[3][0] = 0;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				in >> table[i][j + 1];
			}
		}
		for (int i = 0; i < 3; ++i) {
			in >> table[i][0];
		}
		in.close();
	}

	// Функция для вывода таблицы
	void print_table() {
		std::cout << std::setw(10) << "S";
		for (int i = 0; i < 3; i++)
		{
			std::cout << std::setw(8) << "x" << base_variable[i];
		}
		std::cout << std::endl;
		for (int i = 0; i < 4; ++i)
		{
			if (i >= 3) {
				std::cout << "F ";
			}
			else {
				std::cout << "x" << free_variable[i];
			}
			for (int j = 0; j < 4; ++j)
			{
				std::cout << std::setw(8) << std::setprecision(3) <<
					table[i][j] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	// Функция которая меняет местами разреш. столб и строку + преобразует таблицу
	void jordan_exception() {
		Simplex update(*this);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i != pivot_row && j != pivot_col) //S*(ij)
					table[i][j] = update.table[i][j] - update.table[i][pivot_col] * update.table[pivot_row][j] / update.table[pivot_row][pivot_col];
				else if (i != pivot_row) //S*(jk)
					table[i][j] = -1 * update.table[i][j] /
					update.table[pivot_row][pivot_col];
				else if (j != pivot_col) //S*(rj)
					table[i][j] = update.table[i][j] /
					update.table[pivot_row][pivot_col];
				else //S*(rk)
					table[i][j] = 1 / update.table[i][j];
			}
		}
		std::swap(base_variable[pivot_col - 1], free_variable[pivot_row]);
	}

	//Функция ищущая первый негативный элемент в столбце свободных членов = pivot row
	void find_first_negative() {
		for (int i = 0; i < 3; ++i)
		{
			if (table[i][0] < 0)
			{
				pivot_row = i;
				break;
			}
		}
	}

	//Функция ищущая первый позитивный элемент в строке = pivot col
	void find_first_positive() {
		for (int i = 1; i < 4; ++i)
		{
			if (table[3][i] > 0)
			{
				pivot_col = i;
				break;
			}
		}
	}

	void pivot_change_for_negative() {
		while (flag)
		{
			pivot_row = -1;
			pivot_col = -1;
			min = -1;
			find_first_negative();
			if (pivot_row >= 0)
			{
				flag = false;
				for (int i = 1; i < 4; ++i)
				{
					if (table[pivot_row][i] < 0)
					{
						pivot_col = i;
						flag = true;
						break;
					}
				}
				if (flag)
				{
					for (int i = 0; i < 3; ++i)
					{
						if (((table[i][0] / table[i][pivot_col]) > 0) &&
							((min < 0) || (table[i][0] / table[i][pivot_col] < min)))
						{
							min = table[i][0] / table[i][pivot_col];
							pivot_row = i;
						}
					}
					std::cout << "Found Pivot Number: " << table[pivot_row][pivot_col] << std::endl;
					std::cout << "Pivot Row: " << pivot_row << std::endl;
					std::cout << "Pivot Col: " << pivot_col << std::endl;
					std::cout << std::endl;
					jordan_exception();
					print_table();
				}
			}
			else break;
		}
		if (!flag) std::cout << "No Solution" << std::endl;
	}

	void pivot_change_for_positive() {
		bool exist = true;
		while (exist)
		{
			pivot_col = -1;
			min = -1;
			find_first_positive();
			if (pivot_col >= 0)
			{
				//exist = false;
				for (int i = 0; i < 3; ++i) {
					if (table[i][pivot_col] > 0)
					{
						pivot_row = i;
						exist = true;
						break;
					}
				}
				if (exist)
				{
					for (int i = 0; i < 3; ++i)
					{
						if (((table[i][0] / table[i][pivot_col]) > 0) &&
							((min < 0) || (table[i][0] / table[i][pivot_col] < min)))
						{
							min = table[i][0] / table[i][pivot_col];
							pivot_row = i;
						}
					}
					std::cout << "Found Pivot Number: " << table[pivot_row][pivot_col] << std::endl;
					std::cout << "Pivot Row: " << pivot_row << std::endl;
					std::cout << "Pivot Col: " << pivot_col << std::endl;
					std::cout << std::endl;
					jordan_exception();
					print_table();
				}
			}
			else break;
		}
		if (!exist) std::cout << "No Solution" << std::endl;
	}

	void find_optimal_solution() {
		pivot_change_for_negative();
		if (flag) {
			pivot_change_for_positive();
		}
		if (flag) {
			std::cout << "\nSolution:" << std::endl;
			std::cout << "F  = " << table[3][0] * (-1) << std::endl;
			for (int i = 0; i < 3; ++i)
			{
				std::cout << "x" << base_variable[i] << " = 0" << std::endl;
			}
			for (int i = 0; i < 3; ++i)
			{
				std::cout << "x" << free_variable[i] << " = " << table[i][0] << std::endl;
			}
		}
	}

public:
	Simplex() = default;

	Simplex(const Simplex& simplex) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				table[i][j] = simplex.table[i][j];
			}
		}
	}

	// Симплекс метод
	void simplex_method() {
		load_from_file(); // Получаем данные из файла
		transpose();
		print_table(); // Печатаем нулевую итерацию
		find_optimal_solution(); // Печатаем оптимальное решение
	}
};

int main() {
	Simplex simplex;
	simplex.simplex_method();
	return 0;
}

