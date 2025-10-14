// Ordered linked list class is based on:
// https://runestone.academy/ns/books/published/cppds/LinearLinked/ImplementinganOrderedList.html

#include <iostream>
#include <stdexcept>
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

class MyItem {
private:
    int value;
public:
    MyItem(int v = 0) : value(v) {}

    bool operator>(const MyItem& other) const { return value > other.value; }
    bool operator<(const MyItem& other) const { return value < other.value; }
    bool operator==(const MyItem& other) const { return value == other.value; }

    friend ostream& operator<<(ostream& os, const MyItem& item) {
        os << item.value;
        return os;
    }
};

int main() {
    OrderedList<MyItem> mylist;
    mylist.AddItem(new MyItem(31));
    mylist.AddItem(new MyItem(77));
    mylist.AddItem(new MyItem(17));
    mylist.AddItem(new MyItem(93));
    mylist.AddItem(new MyItem(26));
    mylist.AddItem(new MyItem(54));

    cout << "SIZE: " << mylist.Size() << endl;

    MyItem query93(93);
    MyItem query100(100);
    cout << "contains 93?\t" << mylist.IsInList(&query93) << endl;
    cout << "contains 100?\t" << mylist.IsInList(&query100) << endl << endl;

    mylist.Reset();
    cout << endl << "Iterating with SeeNext:" << endl;
    while (true) {
        MyItem* next = mylist.SeeNext();
        if (next == nullptr) {
            break;
        }
        cout << *next << ' ';
    }
    cout << endl;

    try {
        MyItem* atTwo = mylist.SeeAt(2);
        cout << "Item at index 2: " << *atTwo << endl;
    } catch (const exception& ex) {
        cout << "SeeAt error: " << ex.what() << endl;
    }

    MyItem removeTarget(54);
    MyItem* removed = mylist.GetItem(&removeTarget);
    if (removed != nullptr) {
        cout << "Removed item: " << *removed << endl;
        delete removed;
    } else {
        cout << "Item 54 not found" << endl;
    }

    cout << "SIZE AFTER REMOVE: " << mylist.Size() << endl;
    cout << "LIST AFTER REMOVE:" << endl << mylist;
    return 0;
}
