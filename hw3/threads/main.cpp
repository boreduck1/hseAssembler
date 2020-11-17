/*
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
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

using namespace std;
mutex mtx; //мьютекс для избегания ситуаций неуправляемого изменения одних и тех
           //же общих данных несколькими потоками
vector<vector<int>> vecs; //вектор, состоящий из данных векторов А, B, C, D
vector<thread> threads;   //вектор потоков
int num_of_threads;       //количество потоков

/// Метод для вычисления заданного количества элементов прямого
/// произведения данных множеств, начиная с определенного момента
/// \param a - индекс множества A, с которого нужно начинать вычисление
/// \param b - индекс множества B, с которого нужно начинать вычисление
/// \param c - индекс множества C, с которого нужно начинать вычисление
/// \param d - индекс множества D, с которого нужно начинать вычисление
/// \param num -  заданное количество элементов, которое нужно вычислить
/// \param result - вектор, в который нужно записать полученные элементы
void mult(int a, int b, int c, int d, int num, vector<vector<int>> &result) {
  int ind1 = a, ind2 = b, ind3 = c, ind4 = d;
  for (int i = ind1; i < vecs[0].size(); ++i, ind2 = 0) {
    for (int j = ind2; j < vecs[1].size(); ++j, ind3 = 0) {
      for (int k = ind3; k < vecs[2].size(); ++k, ind4 = 0) {
        for (int l = ind4; l < vecs[3].size(); ++l) {
          if (num == 0) //если num элементов вычислилось, то выход
          {
            return;
          }
          mtx.lock();
          result.push_back({vecs[0][i], vecs[1][j], vecs[2][k], vecs[3][l]});
          mtx.unlock();
          num--;
        }
      }
    }
  }
}

/// Метод для вычисления индексов элементов множеств, начиная с которых нужно
/// начать вычисление элементов прямого произведения \param num - число
/// элементов, которое уже было вычислено \return - вектор длины 4 с индексами
vector<int> find_begin_inds(int num) {
  vector<int> new_num;
  new_num.push_back(num % vecs[3].size()); //индекс 4 множества
  num /= vecs[3].size();
  new_num.push_back(num % vecs[2].size()); //индекс 3 множества
  num /= vecs[2].size();
  new_num.push_back(num % vecs[1].size()); //индекс 2 множества
  num /= vecs[1].size();
  new_num.push_back(num % vecs[0].size()); //индекс 1 множества
  reverse(new_num.begin(),
          new_num.end()); //отзеркалить, чтобы индексы были от 1 до 4 множества
  return new_num;
}

/// Метод переводящий строку в число
/// \param line - строка
/// \param num - число, в которое нужно записать строку
/// \return - 1, если конвертация прошла успешно, иначе - 0
bool string_to_int(string line, int &num) {
  for (int i = 0; i < line.size(); ++i) {
    if (line[i] > '9' ||
        line[i] < '0') //если в строке что-то кроме цифр, то это не число
      if (line[i] == '-' && i == 0)//если число отрицательное
        continue;
      else
        return 0;
    num = atoi(line.c_str());
    return 1;
  }
}
/// Метод для считывания входных данных из файла
/// \param path - путь к файлу
/// \param vecs - вектор, в который нужно записать полученные множества
void read_data(string path, vector<vector<int>> &vecs) {
  try {
    ifstream in(path);
    string line{};
    string number{};
    int ind = -1; //индекс множества ([0;3])
    while (getline(in, line)) {
      stringstream strStream(line);
      ind++;
      while (getline(strStream, number, ' ')) { //разделение через пробел
        int num;
        if (!string_to_int(number, num)) { //если в файле есть другие символы
                                           //кроме цифр, то он неверный
          throw exception();
        }
        vecs[ind].push_back(num);
      }
      if (ind == 3) //если считано 4 множества
        break;
    }
    for (int i = 0; i < vecs.size(); ++i) {
      if (vecs[i].size() == 0)
        throw exception();
    }

    string k;
    in >> k; //считывание количества потоков
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
  string pathin;  //путь для входных данных
  string pathout; //путь для выходных данных
  cout << "Type input file's path" << endl;
  cin >> pathin; //считывание путей
  cout << "Type output file's path" << endl;
  cin >> pathout; //считывание путей
  read_data(pathin, vecs);
  //рассчет количества элементов в прямом произведении множеств
  int result_num =
      vecs[0].size() * vecs[1].size() * vecs[2].size() * vecs[3].size();
  //рассчет количества вычисляемых элементов прямого произведения для каждого потока
  if (num_of_threads <= 0) {//количество потоков не может быть <= 0
    cout << "Number of threads must be > 0";
    return 0;
  }
  int operations_per_thread = result_num / num_of_threads;
  //потоков не может быть больше чем элементов в декартовом произведении
  if (num_of_threads > result_num) {
    cout << "Too many threads" << endl
         << "The program will use only " << result_num << " threads";
    num_of_threads = result_num;
  }

  //цикл с созданием потоков
  for (int i = 0; i < num_of_threads; ++i) {
    vector<int> inds = find_begin_inds(operations_per_thread * i);
    try {
      if (i == num_of_threads - 1)
        // mult(inds[0], inds[1], inds[2], inds[3], result_num -
        // operations_per_thread * i, ref(result));
        threads.push_back(thread(mult, inds[0], inds[1], inds[2], inds[3],
                                 result_num - operations_per_thread * i,
                                 ref(result)));
      else
        // mult(inds[0], inds[1], inds[2], inds[3], operations_per_thread,
        // ref(result));
        threads.push_back(thread(mult, inds[0], inds[1], inds[2], inds[3],
                                 operations_per_thread, ref(result)));
    } catch (const std::exception &err) {
      cout << "ERROR";
    }
  }
  for (auto &th : threads) //ожидание окончания выполнения всех потоков
    th.join();

  write_res(pathout, result); //вывод результата в файл

  return 0;
}