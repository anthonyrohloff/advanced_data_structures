// Ordered linked list class is based on:
// https://runestone.academy/ns/books/published/cppds/LinearLinked/ImplementinganOrderedList.html

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
using namespace std;

template <typename T> class OrderedList; // forward decl for operator<<

// operator<< forward decl for OrderedList<T>
template <typename T>
ostream& operator<<(ostream& os, const OrderedList<T>& ol);

template <typename T>
class Node {
private:
    T* data;
    Node<T>* next;

public:
    explicit Node(T* initdata) : data(initdata), next(nullptr) {}

    T* getData() const { return data; }
    Node<T>* getNext() const { return next; }

    void setData(T* newData) { data = newData; }
    void setNext(Node<T>* newnext) { next = newnext; }
};

template <typename T>
class OrderedList {
public:
    OrderedList() : head(nullptr), iterator(nullptr) {}

    // prevent accidental shallow copies (simple safety)
    OrderedList(const OrderedList&) = delete;
    OrderedList& operator=(const OrderedList&) = delete;

    ~OrderedList() {
        Node<T>* cur = head;
        while (cur) {
            Node<T>* nxt = cur->getNext();
            delete cur->getData();
            delete cur;
            cur = nxt;
        }
    }

    void AddItem(T* item) {
        if (item == nullptr) {
            throw invalid_argument("Cannot add null pointer to OrderedList");
        }

        Node<T>* temp = new Node<T>(item);
        Node<T>* current = head;
        Node<T>* previous = nullptr;

        while (current != nullptr && *(current->getData()) < *item) {
            previous = current;
            current = current->getNext();
        }

        if (previous == nullptr) {
            temp->setNext(head);
            head = temp;
        } else {
            temp->setNext(current);
            previous->setNext(temp);
        }

        if (iterator == nullptr) {
            iterator = head;
        }
    }

    T* GetItem(const T* item) {
        if (item == nullptr) {
            throw invalid_argument("Cannot search for null pointer in OrderedList");
        }

        Node<T>* current = head;
        Node<T>* previous = nullptr;

        while (current != nullptr) {
            if (*(current->getData()) == *item) {
                if (previous == nullptr) {
                    head = current->getNext();
                } else {
                    previous->setNext(current->getNext());
                }

                if (iterator == current) {
                    iterator = current->getNext();
                }

                T* foundItem = current->getData();
                delete current;

                if (head == nullptr) {
                    iterator = nullptr;
                }

                return foundItem;
            }

            if (*(current->getData()) > *item) {
                break;
            }

            previous = current;
            current = current->getNext();
        }

        return nullptr;
    }

    bool IsInList(const T* item) const {
        if (item == nullptr) {
            throw invalid_argument("Cannot search for null pointer in OrderedList");
        }

        Node<T>* current = head;
        while (current != nullptr) {
            if (*(current->getData()) == *item) {
                return true;
            }
            if (*(current->getData()) > *item) {
                return false;
            }
            current = current->getNext();
        }
        return false;
    }

    bool IsEmpty() const { return head == nullptr; }

    int Size() const {
        Node<T>* current = head;
        int count = 0;
        while (current != nullptr) {
            ++count;
            current = current->getNext();
        }
        return count;
    }

    T* SeeNext() {
        if (IsEmpty()) {
            throw runtime_error("SeeNext called on empty list");
        }
        if (iterator == nullptr) {
            return nullptr;
        }
        T* data = iterator->getData();
        iterator = iterator->getNext();
        return data;
    }

    T* SeeAt(int index) {
        if (index < 0) {
            throw out_of_range("Negative index passed to SeeAt");
        }
        if (IsEmpty()) {
            throw runtime_error("SeeAt called on empty list");
        }

        Node<T>* current = head;
        int currentIndex = 0;
        while (current != nullptr && currentIndex < index) {
            current = current->getNext();
            ++currentIndex;
        }

        if (current == nullptr) {
            throw out_of_range("Index passed to SeeAt is past end of list");
        }

        iterator = current->getNext();
        return current->getData();
    }

    void Reset() { iterator = head; }

    void DisplayList() {
        cout << "\n===== Ordered List Display =====\n";
        cout << "(Total items: " << Size() << ")\n\n";

        Node<T>* savedIterator = iterator;

        Node<T>* current = head;
        int index = 0;
        while (current != nullptr) {
            cout << "Item " << index++ << ":\n";
            current->getData()->Display();
            current = current->getNext();
        }

        cout << "===== End of List =====\n";

        iterator = savedIterator;
    }

    friend ostream& operator<< <T>(ostream& os, const OrderedList<T>& ol);

private:
    Node<T>* head; // The first Node of the linked list.
    Node<T>* iterator; // Tracks position for SeeNext.
};

template <typename T>
ostream& operator<<(ostream& os, const OrderedList<T>& ol) {
    Node<T>* current = ol.head;
    while (current != nullptr) {
        os << *(current->getData()) << '\n';
        current = current->getNext();
    }
    return os;
}

class Student {
private:
    string firstName;
    string lastName;
    string mNumber;
    double gpa;
    string birthday; // expected format: MM/DD/YYYY

    static tm ParseBirthday(const string& date) {
        tm birth{};
        int month = 0;
        int day = 0;
        int year = 0;
        char sep1 = '\0';
        char sep2 = '\0';
        istringstream iss(date);

        if (!(iss >> month >> sep1 >> day >> sep2 >> year) || sep1 != '/' || sep2 != '/' ||
            month < 1 || month > 12 || day < 1 || day > 31 || year < 1900) {
            throw invalid_argument("Birthday must be in MM/DD/YYYY format");
        }

        birth.tm_year = year - 1900;
        birth.tm_mon = month - 1;
        birth.tm_mday = day;
        return birth;
    }

public:
    Student(const string& first,
            const string& last,
            const string& mNum,
            const string& birth,
            double gradePointAverage = 0.0)
        : firstName(first),
          lastName(last),
          mNumber(mNum),
          gpa(gradePointAverage),
          birthday(birth) {
        if (mNumber.empty()) {
            throw invalid_argument("MNumber cannot be empty");
        }
        if (mNumber[0] == 'm') {
            mNumber[0] = 'M';
        } else if (mNumber[0] != 'M') {
            mNumber = "M" + mNumber;
        }
        if (gpa < 0.0) {
            throw out_of_range("GPA cannot be negative");
        }
        // Validate birthday format early
        ParseBirthday(birthday);
    }

    string GetName() const { return firstName + " " + lastName; }

    const string& GetMNumber() const { return mNumber; }

    int GetAge() const {
        tm birth = ParseBirthday(birthday);

        auto now = chrono::system_clock::now();
        time_t nowTime = chrono::system_clock::to_time_t(now);
        tm* nowPtr = localtime(&nowTime);
        if (nowPtr == nullptr) {
            throw runtime_error("Failed to obtain current time");
        }
        tm current = *nowPtr;

        int age = (current.tm_year - birth.tm_year);
        if (current.tm_mon < birth.tm_mon ||
            (current.tm_mon == birth.tm_mon && current.tm_mday < birth.tm_mday)) {
            --age;
        }
        return age;
    }

    bool operator>(const Student& other) const { return mNumber > other.mNumber; }
    bool operator<(const Student& other) const { return mNumber < other.mNumber; }
    bool operator==(const Student& other) const { return mNumber == other.mNumber; }

    friend ostream& operator<<(ostream& os, const Student& student) {
        os << student.firstName << ' ' << student.lastName << " (" << student.mNumber
           << ") GPA: " << student.gpa;
        return os;
    }

    void Display() const {
        cout << "+------------------------------------------+\n";
        cout << "| Name: " << firstName << ' ' << lastName;
        size_t nameLen = firstName.size() + lastName.size() + 7;
        for (size_t i = nameLen; i < 42; ++i) {
            cout << ' ';
        }
        cout << "|\n";

        cout << "| M#: " << mNumber;
        size_t mLen = mNumber.size() + 5;
        for (size_t i = mLen; i < 42; ++i) {
            cout << ' ';
        }
        cout << "|\n";

        cout << "| GPA: " << gpa;
        string gpaStr = to_string(gpa);
        size_t gpaLen = gpaStr.size() + 6;
        for (size_t i = gpaLen; i < 42; ++i) {
            cout << ' ';
        }
        cout << "|\n";

        cout << "| Birthday: " << birthday;
        size_t bdayLen = birthday.size() + 12;
        for (size_t i = bdayLen; i < 42; ++i) {
            cout << ' ';
        }
        cout << "|\n";

        cout << "+------------------------------------------+\n";
    }
};

static string PromptLine(const string& message) {
    cout << message;
    string input;
    getline(cin, input);
    return input;
}

static Student* CreateStudentInteractive() {
    while (true) {
        try {
            string first;
            while (first.empty()) {
                first = PromptLine("Enter first name: ");
                if (first.empty()) {
                    cout << "First name cannot be empty.\n";
                }
            }

            string last;
            while (last.empty()) {
                last = PromptLine("Enter last name: ");
                if (last.empty()) {
                    cout << "Last name cannot be empty.\n";
                }
            }

            string mNum;
            while (mNum.empty()) {
                mNum = PromptLine("Enter MNumber (with or without leading M): ");
                if (mNum.empty()) {
                    cout << "MNumber cannot be empty.\n";
                }
            }

            string birthday;
            while (birthday.empty()) {
                birthday = PromptLine("Enter birthday (MM/DD/YYYY): ");
                if (birthday.empty()) {
                    cout << "Birthday cannot be empty.\n";
                }
            }

            string gpaInput = PromptLine("Enter GPA (press enter for 0.0): ");
            double gpa = 0.0;
            if (!gpaInput.empty()) {
                gpa = stod(gpaInput);
            }

            return new Student(first, last, mNum, birthday, gpa);
        } catch (const exception& ex) {
            cout << "Error: " << ex.what() << ". Please try again.\n";
        }
    }
}

static Student CreateKeyStudent(const string& mNumber) {
    return Student("Key", "Student", mNumber, "01/01/2000");
}

int main() {
    OrderedList<Student> students;
    cout << "Ordered Student List Tester\n";

    bool running = true;
    while (running) {
        cout << "\nChoose an option:\n";
        cout << "1. Add student (AddItem)\n";
        cout << "2. Remove student by MNumber (GetItem)\n";
        cout << "3. Check if student exists (IsInList)\n";
        cout << "4. Check if list is empty (IsEmpty)\n";
        cout << "5. Get list size (Size)\n";
        cout << "6. View next student (SeeNext)\n";
        cout << "7. View student at index (SeeAt)\n";
        cout << "8. Reset iteration (Reset)\n";
        cout << "9. Display full list\n";
        cout << "10. Display ASCII list (DisplayList)\n";
        cout << "0. Exit\n";

        string choiceLine = PromptLine("Enter choice: ");
        if (choiceLine.empty()) {
            continue;
        }

        int choice = 0;
        try {
            choice = stoi(choiceLine);
        } catch (...) {
            cout << "Invalid choice. Please enter a number from 0 to 10.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            Student* created = CreateStudentInteractive();
            students.AddItem(created);
            cout << "Student added.\n";
            break;
        }
        case 2: {
            string mNum = PromptLine("Enter MNumber to remove: ");
            if (mNum.empty()) {
                cout << "No MNumber entered.\n";
                break;
            }
            try {
                Student key = CreateKeyStudent(mNum);
                Student* removed = students.GetItem(&key);
                if (removed != nullptr) {
                    cout << "Removed student: " << *removed << '\n';
                    delete removed;
                } else {
                    cout << "Student not found.\n";
                }
            } catch (const exception& ex) {
                cout << "Error: " << ex.what() << '\n';
            }
            break;
        }
        case 3: {
            string mNum = PromptLine("Enter MNumber to search for: ");
            if (mNum.empty()) {
                cout << "No MNumber entered.\n";
                break;
            }
            try {
                Student key = CreateKeyStudent(mNum);
                bool found = students.IsInList(&key);
                cout << (found ? "Student is in the list.\n" : "Student is not in the list.\n");
            } catch (const exception& ex) {
                cout << "Error: " << ex.what() << '\n';
            }
            break;
        }
        case 4: {
            cout << (students.IsEmpty() ? "The list is empty.\n" : "The list is not empty.\n");
            break;
        }
        case 5: {
            cout << "List size: " << students.Size() << '\n';
            break;
        }
        case 6: {
            try {
                Student* next = students.SeeNext();
                if (next == nullptr) {
                    cout << "Reached end of list.\n";
                } else {
                    cout << "Next student: " << *next << '\n';
                }
            } catch (const exception& ex) {
                cout << "Error: " << ex.what() << '\n';
            }
            break;
        }
        case 7: {
            string indexLine = PromptLine("Enter index (0-based): ");
            if (indexLine.empty()) {
                cout << "No index entered.\n";
                break;
            }
            try {
                int index = stoi(indexLine);
                Student* at = students.SeeAt(index);
                cout << "Student at index " << index << ": " << *at << '\n';
            } catch (const exception& ex) {
                cout << "Error: " << ex.what() << '\n';
            }
            break;
        }
        case 8: {
            students.Reset();
            cout << "Iterator reset to beginning of list.\n";
            break;
        }
        case 9: {
            if (students.IsEmpty()) {
                cout << "List is empty.\n";
            } else {
                cout << "Students in list:\n" << students;
            }
            break;
        }
        case 10: {
            students.DisplayList();
            break;
        }
        case 0:
            running = false;
            cout << "Exiting tester.\n";
            break;
        default:
            cout << "Invalid choice. Please enter a number from 0 to 10.\n";
            break;
        }
    }

    return 0;
}
