#include <iostream>
#include <stdexcept>
using namespace std;

// Exception class
class ListError : public exception {
	private:
		string message;
	public:
		ListError(const string& msg) : message(msg){}
		const char* what() const noexcept override {
			return message.c_str();
		}
	};

// Defining class template
template <typename T> class OrderedList {
	private:
		T* items[20]; // array of pointers
		int size;

	public:
		OrderedList() : size(0) {
			for (int i = 0; i < 20; i++) {
				items[i] = nullptr;
			}
		}
	
		// Destructor
		~OrderedList() {
			MakeEmpty();
		}

		// Check if list is empty
		bool IsEmpty() const {
			return size == 0;
		}

		// Check if list is full
		bool IsFull() const {
			return size == 20;
		}

		// Remove all items
		void MakeEmpty() {
			for (int i=0; i < size; i++) {
				delete items[i];
				items[i] = nullptr;
			}
			size = 0;
		}

		// Add item in order
		void AddItem(const T& item) {
			if (IsFull()) {
				throw ListError("Cannot add item: list is full.");
			}
			
			int i = size;
			while (i > 0 && *items[i-1] > item) {				
				i--;
			}
			// SHR
			for (int j = size; j > i; j--) {
				items[j] = items[j - 1];
			}
			items[i] = new T(item);
			size++;
		}

		// Remove item
		void RemoveItem(const T& item) {
			int i=0;
			while (i < size && !(*items[i] == item)) {
				i++;
			}

			if (i == size) {
				throw ListError("Item not found in the list.");
			}

			delete items[i];

			for (int j = i; j < size - 1; j++) {
				items[j] = items [j + 1];
			}

			items[size - 1] = nullptr;
			size--;
		}

		// Display list
		void PrintList() const {
			for (int i = 0; i < size; i++) {
				cout << *items[i] << " ";
			}
			cout << endl;
		}
};

class MyItem {
	private:
		int value;
	public:
		MyItem(int v = 0) : value(v) {}

		bool operator>(const MyItem other) const { return value > other.value; }
		bool operator<(const MyItem other) const { return value < other.value; }
		bool operator==(const MyItem other) const { return value == other.value; }
		
		friend ostream& operator<<(ostream& os, const MyItem& item) {
			os << item.value;
			return os;
		}
	};


int main() {
	OrderedList<MyItem> list;

	try {
		list.AddItem(MyItem(10));
		list.AddItem(MyItem(5));
		list.AddItem(MyItem(15));
		list.PrintList();

		list.RemoveItem(MyItem(10));
		list.PrintList();

		list.RemoveItem(MyItem(100));
	}
	catch (const ListError& e) {
		cout << "Error: " << e.what() << endl;
	}

	return 0;
}
