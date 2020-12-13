/*
 * Гилазутдинова Аделя БПИ194 6 вар
 *
 * Задача о курильщиках.
 * Есть три процесса-курильщика и один процесс-посредник. Курильщик непрерывно
 * скручивает сигареты и курит их. Чтобы скрутить сигарету, нужны табак,
 * бумага и спички. У одного процесса- курильщика есть табак, у второго – бумага,
 * а у третьего – спички. Посредник кладет на стол по два разных случайных компонента.
 * Тот процесс- курильщик, у которого есть третий компонент, забирает компоненты со стола,
 * скручивает сигарету и курит. Посредник дожидается, пока курильщик закончит,
 * затем процесс повторяется. Создать многопоточное приложение, моделирующее поведение
 * курильщиков и посредника. При решении задачи использовать семафоры.
 */
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <thread>
#include <zconf.h>
using namespace std;
enum Ingredients /* Enum representing the ingredients */
{
    paper,
    tobacco,
    matches
};
bool smoker_found;      //флаг (нашел ли курильщик недостающий ингредиент)
sem_t *mediator,        //семафор, обозначающий выбрал ли посредник 2 рандомных ингредиента
*print,         //семафор для изменения глобальных переменных и печати в консоль
*found;         //семафор, обозначающий нащел ли курильщик недостающие ингредиенты на столе
Ingredients ingredient1;//первый выбранный посредником ингредиент
Ingredients ingredient2;//второй выбранный посредником ингредиент
int iter_max, iter_temp;//переменные для итерирования повторений

/// Метод, переводящий элемент перечисления в строковвый формат
/// \param ingredient - элемент перечисления
/// \return - строковцый формат
string ToString(Ingredients ingredient) {
    switch (ingredient) {
        case paper:
            return "paper";
        case tobacco:
            return "tobacco";
        case matches:
            return "matches";
    }
}

/// Метод, имитирующий курение
/// \param ingredient - ингредиент, которым владеет курильщик
void smoke(Ingredients ingredient) {
    cout << "smoker with " + ToString(ingredient) + " is smoking" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "-";
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "-";
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "-";
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << " ~" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
}

/// Метод, имитирующий ожидание
/// \param message - выводимое сообщение
/// \param millisec - количество миллисекунд ожидания
void write_with_pause(string message, int millisec) {
    cout << message;
    this_thread::sleep_for(chrono::milliseconds(millisec));
    cout << " . ";
    this_thread::sleep_for(chrono::milliseconds(millisec));
    cout << ". ";
    this_thread::sleep_for(chrono::milliseconds(millisec));
    cout << ". " << endl;
    this_thread::sleep_for(chrono::milliseconds(millisec));
}

/// Метод, имитирующий то, как курильщики смотрят на стол и ищут недостающие ингредиенты
/// \param ingr - ингредиент курильщика
/// \return - nullptr
void *smoker_look_at_table(void *ingr) {
    Ingredients ingredient = static_cast<Ingredients>((uintptr_t) ingr);
    //определенное количество повторений (заданное в аргументах ком. строки) либо беск. цикл
    for (int i = 1; i <= iter_max; i += iter_temp) {
        //курильщик ожидает, пока посредник выбирает ингридиенты
        sem_wait(mediator);
        //если курильщик с недостающим ингридиентом не найден
        sem_wait(print);
        //есди курильщик с недостающим ингредиентом не наайден
        if (!smoker_found)
            write_with_pause("smoker with " + ToString(ingredient) + " is looking at the table", 100);
        sem_post(print);
        //если у курильщика есть недостающий ингредиент
        if (ingredient != ingredient1 && ingredient != ingredient2) {
            smoker_found = 1;
            sem_wait(print);
            write_with_pause("smoker with " + ToString(ingredient) + " found the missing ingredients", 500);
            sem_post(print);
            smoke(ingredient);
            //3 раза, так как его ждут остальные 2 курильщика и посредник
            sem_post(found);
            sem_post(found);
            sem_post(found);
        } else {
            //ожидание, когда курильщик покурит
            sem_wait(found);
        }
    }
    return nullptr;
}
void *getIngredients(void *args) {
    //определенное количество повторений (заданное в аргументах ком. строки) либо беск. цикл
    for (int i = 1; i <= iter_max; i += iter_temp) {
        write_with_pause("mediator is choosing ingredients", 500);
        smoker_found = 0;
        //рандомный выбор ингредиентов
        int first = rand() % 3;
        int second = rand() % 3;
        while (second == first)
            second = rand() % 3;
        ingredient1 = static_cast<Ingredients>(first);
        ingredient2 = static_cast<Ingredients>(second);
        write_with_pause("mediator puts " + ToString(ingredient1) + " and " + ToString(ingredient2) + " on the table", 500);
        //3 раза, потому что его ждут 3 курильщика
        sem_post(mediator);
        sem_post(mediator);
        sem_post(mediator);
        //ожидание, когда курильщик покурит
        sem_wait(found);
        //sem_post(found);
    }
    return nullptr;
}
int main(int argc, char **argv) {
    //если в агрументы не было передано количество повторений, то повториний будет бесконечное количество
    if (argc == 1) {
        iter_max = 1;
        iter_temp = 0;
    } else {
        if(argc > 2){
            cout << "You should enter 1 integer number > 0" << endl;
            return 0;
        }
        iter_temp = 1;
        iter_max = atoi(argv[1]);
        if (iter_max <= 0) {
            cout << "Number of repetitions must be an integer number > 0" << endl;
            return 0;
        }
    }
    //инициализация семафоров
    sem_unlink("/mediator");
    sem_unlink("/print");
    sem_unlink("/found");
    mediator = sem_open("/mediator", O_CREAT, S_IRWXU, 0);
    print = sem_open("/print", O_CREAT, S_IRWXU, 1);
    found = sem_open("/found", O_CREAT, S_IRWXU, 0);
    if (!mediator || !print || !found) {
        cout << "Semaphores failed" << endl;
        return 0;
    }
    //создание потоков
    pthread_t smoker_w_paper_th;
    pthread_t smoker_w_tobacco_th;
    pthread_t smoker_w_matches_th;
    pthread_t mediator_th;
    pthread_create(&smoker_w_paper_th, NULL, smoker_look_at_table, (char *) 0 + paper);
    pthread_create(&smoker_w_tobacco_th, NULL, smoker_look_at_table, (char *) 0 + tobacco);
    pthread_create(&smoker_w_matches_th, NULL, smoker_look_at_table, (char *) 0 + matches);
    pthread_create(&mediator_th, NULL, getIngredients, NULL);

    //синхронизация потоков
    pthread_join(smoker_w_paper_th, nullptr);
    pthread_join(smoker_w_tobacco_th, nullptr);
    pthread_join(smoker_w_matches_th, nullptr);
    pthread_join(mediator_th, nullptr);

    //удаление семафоров
    sem_unlink("mediator");
    sem_unlink("print");
    sem_unlink("found");
    sem_close(print);
    sem_close(mediator);
    sem_close(found);

    return 0;
}
