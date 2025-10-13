// Ordered linked list class is based on:
// https://runestone.academy/ns/books/published/cppds/LinearLinked/ImplementinganOrderedList.html

// similar to unordered lists except it orders the data

#include <iostream>
using namespace std;

class Node {
    private:
        int data;
        Node *next;

    public:
        Node(int initdata) {
        data = initdata; //the nodes data.
        next = NULL; //next will become a pointer to another Node object.
    }

    int getData() {
        //returns the data of the Node.
        return data;
    }

    Node *getNext() {
        //returns the next Node in the linked list.
        return next;
    }

    void setData(int newData) {
        //Changes the data of the Node.
        data = newData;
    }

    void setNext(Node *newnext) {
        //assigns the next item in the linked list.
        next = newnext;
    }
};

class OrderedList {
    public:
            Node *head; //The first Node of the linked list.

        OrderedList() {
                head = NULL;
        }

        bool search(int item) {
            //finds a Node that contains item in the linked list.
            Node *current = head;
            bool found = false;
            bool stop = false;
            while (current != NULL && !found && !stop) {
                //iterates through the entire list until item is found.
                if (current->getData() == item) {
                    found = true;
                } else {
                    if (current->getData() > item) {
                        stop = true;
                    } else {
                        current = current->getNext();
                    }
                }
            }

            return found;
        }

        void add(int item) {
            if (head == NULL) {
                Node *newNode = new Node(item);
                head = newNode;
            } else {
                Node *current = head;
                Node *previous = NULL;
                bool stop = false;
                while (current != NULL && !stop) {
                    if (current->getData() > item) { //if the data of the current Node is greater than item:
                        stop = true;
                    } else {
                        previous = current;
                        current = current->getNext();
                    }
                }
                Node *temp = new Node(item);
                if (previous == NULL) {
                    //sets the current head as temp's next item,
                    //sets temp as the new head.
                    temp->setNext(head);
                    head = temp;
                } else {
                    //sets the current Node as temp's next Node,
                    //sets temp to previous's next Node.
                    temp->setNext(current);
                    previous->setNext(temp);
                }
            }
        }

        bool isEmpty() {
            //Returns true if the head is NULL.
            return head == NULL;
        }

        int size() {
            //returns the length of the linked list.
            Node *current = head;
            int count = 0;
            while (current != NULL) {
                count++;
                current = current->getNext();
            }

            return count;
        }

        friend ostream& operator<<(ostream& os, const OrderedList& ol);
};

ostream& operator<<(ostream& os, const OrderedList& ol) {
    //operator for printing the data of every Node in the list.
    Node *current = ol.head;
    while (current != NULL) {
        os<<current->getData()<<endl;
        current = current->getNext();
    }
    return os;
}


int main() {
        OrderedList mylist;
    mylist.add(31);
    mylist.add(77);
    mylist.add(17);
    mylist.add(93);
    mylist.add(26);
    mylist.add(54);

    cout<<"SIZE: "<<mylist.size()<<endl;
    cout<<"contains 93?\t"<<mylist.search(93)<<endl;
    cout<<"contains 100?\t"<<mylist.search(100)<<endl<<endl;
    cout<<"MY LIST: "<<endl<<mylist;
    return 0;
}

