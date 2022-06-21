#ifndef SIDE_CONTAINER_HPP
#define SIDE_CONTAINER_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "util.hpp"
using namespace std;

class SideContainer {
   public:
    vector<pair<float, int>> &getPriceVolContainer();
    SideContainer();
    size_t size();     // size of mp_orderId_pairPriceVol_
    size_t vecSize();  // size of vec_price_vol_, debug

    // accessor
    bool empty();
    pair<float, int> top();
    pair<float, int> last();
    string getOrderIdFromManager(const pair<float, int> &targetPair);
    pair<float, int> getPriceVolPairAtIndex(const int &index);

    // actions
    void eraseAt(const int &index);
    void updateVolumeAt(const int &index, const int &updatedVolume);
    void add(const string &strId, const float &price, const int &volume);
    void pull(const string &strId);
    void reOrder(const string &strId, const float &price, const int &volume);

    // logger
    void printChronologicalContainer();
    void printOrderContainer();

   private:
    map<string, pair<float, int>>
        mp_orderId_pairPriceVol_;             // key: id; value: pair(price, vol)
    vector<pair<float, int>> vec_price_vol_;  //<price, volume>;

    // internal use
    void sortPriceInGreaterOrder();
    int findPositionToReOrder(const float &price);

    map<string, pair<float, int>>::iterator
    getOrderIdItFromManager(const pair<float, int> &targetPair);
};

#endif  // end of SIDE_CONTAINER_HPP