#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>

struct Student {
    std::string first_name;
    std::string last_name;
    int m_number;
};

class LinkedList {
public:
    LinkedList() = default;
    ~LinkedList() { clear(); }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void push_back(const Student& student) {
        Node* node = new Node{student, nullptr};
        if (!head_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    void clear() {
        Node* current = head_;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    bool empty() const { return head_ == nullptr; }

    std::size_t size() const { return size_; }

    void print() const {
        const Node* current = head_;
        std::size_t index = 1;
        while (current) {
            const Student& s = current->data;
            std::cout << index << ". " << s.first_name << ' ' << s.last_name << " (M"
                      << s.m_number << ")\n";
            current = current->next;
            ++index;
        }
    }

    void sortByFirstName(bool ascending) {
        if (!head_ || !head_->next) {
            return;
        }
        Node* sorted = nullptr;
        Node* current = head_;
        while (current) {
            Node* next = current->next;
            insertByFirstName(sorted, current, ascending);
            current = next;
        }
        head_ = sorted;
        rebuildTail();
    }

    void sortByLastName(bool ascending) {
        if (!head_ || !head_->next) {
            return;
        }
        Node* unsorted = head_;
        Node* sorted_head = nullptr;
        Node* sorted_tail = nullptr;

        while (unsorted) {
            Node** best_link = &unsorted;
            for (Node** link = &unsorted; *link; link = &((*link)->next)) {
                if (compareByLastName((*link)->data, (*best_link)->data, ascending)) {
                    best_link = link;
                }
            }
            Node* best = *best_link;
            *best_link = best->next;
            best->next = nullptr;
            if (!sorted_head) {
                sorted_head = sorted_tail = best;
            } else {
                sorted_tail->next = best;
                sorted_tail = best;
            }
        }

        head_ = sorted_head;
        tail_ = sorted_tail;
    }

    void sortByMNumber(bool ascending) {
        if (!head_ || !head_->next) {
            return;
        }
        head_ = mergeSortByMNumber(head_, ascending);
        rebuildTail();
    }

private:
    struct Node {
        Student data;
        Node* next;
    };

    Node* head_{nullptr};
    Node* tail_{nullptr};
    std::size_t size_{0};

    static bool compareStrings(const std::string& lhs, const std::string& rhs, bool ascending) {
        if (ascending) {
            return lhs < rhs;
        }
        return lhs > rhs;
    }

    static bool compareNumbers(int lhs, int rhs, bool ascending) {
        if (ascending) {
            return lhs < rhs;
        }
        return lhs > rhs;
    }

    static bool compareByFirstName(const Student& lhs, const Student& rhs, bool ascending) {
        if (lhs.first_name != rhs.first_name) {
            return compareStrings(lhs.first_name, rhs.first_name, ascending);
        }
        if (lhs.last_name != rhs.last_name) {
            return compareStrings(lhs.last_name, rhs.last_name, ascending);
        }
        return compareNumbers(lhs.m_number, rhs.m_number, ascending);
    }

    static bool compareByLastName(const Student& lhs, const Student& rhs, bool ascending) {
        if (lhs.last_name != rhs.last_name) {
            return compareStrings(lhs.last_name, rhs.last_name, ascending);
        }
        if (lhs.first_name != rhs.first_name) {
            return compareStrings(lhs.first_name, rhs.first_name, ascending);
        }
        return compareNumbers(lhs.m_number, rhs.m_number, ascending);
    }

    static bool compareByMNumber(const Student& lhs, const Student& rhs, bool ascending) {
        if (lhs.m_number != rhs.m_number) {
            return compareNumbers(lhs.m_number, rhs.m_number, ascending);
        }
        if (lhs.last_name != rhs.last_name) {
            return compareStrings(lhs.last_name, rhs.last_name, ascending);
        }
        return compareStrings(lhs.first_name, rhs.first_name, ascending);
    }

    void insertByFirstName(Node*& sorted_head, Node* node, bool ascending) {
        if (!sorted_head || compareByFirstName(node->data, sorted_head->data, ascending)) {
            node->next = sorted_head;
            sorted_head = node;
            return;
        }

        Node* current = sorted_head;
        while (current->next &&
               !compareByFirstName(node->data, current->next->data, ascending)) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }

    Node* mergeSortByMNumber(Node* head, bool ascending) {
        if (!head || !head->next) {
            return head;
        }

        Node* slow = head;
        Node* fast = head->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        Node* right = slow->next;
        slow->next = nullptr;

        Node* left_sorted = mergeSortByMNumber(head, ascending);
        Node* right_sorted = mergeSortByMNumber(right, ascending);
        return mergeByMNumber(left_sorted, right_sorted, ascending);
    }

    Node* mergeByMNumber(Node* left, Node* right, bool ascending) {
        Node dummy{{"", "", 0}, nullptr};
        Node* tail = &dummy;

        while (left && right) {
            if (compareByMNumber(left->data, right->data, ascending)) {
                tail->next = left;
                left = left->next;
            } else {
                tail->next = right;
                right = right->next;
            }
            tail = tail->next;
        }

        tail->next = left ? left : right;
        return dummy.next;
    }

    void rebuildTail() {
        tail_ = head_;
        while (tail_ && tail_->next) {
            tail_ = tail_->next;
        }
    }
};

bool askAscending() {
    while (true) {
        std::cout << "Choose direction (1 = Ascending, 2 = Descending): ";
        int direction = 0;
        if (!(std::cin >> direction)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n";
            continue;
        }
        if (direction == 1) {
            return true;
        }
        if (direction == 2) {
            return false;
        }
        std::cout << "Please select 1 or 2.\n";
    }
}

void printMenu() {
    std::cout << "\nMenu\n";
    std::cout << "1. Sort by First Name (Insertion Sort)\n";
    std::cout << "2. Sort by Last Name (Selection Sort)\n";
    std::cout << "3. Sort by MNumber (Merge Sort)\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter choice: ";
}

void seed_students(LinkedList& list) {
    constexpr std::size_t kStudentCount = 50;
    const std::array<std::string, kStudentCount> first_names{
        "Avery",   "Blake",   "Cameron", "Dakota", "Emerson", "Finley",   "Gavin",
        "Harper",  "Isaac",   "Jasper",  "Kendall","Logan",   "Maya",     "Nolan",
        "Olivia",  "Parker",  "Quinn",   "Riley",  "Sawyer",  "Taylor",   "Uma",
        "Victor",  "Wyatt",   "Xavier",  "Yara",   "Zane",    "Amelia",   "Brooke",
        "Colin",   "Dylan",   "Elias",   "Fiona",  "Grayson", "Hazel",    "Ian",
        "Juliet",  "Kai",     "Liam",    "Mila",   "Noah",    "Ophelia",  "Piper",
        "Rowan",   "Sophia",  "Tessa",   "Uri",    "Vera",    "Wren",     "Ximena",
        "Zoey"};

    const std::array<std::string, kStudentCount> last_names{
        "Anderson",  "Bennett",  "Chambers", "Dalton",  "Ellis",    "Fitzgerald",
        "Garcia",    "Hendrix",  "Ingram",   "Jefferson","Keller",  "Lambert",
        "Monroe",    "Nash",     "Owens",    "Pratt",    "Quincy",  "Ramirez",
        "Sinclair",  "Thompson", "Underwood","Vargas",   "Whitman", "Xenos",
        "Young",     "Zimmerman","Ashford",  "Briar",    "Callahan","Donovan",
        "Easton",    "Frost",    "Gallagher","Hawkins",  "Irving",  "Jamison",
        "Keaton",    "Langley",  "Merritt",  "Novak",    "Ortega",  "Prescott",
        "Reeves",    "Sheridan", "Truitt",   "Ulrich",   "Vance",   "Wallace",
        "Yates",     "Zeller"};

    const std::array<int, kStudentCount> m_numbers{
        15219531, 15214320, 15218765, 15216002, 15219999, 15211111, 15217890, 15216754, 15215555,
        15214444, 15213333, 15218888, 15217777, 15216666, 15212222, 15219901, 15215005, 15213030,
        15219000, 15217555, 15216543, 15214999, 15212001, 15219876, 15214567, 15210999, 15219345,
        15211888, 15218222, 15217007, 15216123, 15219654, 15210678, 15218555, 15219801, 15211234,
        15219123, 15213456, 15218080, 15211999, 15217222, 15216555, 15211123, 15218333, 15219555,
        15210777, 15217789, 15214666, 15218801, 15216999};

    for (std::size_t i = 0; i < kStudentCount; ++i) {
        std::size_t first_index = (i * 17 + 11) % kStudentCount;
        std::size_t last_index = (i * 7 + 3) % kStudentCount;
        std::size_t number_index = (i * 13 + 5) % kStudentCount;

        Student student{
            first_names[first_index],
            last_names[last_index],
            m_numbers[number_index],
        };
        list.push_back(student);
    }
}

int main() {
    LinkedList students;
    seed_students(students);

    if (students.empty()) {
        std::cout << "No students available.\n";
        return 0;
    }

    bool running = true;
    while (running) {
        printMenu();
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            bool ascending = askAscending();
            students.sortByFirstName(ascending);
            std::cout << "\nStudents sorted by First Name:\n";
            students.print();
            break;
        }
        case 2: {
            bool ascending = askAscending();
            students.sortByLastName(ascending);
            std::cout << "\nStudents sorted by Last Name:\n";
            students.print();
            break;
        }
        case 3: {
            bool ascending = askAscending();
            students.sortByMNumber(ascending);
            std::cout << "\nStudents sorted by MNumber:\n";
            students.print();
            break;
        }
        case 4:
            running = false;
            break;
        default:
            std::cout << "Please select a number between 1 and 4.\n";
            break;
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
