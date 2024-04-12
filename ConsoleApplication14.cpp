#include <iostream>
#include <fstream>
#include <cstring>
#include <Windows.h>
#include <locale>

using namespace std;

enum Gender { Male, Female };

struct Date {
    int year;
    int month;
    int day;
};

struct Student {
    char lastName[50];
    char firstName[50];
    char patronymic[50];
    Date birthDate;
    char group[20];
    Gender gender;
};

struct Node {
    Student data;
    Node* next;
};

class StudentList {
private:
    Node* head;

public:
    StudentList() : head(nullptr) {}

    void addStudent(const Student& student) {
        Node* newNode = new Node;
        newNode->data = student;
        newNode->next = nullptr;

        if (head == nullptr) {
            head = newNode;
        }
        else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    void deleteStudent(const char* lastName, const char* firstName) {
        Node* current = head;
        Node* prev = nullptr;

        while (current != nullptr) {
            if (strcmp(current->data.lastName, lastName) == 0 &&
                strcmp(current->data.firstName, firstName) == 0) {
                if (prev == nullptr) {
                    head = current->next;
                }
                else {
                    prev->next = current->next;
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }

        cout << "Студента не знайдено." << endl;
    }

    void displayStudents() {
        Node* current = head;
        while (current != nullptr) {
            cout << "Прізвище: " << current->data.lastName << endl;
            cout << "Ім'я: " << current->data.firstName << endl;
            cout << "По батькові: " << current->data.patronymic << endl;
            cout << "Дата народження: " << current->data.birthDate.year << "/"
                << current->data.birthDate.month << "/"
                << current->data.birthDate.day << endl;
            cout << "Група: " << current->data.group << endl;
            cout << "Стать: " << (current->data.gender == Male ? "Чоловік" : "Жінка") << endl;
            cout << "---------------------------" << endl;
            current = current->next;
        }
    }

    void saveToFile(const char* fileName) {
        ofstream file(fileName, ios::binary);
        if (!file.is_open()) {
            cout << "Помилка відкриття файлу " << fileName << endl;
            return;
        }

        Node* current = head;
        while (current != nullptr) {
            file.write(reinterpret_cast<char*>(&current->data), sizeof(Student));
            current = current->next;
        }

        file.close();
    }

    void loadFromFile(const char* fileName, StudentList& studentList) {
        ifstream file(fileName, ios::binary);
        if (!file.is_open()) {
            cout << "Помилка відкриття файлу " << fileName << endl;
            return;
        }

        Student student;
        while (file.read(reinterpret_cast<char*>(&student), sizeof(Student))) {
            studentList.addStudent(student);
        }

        file.close();
    }

    ~StudentList() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

bool isValidName(const char* name) {
    while (*name) {
        if (!isalpha(*name) && *name != ' ') {
            return false;
        }
        name++;
    }
    return true;
}

bool isValidDate(const Date& date) {
    if (date.year < 1900 || date.year > 2024) {
        return false;
    }
    if (date.month < 1 || date.month > 12) { 
        return false;
    }
    if (date.day < 1 || date.day > 31) { 
        return false;
    }

    return true;
}

int main() {
    setlocale(LC_CTYPE, "ukr");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    StudentList studentList;
    studentList.loadFromFile("data.bin", studentList);

    int choice;
    do {
        cout << "Меню:\n";
        cout << "1. Показати список студентів\n";
        cout << "2. Додати нового студента\n";
        cout << "3. Видалити існуючого студента\n";
        cout << "4. Вийти\n";
        cout << "Введіть ваш вибір: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            studentList.displayStudents();
            break;
        case 2: {
            Student newStudent;
            char lastName[50], firstName[50], patronymic[50];
            cout << "Введіть прізвище студента: ";
            cin.getline(lastName, 50);
            if (!isValidName(lastName)) {
                cout << "Некоректне прізвище. Будь ласка, введіть правильне значення.\n";
                break;
            }
            cout << "Введіть ім'я студента: ";
            cin.getline(firstName, 50);
            if (!isValidName(firstName)) {
                cout << "Некоректне ім'я. Будь ласка, введіть правильне значення.\n";
                break;
            }
            cout << "Введіть по батькові студента: ";
            cin.getline(patronymic, 50);
            if (!isValidName(patronymic)) {
                cout << "Некоректний по батькові. Будь ласка, введіть правильне значення.\n";
                break;
            }
            cout << "Введіть дату народження (день місяць рік): ";
            cin >> newStudent.birthDate.day >> newStudent.birthDate.month >> newStudent.birthDate.year;
            if (!isValidDate(newStudent.birthDate)) {
                cout << "Некоректна дата народження. Будь ласка, введіть правильне значення.\n";
                break;
            }
            cin.ignore();
            cout << "Введіть групу студента: ";
            cin.getline(newStudent.group, 20);
            cout << "Введіть стать студента (0 - Чоловік, 1 - Жінка): ";
            int genderInput;
            cin >> genderInput;
            newStudent.gender = static_cast<Gender>(genderInput);
            cin.ignore();

            strcpy_s(newStudent.lastName, lastName);
            strcpy_s(newStudent.firstName, firstName);
            strcpy_s(newStudent.patronymic, patronymic);

            studentList.addStudent(newStudent);
            break;
        }
        case 3: {
            char lastName[50], firstName[50];
            cout << "Введіть прізвище студента для видалення: ";
            cin.getline(lastName, 50);
            cout << "Введіть ім'я студента для видалення: ";
            cin.getline(firstName, 50);
            studentList.deleteStudent(lastName, firstName);
            break;
        }
        case 4:
            studentList.saveToFile("data.bin");
            cout << "Завершення програми.\n";
            break;
        default:
            cout << "Некоректний вибір. Будь ласка, введіть правильну опцію.\n";
            break;
        }
    } while (choice != 4);

    return 0;
}
