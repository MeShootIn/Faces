#include <iostream>
#include <locale>
#include "subscriber.h"



int main() {
    setlocale(0, "");

    // ���-�� ���������
    int const N = 4;

    // ��������� ������ ��� ������ ����������
    Subscriber ** subscribers = new Subscriber * [N];

    std::cout << "������� " << (N - 1) << " ����� � ������� \"������� ���������_������_(������������_�����) ���-��_�����\":\n";

    // ���������� ������ ��� (N - 1) ��������� � ����������
    for(int i = 0; i < N - 1; ++i) {
        std::string surname;
        double cost;
        int minutes;

        std::cin >> surname >> cost >> minutes;

        // �������� ���������� ������ ����� ����������� � �����������
        subscribers[i] = new Subscriber(surname, cost, minutes);
    }

    // ��������� ������� �������� �� ���������
    subscribers[N - 1] = new Subscriber();

    std::cout << "��������� ���� ����������: " << getTotalCost(subscribers, N) << "\n";

    // ������� ������, ���������� ��� ���������
    for(int i = 0; i < N; ++i) {
        delete subscribers[i];
        subscribers[i] = nullptr;
    }

    // ������� ������, ���������� ��� ������ ����������
    delete [] subscribers;
    subscribers = nullptr;

    system("PAUSE");

    return 0;
}
