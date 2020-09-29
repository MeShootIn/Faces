#include "subscriber.h"



Subscriber::Subscriber() {
    setSurname("Иванов");
    setCost(2);
    setMinutes(0);
}

Subscriber::Subscriber(std::string surname, double cost, int minutes = 0) {
    setSurname(surname);
    setMinutes(minutes);
    setCost(cost);
}

Subscriber::~Subscriber() = default;

std::string Subscriber::getSurname() {
    return surname;
}

void Subscriber::setSurname(std::string surname) {
    this->surname = surname;
}

int Subscriber::getMinutes() {
    return minutes;
}

void Subscriber::setMinutes(int minutes) {
    this->minutes = minutes;
}

double Subscriber::getCost() {
    return cost;
}

void Subscriber::setCost(double cost) {
    this->cost = cost;
}

double getTotalCost(Subscriber ** array, int size) {
    double total = 0;

    for(int i = 0; i < size; ++i) {
        total += array[i]->getCost() * array[i]->getMinutes();
    }

    return total;
}
