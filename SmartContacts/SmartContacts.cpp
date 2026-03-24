#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <ctime>
#include <Windows.h>

using namespace std;

// ==================== СТРУКТУРЫ ДАННЫХ ====================

struct Date {
    int day;
    int month;
    int year;

    Date() : day(0), month(0), year(0) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
};

struct Phone {
    int countryCode;
    int cityCode;
    long long number;

    Phone() : countryCode(0), cityCode(0), number(0) {}
    Phone(int cc, int c, long long n) : countryCode(cc), cityCode(c), number(n) {}
};

enum class Group {
    WORK,
    FAMILY,
    FRIENDS,
    OTHERS
};

struct Contact {
    string lastName;
    string firstName;
    string patronymic;
    Phone phone;
    Date birthday;
    string email;
    Group category;

    string getFullName() const {
        return lastName + firstName;
    }

    int getBirthdayDayOfYear() const {
        static const int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int dayOfYear = 0;
        for (int i = 1; i < birthday.month; i++) {
            dayOfYear += daysInMonth[i];
        }
        dayOfYear += birthday.day;
        return dayOfYear;
    }
};

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

string groupToString(Group g) {
    switch (g) {
    case Group::WORK: return "Работа";
    case Group::FAMILY: return "Семья";
    case Group::FRIENDS: return "Друзья";
    case Group::OTHERS: return "Другие";
    default: return "Неизвестно";
    }
}

bool validatePhone(const Phone& phone) {
    if (phone.countryCode < 1 || phone.countryCode > 999) return false;
    if (phone.cityCode < 0 || phone.cityCode > 999) return false;
    if (phone.number < 0) return false;
    return true;
}

bool validateEmail(const string& email) {
    size_t atPos = email.find('@');
    if (atPos == string::npos) return false;

    size_t dotPos = email.find('.', atPos + 1);
    if (dotPos == string::npos) return false;

    return true;
}

bool validateDate(const Date& date) {
    if (date.year < 1900 || date.year > 2025) return false;
    if (date.month < 1 || date.month > 12) return false;

    static const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int maxDays = daysInMonth[date.month - 1];

    if (date.month == 2) {
        bool isLeap = (date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0);
        if (isLeap) maxDays = 29;
    }

    return (date.day >= 1 && date.day <= maxDays);
}

void clearInput() {
    cin.clear();
    cin.ignore(10000, '\n');
}

// Функция для проверки, состоит ли строка только из цифр
bool isOnlyDigits(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

int inputInt(const string& prompt, int minVal = -1000000, int maxVal = 1000000) {
    string input;
    int value;

    while (true) {
        cout << prompt;
        getline(cin, input);

        // Проверяем, что введены только цифры
        if (!isOnlyDigits(input)) {
            cout << "Ошибка! Введите только цифры, без букв.\n";
            continue;
        }

        // Преобразуем в число
        try {
            value = stoi(input);
        }
        catch (...) {
            cout << "Ошибка! Некорректное число.\n";
            continue;
        }

        if (value >= minVal && value <= maxVal) {
            return value;
        }

        cout << "Ошибка! Число должно быть от " << minVal << " до " << maxVal << ".\n";
    }
}

long long inputLongLong(const string& prompt, long long minVal = 0, int minLength = 0) {
    string input;
    long long value;

    while (true) {
        cout << prompt;
        getline(cin, input);

        // Проверяем, что введены только цифры
        if (!isOnlyDigits(input)) {
            cout << "Ошибка! Номер телефона должен содержать только цифры.\n";
            continue;
        }

        // Проверяем минимальную длину номера
        if (input.length() < minLength) {
            cout << "Ошибка! Номер телефона должен содержать минимум " << minLength << " цифр.\n";
            continue;
        }

        // Преобразуем в число
        try {
            value = stoll(input);
        }
        catch (...) {
            cout << "Ошибка! Некорректный номер.\n";
            continue;
        }

        if (value >= minVal) {
            return value;
        }

        cout << "Ошибка! Число должно быть не меньше " << minVal << ".\n";
    }
}

string inputString(const string& prompt, bool allowEmpty = false) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (!allowEmpty && value.empty()) {
            cout << "Поле не может быть пустым! Попробуйте снова.\n";
        }
        else {
            return value;
        }
    }
}

// ==================== ВВОД КОНТАКТА ====================

Contact inputContact() {
    Contact c;

    cout << "\n=== Ввод нового контакта ===\n";

    c.lastName = inputString("Фамилия: ", false);
    c.firstName = inputString("Имя: ", false);
    c.patronymic = inputString("Отчество: ", true);

    cout << "\n--- Ввод телефона ---\n";
    do {
        c.phone.countryCode = inputInt("Код страны (1-999): ", 1, 999);
        c.phone.cityCode = inputInt("Код города (0-999): ", 0, 999);
        c.phone.number = inputLongLong("Номер телефона (минимум 8 цифр): ", 0, 8);

        if (!validatePhone(c.phone)) {
            cout << "Неверный формат телефона! Повторите ввод.\n";
        }
    } while (!validatePhone(c.phone));

    cout << "\n--- Ввод даты рождения ---\n";
    do {
        c.birthday.day = inputInt("День (1-31): ", 1, 31);
        c.birthday.month = inputInt("Месяц (1-12): ", 1, 12);
        c.birthday.year = inputInt("Год (1900-2025): ", 1900, 2025);

        if (!validateDate(c.birthday)) {
            cout << "Неверная дата! Повторите ввод.\n";
        }
    } while (!validateDate(c.birthday));

    do {
        c.email = inputString("Email: ", false);
        if (!validateEmail(c.email)) {
            cout << "Неверный формат email! Должен содержать @ и . после него.\n";
        }
    } while (!validateEmail(c.email));

    cout << "\nВыберите категорию:\n";
    cout << "1 - Работа (WORK)\n";
    cout << "2 - Семья (FAMILY)\n";
    cout << "3 - Друзья (FRIENDS)\n";
    cout << "4 - Другие (OTHERS)\n";

    int choice = inputInt("Ваш выбор (1-4): ", 1, 4);

    switch (choice) {
    case 1: c.category = Group::WORK; break;
    case 2: c.category = Group::FAMILY; break;
    case 3: c.category = Group::FRIENDS; break;
    case 4: c.category = Group::OTHERS; break;
    }

    cout << "\nКонтакт успешно добавлен!\n";
    return c;
}

// ==================== ВЫВОД ТАБЛИЦЫ ====================

string truncateString(const string& str, size_t maxLen) {
    if (str.length() <= maxLen) return str;
    return str.substr(0, maxLen - 3) + "...";
}

void printTableHeader(int colWidths[]) {
    cout << "\n" << string(colWidths[0] + colWidths[1] + colWidths[2] + colWidths[3] + colWidths[4] + colWidths[5] + 29, '-') << "\n";
    cout << "| " << left << setw(colWidths[0]) << "Фамилия"
        << " | " << setw(colWidths[1]) << "Имя"
        << " | " << setw(colWidths[2]) << "Телефон"
        << " | " << setw(colWidths[3]) << "Дата рожд."
        << " | " << setw(colWidths[4]) << "Email"
        << " | " << setw(colWidths[5]) << "Категория"
        << " |\n";
    cout << "|" << string(colWidths[0] + 2, '-')
        << "|" << string(colWidths[1] + 2, '-')
        << "|" << string(colWidths[2] + 2, '-')
        << "|" << string(colWidths[3] + 2, '-')
        << "|" << string(colWidths[4] + 2, '-')
        << "|" << string(colWidths[5] + 2, '-') << "|\n";
}

void printContact(const Contact& c, int colWidths[]) {
    string phoneStr = to_string(c.phone.countryCode) + "(" + to_string(c.phone.cityCode) + ")" + to_string(c.phone.number);
    string dateStr = to_string(c.birthday.day) + "." + to_string(c.birthday.month) + "." + to_string(c.birthday.year);

    cout << "| " << left << setw(colWidths[0]) << truncateString(c.lastName, colWidths[0])
        << " | " << setw(colWidths[1]) << truncateString(c.firstName, colWidths[1])
        << " | " << setw(colWidths[2]) << truncateString(phoneStr, colWidths[2])
        << " | " << setw(colWidths[3]) << dateStr
        << " | " << setw(colWidths[4]) << truncateString(c.email, colWidths[4])
        << " | " << setw(colWidths[5]) << groupToString(c.category)
        << " |\n";
}

void printAllContacts(const vector<Contact>& contacts) {
    if (contacts.empty()) {
        cout << "\nСписок контактов пуст.\n";
        return;
    }

    int colWidths[6] = { 15, 15, 20, 12, 25, 12 };

    for (const auto& c : contacts) {
        string phoneStr = to_string(c.phone.countryCode) + "(" + to_string(c.phone.cityCode) + ")" + to_string(c.phone.number);
        string dateStr = to_string(c.birthday.day) + "." + to_string(c.birthday.month) + "." + to_string(c.birthday.year);

        colWidths[0] = max(colWidths[0], (int)c.lastName.length());
        colWidths[1] = max(colWidths[1], (int)c.firstName.length());
        colWidths[2] = max(colWidths[2], (int)phoneStr.length());
        colWidths[3] = max(colWidths[3], (int)dateStr.length());
        colWidths[4] = max(colWidths[4], (int)c.email.length());
        colWidths[5] = max(colWidths[5], (int)groupToString(c.category).length());
    }

    for (int i = 0; i < 6; i++) {
        if (colWidths[i] > 30) colWidths[i] = 30;
        if (colWidths[i] < 10) colWidths[i] = 10;
    }

    printTableHeader(colWidths);
    for (const auto& c : contacts) {
        printContact(c, colWidths);
    }
    cout << string(colWidths[0] + colWidths[1] + colWidths[2] + colWidths[3] + colWidths[4] + colWidths[5] + 29, '-') << "\n";
}

// ==================== ПОИСК ====================

string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

vector<Contact> searchByMask(const vector<Contact>& contacts, const string& mask) {
    vector<Contact> results;

    vector<string> keywords;
    stringstream ss(mask);
    string word;
    while (ss >> word) {
        keywords.push_back(toLower(word));
    }

    for (const auto& c : contacts) {
        string lowerLastName = toLower(c.lastName);
        string lowerFirstName = toLower(c.firstName);

        bool match = true;
        for (const auto& kw : keywords) {
            if (lowerLastName.find(kw) == string::npos &&
                lowerFirstName.find(kw) == string::npos) {
                match = false;
                break;
            }
        }

        if (match) {
            results.push_back(c);
        }
    }

    return results;
}

// ==================== УДАЛЕНИЕ ====================

bool deleteContactByLastName(vector<Contact>& contacts, const string& lastName) {
    auto it = remove_if(contacts.begin(), contacts.end(),
        [&lastName](const Contact& c) {
            return toLower(c.lastName) == toLower(lastName);
        });

    if (it != contacts.end()) {
        contacts.erase(it, contacts.end());
        return true;
    }
    return false;
}

// ==================== РЕДАКТИРОВАНИЕ ====================

void editContact(Contact& c) {
    cout << "\n=== Редактирование контакта ===\n";
    cout << "Текущие данные:\n";
    cout << "Фамилия: " << c.lastName << "\n";
    cout << "Имя: " << c.firstName << "\n";
    cout << "Отчество: " << (c.patronymic.empty() ? "(не указано)" : c.patronymic) << "\n";
    cout << "Телефон: +" << c.phone.countryCode << "(" << c.phone.cityCode << ")" << c.phone.number << "\n";
    cout << "Дата рождения: " << c.birthday.day << "." << c.birthday.month << "." << c.birthday.year << "\n";
    cout << "Email: " << c.email << "\n";
    cout << "Категория: " << groupToString(c.category) << "\n\n";

    cout << "Что хотите изменить?\n";
    cout << "1 - Фамилию\n2 - Имя\n3 - Отчество\n4 - Телефон\n5 - Дату рождения\n6 - Email\n7 - Категорию\n0 - Отмена\n";

    int choice = inputInt("Ваш выбор: ", 0, 7);

    switch (choice) {
    case 1:
        c.lastName = inputString("Новая фамилия: ", false);
        break;
    case 2:
        c.firstName = inputString("Новое имя: ", false);
        break;
    case 3:
        c.patronymic = inputString("Новое отчество: ", true);
        break;
    case 4: {
        do {
            c.phone.countryCode = inputInt("Код страны (1-999): ", 1, 999);
            c.phone.cityCode = inputInt("Код города (0-999): ", 0, 999);
            c.phone.number = inputLongLong("Номер телефона: ", 0);
            if (!validatePhone(c.phone)) {
                cout << "Неверный формат телефона! Повторите ввод.\n";
            }
        } while (!validatePhone(c.phone));
        break;
    }
    case 5: {
        do {
            c.birthday.day = inputInt("День (1-31): ", 1, 31);
            c.birthday.month = inputInt("Месяц (1-12): ", 1, 12);
            c.birthday.year = inputInt("Год (1900-2025): ", 1900, 2025);
            if (!validateDate(c.birthday)) {
                cout << "Неверная дата! Повторите ввод.\n";
            }
        } while (!validateDate(c.birthday));
        break;
    }
    case 6: {
        do {
            c.email = inputString("Новый email: ", false);
            if (!validateEmail(c.email)) {
                cout << "Неверный формат email!\n";
            }
        } while (!validateEmail(c.email));
        break;
    }
    case 7: {
        cout << "Выберите категорию:\n1 - Работа\n2 - Семья\n3 - Друзья\n4 - Другие\n";
        int catChoice = inputInt("Ваш выбор (1-4): ", 1, 4);
        switch (catChoice) {
        case 1: c.category = Group::WORK; break;
        case 2: c.category = Group::FAMILY; break;
        case 3: c.category = Group::FRIENDS; break;
        case 4: c.category = Group::OTHERS; break;
        }
        break;
    }
    default:
        cout << "Редактирование отменено.\n";
        return;
    }

    cout << "Контакт успешно обновлён!\n";
}

// ==================== СОРТИРОВКА ====================

void sortByFullName(vector<Contact>& contacts) {
    sort(contacts.begin(), contacts.end(),
        [](const Contact& a, const Contact& b) {
            return a.getFullName() < b.getFullName();
        });
    cout << "Сортировка по алфавиту (Фамилия + Имя) выполнена.\n";
}

void sortByBirthday(vector<Contact>& contacts) {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int currentDayOfYear = now->tm_yday + 1;

    sort(contacts.begin(), contacts.end(),
        [currentDayOfYear](const Contact& a, const Contact& b) {
            int aDay = a.getBirthdayDayOfYear();
            int bDay = b.getBirthdayDayOfYear();

            int aDaysUntil = (aDay >= currentDayOfYear) ? (aDay - currentDayOfYear) : (aDay + 365 - currentDayOfYear);
            int bDaysUntil = (bDay >= currentDayOfYear) ? (bDay - currentDayOfYear) : (bDay + 365 - currentDayOfYear);

            return aDaysUntil < bDaysUntil;
        });
    cout << "Сортировка по дате рождения (от ближайших к будущим) выполнена.\n";
}

// ==================== ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ ====================

void searchByCategory(const vector<Contact>& contacts) {
    if (contacts.empty()) {
        cout << "Список контактов пуст.\n";
        return;
    }

    cout << "Выберите категорию для поиска:\n";
    cout << "1 - Работа\n2 - Семья\n3 - Друзья\n4 - Другие\n";
    int choice = inputInt("Ваш выбор: ", 1, 4);

    Group target;
    switch (choice) {
    case 1: target = Group::WORK; break;
    case 2: target = Group::FAMILY; break;
    case 3: target = Group::FRIENDS; break;
    default: target = Group::OTHERS; break;
    }

    vector<Contact> results;
    for (const auto& c : contacts) {
        if (c.category == target) {
            results.push_back(c);
        }
    }

    if (results.empty()) {
        cout << "Контакты в выбранной категории не найдены.\n";
    }
    else {
        cout << "\nНайдено контактов: " << results.size() << "\n";
        printAllContacts(results);
    }
}

void showStatistics(const vector<Contact>& contacts) {
    if (contacts.empty()) {
        cout << "Список контактов пуст.\n";
        return;
    }

    cout << "\n=== СТАТИСТИКА КОНТАКТОВ ===\n";
    cout << "Всего контактов: " << contacts.size() << "\n\n";

    int workCount = 0, familyCount = 0, friendsCount = 0, othersCount = 0;

    for (const auto& c : contacts) {
        switch (c.category) {
        case Group::WORK: workCount++; break;
        case Group::FAMILY: familyCount++; break;
        case Group::FRIENDS: friendsCount++; break;
        case Group::OTHERS: othersCount++; break;
        }
    }

    cout << "По категориям:\n";
    cout << "  - Работа: " << workCount << " (" << (workCount * 100 / contacts.size()) << "%)\n";
    cout << "  - Семья: " << familyCount << " (" << (familyCount * 100 / contacts.size()) << "%)\n";
    cout << "  - Друзья: " << friendsCount << " (" << (friendsCount * 100 / contacts.size()) << "%)\n";
    cout << "  - Другие: " << othersCount << " (" << (othersCount * 100 / contacts.size()) << "%)\n";
}

// ==================== ФАЙЛОВЫЕ ОПЕРАЦИИ ====================

void saveToFile(const vector<Contact>& contacts, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!\n";
        return;
    }

    file << contacts.size() << "\n";

    for (const auto& c : contacts) {
        file << c.lastName << "\n";
        file << c.firstName << "\n";
        file << c.patronymic << "\n";
        file << c.phone.countryCode << " " << c.phone.cityCode << " " << c.phone.number << "\n";
        file << c.birthday.day << " " << c.birthday.month << " " << c.birthday.year << "\n";
        file << c.email << "\n";
        file << static_cast<int>(c.category) << "\n";
    }

    file.close();
    cout << "Данные сохранены в файл " << filename << "\n";
}

void loadFromFile(vector<Contact>& contacts, const string& filename) {
    contacts.clear();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Файл " << filename << " не найден. Будет создан новый при сохранении.\n";
        return;
    }

    int count;
    file >> count;
    file.ignore();

    for (int i = 0; i < count; i++) {
        Contact c;

        getline(file, c.lastName);
        getline(file, c.firstName);
        getline(file, c.patronymic);

        file >> c.phone.countryCode >> c.phone.cityCode >> c.phone.number;
        file.ignore();

        file >> c.birthday.day >> c.birthday.month >> c.birthday.year;
        file.ignore();

        getline(file, c.email);

        int cat;
        file >> cat;
        file.ignore();
        c.category = static_cast<Group>(cat);

        contacts.push_back(c);
    }

    file.close();
    cout << "Загружено " << contacts.size() << " контактов из файла " << filename << "\n";
}

// ==================== ГЛАВНОЕ МЕНЮ ====================

void showMenu() {
    cout << "\n========================================\n";
    cout << "   СИСТЕМА УПРАВЛЕНИЯ КОНТАКТАМИ\n";
    cout << "========================================\n";
    cout << "1. Добавить контакт\n";
    cout << "2. Просмотр всех записей\n";
    cout << "3. Поиск по маске (Фамилия/Имя)\n";
    cout << "4. Удаление контакта по фамилии\n";
    cout << "5. Редактирование контакта\n";
    cout << "6. Сортировка по алфавиту (Фамилия+Имя)\n";
    cout << "7. Сортировка по дате рождения\n";
    cout << "8. Поиск по категории (доп. функция 1)\n";
    cout << "9. Статистика контактов (доп. функция 2)\n";
    cout << "0. Выход и сохранение\n";
    cout << "========================================\n";
    cout << "Ваш выбор: ";
}

// ==================== MAIN ====================

int main() {
    // Настройка русского языка для Windows
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    vector<Contact> contacts;
    const string filename = "database.txt";

    loadFromFile(contacts, filename);

    int choice;

    do {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите число от 0 до 9.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
        case 1: {
            Contact newContact = inputContact();
            contacts.push_back(newContact);
            break;
        }
        case 2: {
            printAllContacts(contacts);
            break;
        }
        case 3: {
            if (contacts.empty()) {
                cout << "Список контактов пуст.\n";
                break;
            }
            string mask;
            cout << "Введите маску для поиска (одно или несколько слов): ";
            getline(cin, mask);

            vector<Contact> results = searchByMask(contacts, mask);
            if (results.empty()) {
                cout << "Контакты не найдены.\n";
            }
            else {
                cout << "\nНайдено контактов: " << results.size() << "\n";
                printAllContacts(results);
            }
            break;
        }
        case 4: {
            if (contacts.empty()) {
                cout << "Список контактов пуст.\n";
                break;
            }
            string lastName;
            cout << "Введите фамилию контакта для удаления: ";
            getline(cin, lastName);

            if (deleteContactByLastName(contacts, lastName)) {
                cout << "Контакт удалён (если существовал).\n";
            }
            else {
                cout << "Контакт с фамилией \"" << lastName << "\" не найден.\n";
            }
            break;
        }
        case 5: {
            if (contacts.empty()) {
                cout << "Список контактов пуст.\n";
                break;
            }
            string lastName;
            cout << "Введите фамилию контакта для редактирования: ";
            getline(cin, lastName);

            bool found = false;
            for (auto& c : contacts) {
                if (toLower(c.lastName) == toLower(lastName)) {
                    editContact(c);
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Контакт с фамилией \"" << lastName << "\" не найден.\n";
            }
            break;
        }
        case 6: {
            if (contacts.empty()) {
                cout << "Список контактов пуст.\n";
                break;
            }
            sortByFullName(contacts);
            break;
        }
        case 7: {
            if (contacts.empty()) {
                cout << "Список контактов пуст.\n";
                break;
            }
            sortByBirthday(contacts);
            break;
        }
        case 8: {
            if (contacts.empty()) {
                cout << "Список контактов пуст.\n";
                break;
            }
            searchByCategory(contacts);
            break;
        }
        case 9: {
            showStatistics(contacts);
            break;
        }
        case 0: {
            cout << "Сохранение и выход...\n";
            break;
        }
        default: {
            cout << "Неверный выбор! Пожалуйста, выберите пункт от 0 до 9.\n";
            break;
        }
        }
    } while (choice != 0);

    saveToFile(contacts, filename);

    cout << "Программа завершена. До свидания!\n";

    return 0;
}
