#include "sideContainer.hpp"

vector<pair<float, int>> &SideContainer::getPriceVolContainer() {
    return vec_price_vol_;
}

SideContainer::SideContainer() { vec_price_vol_.clear(); };

size_t SideContainer::size() { return mp_orderId_pairPriceVol_.size(); };

size_t SideContainer::vecSize() { return vec_price_vol_.size(); };

bool SideContainer::empty() { return mp_orderId_pairPriceVol_.empty(); }

pair<float, int> SideContainer::top() { return vec_price_vol_.front(); }

pair<float, int> SideContainer::last() { return vec_price_vol_.back(); }

map<string, pair<float, int>>::iterator
SideContainer::getOrderIdItFromManager(const pair<float, int> &targetPair) {
    auto it =
        find_if(mp_orderId_pairPriceVol_.begin(), mp_orderId_pairPriceVol_.end(),
                [&targetPair](const auto &m) { return m.second == targetPair; });
#ifdef DEBUG_SIDECONTAINER
//   if (it == mp_orderId_pairPriceVol_.end()) {
//     printOrderContainer();
//     cout << "(getOrderIdItFromManager) target pair: " << targetPair.first
//          << ", " << targetPair.second << endl;
//     cout << "(getOrderIdItFromManager) it is end" << endl;
//   }
#endif
    return it;
}

string
SideContainer::getOrderIdFromManager(const pair<float, int> &targetPair) {
    auto it = getOrderIdItFromManager(targetPair);
    if (it == mp_orderId_pairPriceVol_.end()) {
        return "";
    }
    return it->first;
}

void SideContainer::eraseAt(const int &index) {
    if (index >= vecSize()) {
        return;
    }
    pair<float, int> pairFoundFromVec = vec_price_vol_.at(index);
    // erase from container
    vec_price_vol_.erase(vec_price_vol_.begin() + index);

    // erase from manager
    auto it = getOrderIdItFromManager(pairFoundFromVec);
    mp_orderId_pairPriceVol_.erase(it);
}

void SideContainer::updateVolumeAt(const int &index, const int &updatedVolume) {
    pair<float, int> pairFoundFromVec = vec_price_vol_.at(index);
    // update container
    vec_price_vol_[index].second = updatedVolume;

    // update manager
#ifdef DEBUG_SIDECONTAINER
    cout << "(updateVolumeAt) pair before update: " << pairFoundFromVec.first
         << ", " << pairFoundFromVec.second << endl;
#endif
    string orderId = getOrderIdFromManager(pairFoundFromVec);
    mp_orderId_pairPriceVol_[orderId].second = updatedVolume;
}

pair<float, int> SideContainer::getPriceVolPairAtIndex(const int &index) {
    if (index >= vecSize()) {
        return pair<float, int>(-1.0f, -1);
    }
    return vec_price_vol_.at(index);
}

void SideContainer::add(const string &strId, const float &price,
                        const int &volume) {
    vec_price_vol_.emplace_back(make_pair(price, volume));
    sortPriceInGreaterOrder();
    mp_orderId_pairPriceVol_.insert(make_pair(strId, make_pair(price, volume)));
}

void SideContainer::pull(const string &strId) {
    if (mp_orderId_pairPriceVol_.find(strId) != mp_orderId_pairPriceVol_.end()) {
        pair<float, int> pairFound = mp_orderId_pairPriceVol_.at(strId);
        mp_orderId_pairPriceVol_.erase(strId);
        auto it = std::find(vec_price_vol_.begin(), vec_price_vol_.end(), pairFound);
        vec_price_vol_.erase(it);
    }
}

void SideContainer::reOrder(const string &strId, const float &price,
                            const int &volume) {
    if (mp_orderId_pairPriceVol_.find(strId) != mp_orderId_pairPriceVol_.end()) {
        pair<float, int> pairFound = mp_orderId_pairPriceVol_.at(strId);

        // found pair from vector before modified
        auto vecIt =
            std::find(vec_price_vol_.begin(), vec_price_vol_.end(), pairFound);

        // keep the original
        float orgPrice = pairFound.first;
        int orgVol = pairFound.second;

        // update to the latest info
        pair<float, int> updatedPair = make_pair(price, volume);
#ifdef DEBUG_SIDECONTAINER
        cout << "(reOrder) updatedPair.first= " << updatedPair.first
             << ", updatedPair.second= " << updatedPair.second << endl;
#endif
        mp_orderId_pairPriceVol_[strId] = updatedPair;

        // volume_increase || price_increase || price_descrease, reorder
        bool bIsVolIncreased = volume > orgVol;
        bool bIsPriceRemainedSame = orgPrice == price;
        if (bIsVolIncreased || !bIsPriceRemainedSame) {
            // remove the pair with original price and volume from container
            vec_price_vol_.erase(vecIt);

            // insert pair to new position
            auto itToInsert = vec_price_vol_.begin() + findPositionToReOrder(price);
            vec_price_vol_.insert(itToInsert, updatedPair);
        } else {
            // if the only change is to descrease volume, then update volume only.
            vecIt->second = volume;
        }
    }
}

int SideContainer::findPositionToReOrder(const float &price) {
    vector<pair<float, int>> vecReversed(vec_price_vol_);
    // after reverse the vector, the order of elements in this container is in
    // less order.
    reverse(vecReversed.begin(), vecReversed.end());
    auto it2 =
        find_if(vecReversed.begin(), vecReversed.end(),
                [&price](pair<float, int> pair) { return pair.first == price; });
    if (it2 == vecReversed.end()) {
        return 0;
    }
    return vecReversed.size() - (it2 - vecReversed.begin());
}

void SideContainer::sortPriceInGreaterOrder() {
    struct {
        bool operator()(const pair<float, int> &p1,
                        const pair<float, int> &p2) const {
            return p1.first > p2.first;
        }
    } customGreater;
    std::sort(vec_price_vol_.begin(), vec_price_vol_.end(), customGreater);
}

void SideContainer::printChronologicalContainer() {
    cout << "chronological container: " << endl;
    for (auto &&v : vec_price_vol_) {
        cout << "(" << v.first << ", " << v.second << ")" << endl;
    }
}

void SideContainer::printOrderContainer() {
    cout << "order container: " << endl;
    for (auto &&m : mp_orderId_pairPriceVol_) {
        cout << "(" << m.first << ", " << m.second.first << ", " << m.second.second
             << ")" << endl;
    }
}
