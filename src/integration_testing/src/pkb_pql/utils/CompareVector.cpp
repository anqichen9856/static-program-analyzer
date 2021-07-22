#include "CompareVectors.h"
#include <algorithm>
vector<string> CompareVectors::sorted(vector<string> v) {
    std::sort(v.begin(), v.end());
    return v;
}
vector<int> CompareVectors::sorted(vector<int> v) {
    std::sort(v.begin(), v.end());
    return v;
}
