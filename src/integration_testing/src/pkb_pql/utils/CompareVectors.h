#ifndef AUTOTESTER_COMPAREVECTORS_H
#define AUTOTESTER_COMPAREVECTORS_H

#include <string>
#include <vector>

using std::vector;
using std::string;

class CompareVectors {
public:
    static vector<string> sorted(vector<string> v);
    static vector<int> sorted(vector<int> v);

};

#endif //AUTOTESTER_COMPAREVECTORS_H
