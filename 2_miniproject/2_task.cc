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
};

int main() {
    OrderedList<Student> students;
    students.AddItem(new Student("Alice", "Smith", "M0000001", "01/15/2002", 3.6));
    students.AddItem(new Student("Brian", "Johnson", "M0000005", "07/04/2001", 3.1));
    students.AddItem(new Student("Cara", "Lee", "M0000003", "11/30/2000", 3.9));
    students.AddItem(new Student("Derek", "Brown", "M0000012", "02/22/2002", 3.4));

    cout << "SIZE: " << students.Size() << endl;

    Student queryExisting("Temp", "Student", "M0000005", "01/01/2000");
    Student queryMissing("Temp", "Student", "M9999999", "01/01/2000");
    cout << "contains M0000005?\t" << students.IsInList(&queryExisting) << endl;
    cout << "contains M9999999?\t" << students.IsInList(&queryMissing) << endl << endl;

    students.Reset();
    cout << endl << "Iterating with SeeNext:" << endl;
    while (true) {
        Student* next = students.SeeNext();
        if (next == nullptr) {
            break;
        }
        cout << *next << '\n';
    }
    cout << endl;

    try {
        Student* atOne = students.SeeAt(1);
        cout << "Item at index 1: " << *atOne << endl;
        cout << "Age of that student: " << atOne->GetAge() << endl;
    } catch (const exception& ex) {
        cout << "SeeAt error: " << ex.what() << endl;
    }

    Student removeTarget("Temp", "Student", "M0000005", "01/01/2000");
    Student* removed = students.GetItem(&removeTarget);
    if (removed != nullptr) {
        cout << "Removed student: " << *removed << endl;
        delete removed;
    } else {
        cout << "Student M0000005 not found" << endl;
    }

    cout << "SIZE AFTER REMOVE: " << students.Size() << endl;
    cout << "LIST AFTER REMOVE:" << endl << students;
    return 0;
}
