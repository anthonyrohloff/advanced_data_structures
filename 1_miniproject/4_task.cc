#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
using namespace std;

struct Counter {
    static long comparisons;
    static long moves;
    static void Reset() { comparisons = moves = 0; }
};

long Counter::comparisons = 0;
long Counter::moves = 0;

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

class MyItem {
    int value;
public:
    MyItem(int v=0) : value(v) {}

    bool operator>(const MyItem& other) const { Counter::comparisons++; return value > other.value; }
    bool operator<(const MyItem& other) const { Counter::comparisons++; return value < other.value; }
    bool operator==(const MyItem& other) const { Counter::comparisons++; return value == other.value; }

    friend ostream& operator<<(ostream& os, const MyItem& item) {
        os << item.value;
        return os;
    }

    int GetValue() const { return value; }
};

// CLASS 1
// Defining class template
template <typename T> class OrderedList1 {
	private:
		T* items[30]; // array of pointers
		int size;

	public:
		OrderedList1() : size(0) {
			for (int i = 0; i < 30; i++) {
				items[i] = nullptr;
			}
		}
	
		// Destructor
		~OrderedList1() {
			MakeEmpty();
		}

		// Check if list is empty
		bool IsEmpty() const {
			return size == 0;
		}

		// Check if list is full
		bool IsFull() const {
			return size == 30;
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

			int i = 0;
			while (i < size && *items[i] < item) {
				i++;
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

// CLASS 2
// Defining class template
template <typename T> class OrderedList2 {
	private:
		T* items[30]; // array of pointers
		int size;

	public:
		OrderedList2() : size(0) {
			for (int i = 0; i < 30; i++) {
				items[i] = nullptr;
			}
		}
	
		// Destructor
		~OrderedList2() {
			MakeEmpty();
		}

		// Check if list is empty
		bool IsEmpty() const {
			return size == 0;
		}

		// Check if list is full
		bool IsFull() const {
			return size == 30;
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

// CLASS 3
// Defining class template
template <typename T> class OrderedList3 {
	private:
		T* items[30]; // array of pointers
		int size;

	public:
		OrderedList3() : size(0) {
			for (int i = 0; i < 30; i++) {
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
			return size == 30;
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
		        while (next < 30 && items[next] == nullptr) next++;
		        if (next >= 30 || items[next] == nullptr) break;
		
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
		    for (int i = 0; i < 30; i++) {
		        if (items[i] != nullptr && item < *items[i]) {
		            // shift right to open this slot
		            for (int j = 19; j > i; j--) items[j] = items[j - 1];
		            items[i] = new T(item);
		            size++;
		            return;
		        }
		    }
		
		    // Case 4: bigger than all, put in first free spot
		    for (int i = 0; i < 30; i++) {
		        if (items[i] == nullptr) {
		            items[i] = new T(item);
		            size++;
		            return;
		        }
		    }
		}
		
		// Remove item
		void RemoveItem(const T& item) {
			for (int i = 0; i < 30; i++) {
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
			for (int i = 0; i < 30; i++) {
				if (items[i] != nullptr){
					cout << *items[i] << " ";
					j++;
				}
			}	
			cout << endl;
		}
};

int main(){
    srand(time(nullptr));
    const int RUNS=100;

    long totalComp1=0,totalMoves1=0;
    long totalComp2=0,totalMoves2=0;
    long totalComp3=0,totalMoves3=0;

    for(int run=0; run<RUNS; run++){
        OrderedList1<MyItem> list1;
        OrderedList2<MyItem> list2;
        OrderedList3<MyItem> list3;

        vector<int> inserted;
        // Insert 30 random numbers
        for(int i=0;i<30;i++){
            int val=rand()%100;
            inserted.push_back(val);

            Counter::Reset(); 
            list1.AddItem(MyItem(val)); 
            totalComp1+=Counter::comparisons; 
            totalMoves1+=Counter::moves;

            Counter::Reset(); 
            list2.AddItem(MyItem(val)); 
            totalComp2+=Counter::comparisons; 
            totalMoves2+=Counter::moves;

            Counter::Reset(); 
            list3.AddItem(MyItem(val)); 
            totalComp3+=Counter::comparisons; 
            totalMoves3+=Counter::moves;
        }
        // Remove 25 random ones
        for(int i=0;i<25;i++){
            int idx=rand()%inserted.size();
            int val=inserted[idx];
            inserted.erase(inserted.begin()+idx);

            Counter::Reset();
            try { 
                list1.RemoveItem(MyItem(val));
            }catch(...){ }

            totalComp1+=Counter::comparisons; 
            totalMoves1+=Counter::moves;

            Counter::Reset();
            try { 
                list2.RemoveItem(MyItem(val));
            }catch(...){ }

            totalComp2+=Counter::comparisons;
            totalMoves2+=Counter::moves;

            Counter::Reset();
            try{
                list3.RemoveItem(MyItem(val));
            }catch(...){ } 
			
            totalComp3+=Counter::comparisons; 
            totalMoves3+=Counter::moves;
        }
    }

    cout << "After " << RUNS << " runs:\n";
    cout << "OrderedList1: Comparisons=" << totalComp1 << " Moves=" << totalMoves1 << endl;
    cout << "OrderedList2: Comparisons=" << totalComp2 << " Moves=" << totalMoves2 << endl;
    cout << "OrderedList3: Comparisons=" << totalComp3 << " Moves=" << totalMoves3 << endl;
    return 0;
}