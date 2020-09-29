#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <string>



class Subscriber
{
public:
    Subscriber(); // ����������� �� ���������
    Subscriber(std::string surname, double cost, int minutes); // ����������� � �����������
    ~Subscriber(); // ����������

    std::string getSurname(); // ������ ��� �������
    void setSurname(std::string surname); // ������ ��� �������
    int getMinutes(); // ������ ��� �����
    void setMinutes(int minutes); // ������ ��� �����
    double getCost(); // ������ ��� ���������
    void setCost(double cost); // ������ ��� ���������

private:
    std::string surname; // ������� ��������
    int minutes; // ����� ����������������� ��������� � �������
    double cost; // ��������� ������ ���������
};

// ���������� �������, ����������� ����� ��������� ������� ���������, ������������ � �������
double getTotalCost(Subscriber ** array, int size);

#endif // SUBSCRIBER_H
