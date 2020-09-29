#include <iostream>
#include <locale>
#include "subscriber.h"



int main() {
    setlocale(0, "");

    // Кол-во абонентов
    int const N = 4;

    // Выделение памяти под массив указателей
    Subscriber ** subscribers = new Subscriber * [N];

    std::cout << "Введите " << (N - 1) << " строк в формате \"Фамилия стоимость_звонка_(вещественное_число) кол-во_минут\":\n";

    // Считывание данных для (N - 1) абонентов с клавиатуры
    for(int i = 0; i < N - 1; ++i) {
        std::string surname;
        double cost;
        int minutes;

        std::cin >> surname >> cost >> minutes;

        // Создание экземпляра класса через конструктор с параметрами
        subscribers[i] = new Subscriber(surname, cost, minutes);
    }

    // Последний абонент создаётся по умолчанию
    subscribers[N - 1] = new Subscriber();

    std::cout << "Стоимость всех разговоров: " << getTotalCost(subscribers, N) << "\n";

    // Очистка памяти, выделенной под указатели
    for(int i = 0; i < N; ++i) {
        delete subscribers[i];
        subscribers[i] = nullptr;
    }

    // Очистка памяти, выделенной под массив указателей
    delete [] subscribers;
    subscribers = nullptr;

    system("PAUSE");

    return 0;
}
