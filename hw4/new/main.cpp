/* 6 вариант
 * Вычислить прямое произведение множеств А1, А2, А3, А4.
 * Входные данные: множества чисел А1, А2, А3, А4,
 * мощности множеств могут быть не равны между собой и мощность
 * каждого множества больше или равна 1. Количество потоков является входным
 * параметром. входные параметры: файл вида
 *
 * A1 A2 ... An
 * B1 B2 ... Bm
 * C1 C2 ... Ck
 * D1 D2 ... Dl
 * <num_of_threads>
 *
 * где
 * Ai - элементы первого множества, целые
 * Bi - элементы второго множества, целые
 * Ci - элементы третьего множества, целые
 * Di - элементы четвертого множества, целые
 * <num_of_threads> - количество потоков, целые
 */

#include "omp.h"
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;
vector<vector<int>> vecs;//вектор, состоящий из данных векторов А, B, C, D
int num_of_threads;//количество потоков

/// Метод для параллельного вычисления прямого произведения множеств
/// из вектора vecs
/// \param result - результирующий вектор, состоящий из векторов длины 4 (элементы прямого произведения)
/// \param num_of_threads - заданное количество потоков
void mult(vector<vector<int>> &result, int num_of_threads) {
    int i, j, k, l;
    //директива для разделения итераций циклов между потоками
    double time = omp_get_wtime();
    #pragma omp parallel for num_threads(num_of_threads) collapse(4) private(i,j,k,l)
    for (i = 0; i < vecs[0].size(); ++i) {
        for (j = 0; j < vecs[1].size(); ++j) {
            for (k = 0; k < vecs[2].size(); ++k) {
                for (l = 0; l < vecs[3].size(); ++l) {
                    //критическая секция для записи
                    #pragma omp critical
                    {
                        result.push_back({vecs[0][i], vecs[1][j], vecs[2][k], vecs[3][l]});
                    }
                }
            }
        }
    }
    cout << "Time = " << omp_get_wtime() - time;
}


/// Метод переводящий строку в число
/// \param line - строка
/// \param num - число, в которое нужно записать строку
/// \return - 1, если конвертация прошла успешно, иначе - 0
bool string_to_int(string line, int &num) {
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] > '9' ||
            line[i] < '0')               //если в строке что-то кроме цифр, то это не число
            if (line[i] == '-' && i == 0)//если число отрицательное
                continue;
            else
                return 0;
    }
    num = atoi(line.c_str());
    return 1;
}

/// Метод для считывания входных данных из файла
/// \param path - путь к файлу
/// \param vecs - вектор, в который нужно записать полученные множества
void read_data(string path, vector<vector<int>> &vecs) {
    try {
        ifstream in(path);
        string line{};
        string number{};
        int ind = -1;//индекс множества ([0;3])
        while (getline(in, line)) {
            stringstream strStream(line);
            ind++;
            while (getline(strStream, number, ' ')) {//разделение через пробел
                int num;
                if (!string_to_int(number, num)) {//если в файле есть другие символы
                    //кроме цифр, то он неверный
                    throw exception();
                }
                vecs[ind].push_back(num);
            }
            if (ind == 3)//если считано 4 множества
                break;
        }
        for (int i = 0; i < vecs.size(); ++i) {
            if (vecs[i].size() == 0)
                throw exception();
        }

        string k;
        in >> k;//считывание количества потоков
        num_of_threads = atoi(k.c_str());
        in.close();
    } catch (const std::exception &err) {
        num_of_threads = 0;
        vecs = vector<vector<int>>(0);
        cout << "Error while reading the file." << endl;
        cout << "Check the correctness of input data" << endl;
        cout << "input data in file: " << endl
             << "A1 A2 ... An" << endl
             << "B1 B2 ... Bm" << endl
             << "C1 C2 ... Ck" << endl
             << "D1 D2 ... Dl" << endl
             << "<num_of_threads>" << endl
             << "where" << endl
             << "Ai - elements of the first multiplicity (integer)" << endl
             << "Bi - elements of the second multiplicity (integer)" << endl
             << "Ci - elements of the third multiplicity (integer)" << endl
             << "Di - elements of the fourth multiplicity (integer)" << endl
             << "<num_of_threads> - number of threads (> 0)" << endl;
    }
}
/// Метод для записи в файл результата выполнения программы
/// \param path - путь к файлу
/// \param result - прямое произведение множеств
void write_res(string path, vector<vector<int>> result) {
    try {
        ofstream out(path);
        if (out.is_open()) {
            out << "{";
            for (int i = 0; i < result.size(); ++i) {
                if (i != 0)
                    out << ",";
                out << "{" << result[i][0] << ", " << result[i][1] << ", "
                    << result[i][2] << ", " << result[i][3] << "}";
                if (i != result.size() - 1)
                    out << endl;
            }
            out << "}";
            out.close();
        }
    } catch (const exception &err) {
        cout << "Error while writing result into the file.";
    }
}

int main() {
    vector<vector<int>> result; //вектор для результата выполнения программы
    vecs = vector<vector<int>>(4); //вектор для хранения данных множеств
    string pathin; //путь для входных данных
    string pathout; //путь для выходных данных
    cout << "Type input file's path" << endl;
    cin >> pathin; //считывание путей
    cout << "Type output file's path" << endl;
    cin >> pathout; //считывание путей
    read_data(pathin, vecs);
    //рассчет количества элементов в прямом произведении множеств
    int result_num = vecs[0].size() * vecs[1].size() * vecs[2].size() * vecs[3].size();
    if (num_of_threads <= 0) {//количество потоков не может быть <= 0
        cout << "Number of threads must be > 0" << endl;
        cout << "The program will use 2 threads" << endl;
    }
    //потоков не может быть больше чем элементов в декартовом произведении
    if (num_of_threads > result_num) {
        cout << "Too many threads" << endl
             << "The program will use only " << result_num << " threads" << endl;
        num_of_threads = result_num;
    }
    clock_t start_time = clock();//время начала работы потоков
    mult(result, num_of_threads);
    clock_t end_time = clock();//время окончания работы потоков
    write_res(pathout, result);//вывод результата в файл

    return 0;
}
