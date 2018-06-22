#ifndef VLSI_FINAL_PROJECT_FENWICKTREE_HPP
#define VLSI_FINAL_PROJECT_FENWICKTREE_HPP

#include <vector>

class FenwickTree {
public:
    FenwickTree() = default;

    FenwickTree(int width) :
        width(width) {
        array = std::vector<int>((unsigned long) width);
    }

    void insert(int value, int count) {
        while (value < width) {
            array[value] += count;
            value += lowbit(value);
        }
    }

    void remove(int value, int count) {
        while (value < width) {
            array[value] -= count;
            value += lowbit(value);
        }
    }

    int query(int value) const {
        int sum = 0;
        while (value > 0) {
            sum += array[value];
            value -= lowbit(value);
        }
        return sum;
    }

private:
    int lowbit(int x) const {
        return x & (-x);
    }

private:
    int width;
    std::vector<int> array;
};

#endif // VLSI_FINAL_PROJECT_FENWICKTREE_HPP
