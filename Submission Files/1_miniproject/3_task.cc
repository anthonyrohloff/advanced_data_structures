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
template <typename T> class OrderedList3 {
	private:
		T* items[20]; // array of pointers
		int size;

	public:
		OrderedList3() : size(0) {
			for (int i = 0; i < 20; i++) {
				items[i] = nullptr;
			}
		}
	
		// Destructor
		~OrderedList3() {
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
		    if (IsFull()) throw ListError("List is full");
		
		    // Case 1: empty
		    if (IsEmpty()) {
		        items[0] = new T(item);
		        size++;
		        return;
		    }
		
		    // Case 2: walk through to find insertion window
		    for (int i = 0; i < 19; i++) {
		        if (items[i] == nullptr) continue;
		
		        // find right neighbor
		        int next = i + 1;
		        while (next < 20 && items[next] == nullptr) next++;
		        if (next >= 20 || items[next] == nullptr) break;
		
		        // check if item belongs here
		        if (*items[i] < item && item < *items[next]) {
		            if (next > i + 1) {
		                // gap exists, insert midpoint
		                int mid = (i + next) / 2;
		                items[mid] = new T(item);
		                size++;
		                return;
		            } else {
		                // contiguous, shift right once
		                for (int j = 19; j > next; j--) items[j] = items[j - 1];
		                items[next] = new T(item);
		                size++;
		                return;
		            }
		        }
		    }
		
		    // Case 3: smaller than all
		    for (int i = 0; i < 20; i++) {
		        if (items[i] != nullptr && item < *items[i]) {
		            // shift right to open this slot
		            for (int j = 19; j > i; j--) items[j] = items[j - 1];
		            items[i] = new T(item);
		            size++;
		            return;
		        }
		    }
		
		    // Case 4: bigger than all, put in first free spot
		    for (int i = 0; i < 20; i++) {
		        if (items[i] == nullptr) {
		            items[i] = new T(item);
		            size++;
		            return;
		        }
		    }
		}
		
		// Remove item
		void RemoveItem(const T& item) {
			for (int i = 0; i < 20; i++) {
				if (items[i] != nullptr && *items[i] == item) {
					delete items[i];
					items[i] = nullptr;
					size--;
					return;
				}
			}
			throw ListError("Item not found in list");
		}

		// Display list
		void PrintList() const {
			int j = 0;
			for (int i = 0; i < 20; i++) {
				if (items[i] != nullptr){
					cout << *items[i] << " ";
					j++;
				}
			}	
			cout << endl;
		}
};

class MyItem {
	private:
		int value;
	public:
		MyItem(int v = 0) : value(v) {}

		bool operator>(const MyItem& other) const { return value > other.value; }
		bool operator<(const MyItem& other) const { return value < other.value; }
		bool operator==(const MyItem& other) const { return value == other.value; }
		bool operator!=(const MyItem& other) const { return value != other.value; }

		friend ostream& operator<<(ostream& os, const MyItem& item) {
			os << item.value;
			return os;
		}
	};


int main() {
	OrderedList3<MyItem> list;

	try {
		list.AddItem(MyItem(10));
		list.AddItem(MyItem(5));
		list.AddItem(MyItem(15));
		list.AddItem(MyItem(25));
		list.AddItem(MyItem(20));
		list.PrintList();

		list.RemoveItem(MyItem(10));
		list.PrintList();

		list.RemoveItem(MyItem(15));
		list.PrintList();
		
		list.RemoveItem(MyItem(20));
		list.PrintList();

		list.AddItem(MyItem(17));
		list.PrintList();

		list.RemoveItem(MyItem(100));
	}
	catch (const ListError& e) {
		cout << "Error: " << e.what() << endl;
	}

	return 0;
}
