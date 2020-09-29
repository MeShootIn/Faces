#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <string>



class Subscriber
{
public:
    Subscriber(); // Конструктор по умолчанию
    Subscriber(std::string surname, double cost, int minutes); // Конструктор с параметрами
    ~Subscriber(); // Деструктор

    std::string getSurname(); // Геттер для фамилии
    void setSurname(std::string surname); // Сеттер для фамилии
    int getMinutes(); // Геттер для минут
    void setMinutes(int minutes); // Сеттер для минут
    double getCost(); // Геттер для стоимости
    void setCost(double cost); // Сеттер для стоимости

private:
    std::string surname; // Фамилия абонента
    int minutes; // Общая продолжительность разговора в минутах
    double cost; // Стоимость минуты разговора
};

// Глобальная функция, вычисляющая общую стоимость звонков абонентов, содержащаяся в массиве
double getTotalCost(Subscriber ** array, int size);

#endif // SUBSCRIBER_H
