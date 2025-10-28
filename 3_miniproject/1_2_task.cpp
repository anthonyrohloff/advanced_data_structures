#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;
using Data = std::vector<int>;

constexpr std::array<std::size_t, 8> kRequestedSizes{
    5ull,
    50ull,
    500ull,
    5'000ull,
    50'000ull,
    500'000ull,
    5'000'000ull,
    5'000'000'000ull,
};

struct Timer {
    Timer() : start_(Clock::now()) {}

    void reset() { start_ = Clock::now(); }

    double elapsed_seconds() const {
        return Duration(Clock::now() - start_).count();
    }

private:
    Clock::time_point start_;
};

bool exceeds_reasonable_memory(std::size_t n, std::size_t max_bytes) {
    return n > 0 && (n > max_bytes / sizeof(int));
}

Data generate_random_data(std::size_t n, std::mt19937_64& rng, std::size_t max_bytes) {
    if (exceeds_reasonable_memory(n, max_bytes)) {
        throw std::runtime_error(
            "Requested array size exceeds the configured memory safety limit.");
    }

    Data data;
    data.reserve(n);
    const std::size_t max_random = std::min<std::size_t>(
        2ull * n,
        static_cast<std::size_t>(std::numeric_limits<int>::max()));
    std::uniform_int_distribution<int> dist(0, static_cast<int>(max_random));
    for (std::size_t i = 0; i < n; ++i) {
        data.push_back(dist(rng));
    }
    return data;
}

void insertion_sort(Data& data) {
    for (std::size_t i = 1; i < data.size(); ++i) {
        int key = data[i];
        std::size_t j = i;
        while (j > 0 && data[j - 1] > key) {
            data[j] = data[j - 1];
            --j;
        }
        data[j] = key;
    }
}

void selection_sort(Data& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        std::size_t min_index = i;
        for (std::size_t j = i + 1; j < data.size(); ++j) {
            if (data[j] < data[min_index]) {
                min_index = j;
            }
        }
        std::swap(data[i], data[min_index]);
    }
}

void shell_sort(Data& data) {
    for (std::size_t gap = data.size() / 2; gap > 0; gap /= 2) {
        for (std::size_t i = gap; i < data.size(); ++i) {
            int temp = data[i];
            std::size_t j = i;
            while (j >= gap && data[j - gap] > temp) {
                data[j] = data[j - gap];
                j -= gap;
            }
            data[j] = temp;
        }
    }
}

void heapify(Data& data, std::size_t n, std::size_t i) {
    while (true) {
        std::size_t largest = i;
        std::size_t left = 2 * i + 1;
        std::size_t right = 2 * i + 2;

        if (left < n && data[left] > data[largest]) {
            largest = left;
        }
        if (right < n && data[right] > data[largest]) {
            largest = right;
        }

        if (largest != i) {
            std::swap(data[i], data[largest]);
            i = largest;
        } else {
            break;
        }
    }
}

void heap_sort(Data& data) {
    const std::size_t n = data.size();
    if (n <= 1) {
        return;
    }
    for (std::size_t i = n / 2; i > 0; --i) {
        heapify(data, n, i - 1);
    }
    for (std::size_t i = n; i-- > 1;) {
        std::swap(data[0], data[i]);
        heapify(data, i, 0);
    }
}

void merge(Data& data, Data& temp, std::size_t left, std::size_t mid, std::size_t right) {
    std::size_t i = left;
    std::size_t j = mid;
    std::size_t k = left;
    while (i < mid && j < right) {
        if (data[i] <= data[j]) {
            temp[k++] = data[i++];
        } else {
            temp[k++] = data[j++];
        }
    }
    while (i < mid) {
        temp[k++] = data[i++];
    }
    while (j < right) {
        temp[k++] = data[j++];
    }
    for (std::size_t idx = left; idx < right; ++idx) {
        data[idx] = temp[idx];
    }
}

void merge_sort_recursive(Data& data, Data& temp, std::size_t left, std::size_t right) {
    if (right - left <= 1) {
        return;
    }
    std::size_t mid = left + (right - left) / 2;
    merge_sort_recursive(data, temp, left, mid);
    merge_sort_recursive(data, temp, mid, right);
    merge(data, temp, left, mid, right);
}

void merge_sort(Data& data) {
    Data temp(data.size());
    merge_sort_recursive(data, temp, 0, data.size());
}

std::size_t median_of_three(Data& data, std::size_t low, std::size_t mid, std::size_t high) {
    if (data[mid] < data[low]) {
        std::swap(data[mid], data[low]);
    }
    if (data[high] < data[low]) {
        std::swap(data[high], data[low]);
    }
    if (data[high] < data[mid]) {
        std::swap(data[high], data[mid]);
    }
    return mid;
}

std::size_t partition(Data& data, std::size_t low, std::size_t high) {
    std::size_t mid = low + (high - low) / 2;
    std::size_t pivot_index = median_of_three(data, low, mid, high);
    std::swap(data[pivot_index], data[high]);
    int pivot = data[high];
    std::size_t i = low;
    for (std::size_t j = low; j < high; ++j) {
        if (data[j] < pivot) {
            std::swap(data[i], data[j]);
            ++i;
        }
    }
    std::swap(data[i], data[high]);
    return i;
}

void quick_sort_recursive(Data& data, std::size_t low, std::size_t high) {
    if (low >= high) {
        return;
    }
    std::size_t pivot_index = partition(data, low, high);
    if (pivot_index > 0) {
        quick_sort_recursive(data, low, pivot_index - 1);
    }
    quick_sort_recursive(data, pivot_index + 1, high);
}

void quick_sort(Data& data) {
    if (data.empty()) {
        return;
    }
    static thread_local std::mt19937 rng(std::random_device{}());
    std::shuffle(data.begin(), data.end(), rng);
    quick_sort_recursive(data, 0, data.size() - 1);
}

void radix_sort(Data& data) {
    if (data.empty()) {
        return;
    }
    constexpr int radix = 256;
    constexpr int bits = 8;
    constexpr int mask = radix - 1;
    int max_val = *std::max_element(data.begin(), data.end());
    Data output(data.size());
    for (int shift = 0; (max_val >> shift) > 0; shift += bits) {
        std::array<int, radix> count{};
        for (int value : data) {
            ++count[(value >> shift) & mask];
        }
        int cumulative = 0;
        for (int i = 0; i < radix; ++i) {
            int tmp = count[i];
            count[i] = cumulative;
            cumulative += tmp;
        }
        for (int value : data) {
            output[count[(value >> shift) & mask]++] = value;
        }
        data.swap(output);
    }
}

struct ListNode {
    int value;
    ListNode* next;
};

ListNode* list_insertion_sort(ListNode* head) {
    ListNode dummy{0, nullptr};
    ListNode* current = head;
    while (current) {
        ListNode* next = current->next;
        ListNode* prev = &dummy;
        while (prev->next && prev->next->value < current->value) {
            prev = prev->next;
        }
        current->next = prev->next;
        prev->next = current;
        current = next;
    }
    return dummy.next;
}

ListNode* build_list(const Data& data) {
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    for (int value : data) {
        auto* node = new ListNode{value, nullptr};
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    return head;
}

void free_list(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

struct SortDefinition {
    std::string name;
    void (*sort_fn)(Data&);
    bool is_quadratic;
};

const std::vector<SortDefinition> kSorts{
    {"Insertion Sort", insertion_sort, true},
    {"Selection Sort", selection_sort, true},
    {"Shell Sort", shell_sort, false},
    {"Heap Sort", heap_sort, false},
    {"Merge Sort", merge_sort, false},
    {"Quick Sort", quick_sort, false},
    {"Radix Sort", radix_sort, false},
};

bool should_skip(const SortDefinition& sort, std::size_t n, std::size_t quadratic_limit) {
    if (sort.is_quadratic && n > quadratic_limit) {
        return true;
    }
    return false;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::size_t quadratic_limit = 50'000;
    std::size_t max_bytes = 2ull * 1024 * 1024 * 1024;
    bool include_enormous_size = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quadratic-limit" && i + 1 < argc) {
            quadratic_limit = std::stoull(argv[++i]);
        } else if (arg == "--max-bytes" && i + 1 < argc) {
            max_bytes = std::stoull(argv[++i]);
        } else if (arg == "--skip-largest") {
            include_enormous_size = false;
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  --quadratic-limit N   Max size for insertion/selection sorts (default 50000)\n"
                      << "  --max-bytes B         Max bytes allowed when generating arrays (default 2147483648)\n"
                      << "  --skip-largest        Skip the 5,000,000,000 element case\n"
                      << "  --help                Show this message\n";
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        }
    }

    std::random_device rd;
    std::mt19937_64 rng(rd());

    std::cout << std::fixed << std::setprecision(6);

    for (std::size_t size : kRequestedSizes) {
        if (!include_enormous_size && size == 5'000'000'000ull) {
            continue;
        }

        std::cout << "\nSize n = " << size << '\n';
        Data base;
        try {
            if (exceeds_reasonable_memory(size, max_bytes)) {
                std::cout << "  Skipped data generation: exceeds configured memory limit ("
                          << max_bytes << " bytes)\n";
                continue;
            }
            base = generate_random_data(size, rng, max_bytes);
        } catch (const std::exception& ex) {
            std::cout << "  Failed to generate data: " << ex.what() << '\n';
            continue;
        }

        for (const auto& sort : kSorts) {
            if (should_skip(sort, size, quadratic_limit)) {
                std::cout << "  " << sort.name << ": skipped (n beyond quadratic limit "
                          << quadratic_limit << ")\n";
                continue;
            }
            Data data = base;
            Timer timer;
            sort.sort_fn(data);
            double seconds = timer.elapsed_seconds();
            std::cout << "  " << sort.name << ": " << seconds << " s\n";
        }

        if (size <= quadratic_limit && !base.empty()) {
            Timer ll_timer;
            ListNode* list = build_list(base);
            ll_timer.reset();
            ListNode* sorted = list_insertion_sort(list);
            double seconds = ll_timer.elapsed_seconds();
            std::cout << "  Linked-list Insertion Sort: " << seconds << " s\n";
            free_list(sorted);
        }
    }

    return 0;
}
