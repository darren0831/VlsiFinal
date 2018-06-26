#ifndef VLSI_FINAL_PROJECT_DISJOINT_SET_HPP_
#define VLSI_FINAL_PROJECT_DISJOINT_SET_HPP_

#include <vector>

class DisjointSet {
public:
    DisjointSet(int size) {
        this->size = size;
        this->parent = std::vector<int>();
        initialize();
    }

    void initialize() {
        parent = std::vector<int>(size);
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
        groupSize = (int) parent.size();
    }

    void pack(int a, int b) {
        if (find(a) != find(b)) {
            --groupSize;
        }
        parent[find(b)] = find(a);
    }

    int find(int t) {
        return (t == parent[t]) ? t : (parent[t] = find(parent[t]));
    }

    int numGroups() const {
        return groupSize;
    }

private:
    std::vector<int> parent;
    int groupSize;
    int size;
};

#endif // VLSI_FINAL_PROJECT_DISJOINT_SET_HPP_
