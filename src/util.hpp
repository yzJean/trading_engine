#ifndef UTIL_HPP
#define UTIL_HPP

#include <sstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

/** matchingEngine **/
//#define DEBUG_MATCHING
//#define DEBUG_INSERT

/**orderBook**/
//#define DEBUG_CAL_ORDERBOOK

/**sideContainer**/
//#define DEBUG_SIDECONTAINER

namespace util {
inline string stringProcessor(const float& fNumb) {
    string ss = to_string(fNumb);
    size_t dotPos = ss.find_last_of('.');
    size_t numberDigits = ss.size() - 1 - ss.find_last_of('.');
    if (numberDigits > 4) {
        ss.erase(ss.begin() + ss.size() - 1);
    }
    ss.erase(ss.find_last_not_of('0') + 1, string::npos);
    ss.erase(ss.find_last_not_of('.') + 1, string::npos);
    return ss;
};

enum class CMD {
    PULL = 2,
    AMEND = 4,
    INSERT = 6
};

inline vector<string> getSpiltedCmd(const string& cmd) {
    vector<string> spiltedInput;
    spiltedInput.reserve(10);
    stringstream ss(cmd);
    while (ss.good()) {
        string substr = "";
        getline(ss, substr, ',');
        spiltedInput.emplace_back(substr);
    }
    return spiltedInput;
}

inline CMD getCmdCategory(const vector<string>& spiltedInput) {
    size_t sizeSpiltedInput = spiltedInput.size();
    CMD retCmd;
    switch (sizeSpiltedInput) {
        case (2): {
            retCmd = CMD::PULL;
            break;
        }
        case (4): {
            retCmd = CMD::AMEND;
            break;
        }
        default:
            retCmd = CMD::INSERT;
            break;
    }
    return retCmd;
}

};  //end of namespace util

#endif  //end of UTIL_HPP