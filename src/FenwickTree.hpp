#ifndef VLSI_FINAL_PROJECT_FENWICKTREE_HPP
#define VLSI_FINAL_PROJECT_FENWICKTREE_HPP

#include <utility>
#include <vector>

class FenwickTree {
public:
    FenwickTree() = default;

    explicit FenwickTree(int width) :
        width(width + 5) {
        treeData = std::vector<int>((unsigned long) width + 5);
        rawData = std::vector<int>((unsigned long) width + 5);
    }

    void insert(int value, int count) {
        if (value > width || value <= 0) {
            fprintf(stderr, "Error: Add %d into Fenwick Tree with width %d\n", value, width);
            return;
        }
        rawData[value] += count;
        while (value < width) {
            treeData[value] += count;
            value += lowbit(value);
        }
    }

    int query(int value) const {
        if (value > width || value <= 0) {
            fprintf(stderr, "Error: Query %d from Fenwick Tree with width %d\n", value, width);
            return 0;
        }
        int sum = 0;
        while (value > 0) {
            sum += treeData[value];
            value -= lowbit(value);
        }
        return sum;
    }

    std::vector<std::pair<int, int>> remove(int value, int count) {
        if (value > width || value <= 0) {
            fprintf(stderr, "Error: Remove %d from Fenwick Tree with width %d\n", value, width);
            return std::vector<std::pair<int, int>>();
        }
        std::vector<std::pair<int, int>> result;
        while (value <= width && count > 0) {
            int request = std::min(count, rawData[value]);
            count -= request;
            insert(value, -request);
            rawData[value] -= request;
            result.emplace_back(std::make_pair(value, request));
            ++value;
        }
        return result;
    };

private:
    int lowbit(int x) const {
        return x & (-x);
    }

private:
    int width;
    std::vector<int> treeData;
    std::vector<int> rawData;
};

#endif // VLSI_FINAL_PROJECT_FENWICKTREE_HPP
