#include "matchingEngine.hpp"

#include <iostream>
using namespace std;

MatchingEngine::MatchingEngine(const string& name) : strEngineName_(name) {}

MatchingEngine::~MatchingEngine() {}

const string MatchingEngine::getEngineName() {
    return strEngineName_;
}

// data processing
void MatchingEngine::addToSideContainer(const string& strId, const string& side, const float& price, const int& volume) {
    SideContainer& sideContainer = getSideContainer(side);
    sideContainer.add(strId, price, volume);
    addToSideOrderIdManager(strId, side);

#ifdef DEBUG_INSERT
    cout << "(addToSideContainer) print orderId= " << strId << ", " << side << ": " << endl;
    sideContainer.printChronologicalContainer();
    sideContainer.printOrderContainer();
#endif
}

void MatchingEngine::addToSideOrderIdManager(const string& orderId, const string& side) {
    mpSideOrderIdManger_.insert(make_pair(orderId, side));
}

string MatchingEngine::getSideFromOrderIdManager(const string& orderId) {
    return mpSideOrderIdManger_.at(orderId);
}

void MatchingEngine::removeFromContainer(const string& strId) {
    SideContainer& sideContainer = getSideContainer(getSideFromOrderIdManager(strId));
    sideContainer.pull(strId);
}

void MatchingEngine::reOrder(const string& strId, const float& price, const int& volume) {
    string side = getSideFromOrderIdManager(strId);
    SideContainer& sideContainer = getSideContainer(side);
    sideContainer.reOrder(strId, price, volume);
}

// main matching
//input: containers for both buy and sell
//output: matched id pair(sellId, "price, volume, sellId, buyId")
void MatchingEngine::matching() {
    if (!doesStartToMatch()) {
        //cout << "Stop matching as not enough element in either buy or sell container." << endl;
        return;
    }

    SideContainer& localBuyContainer = getSideContainer(macBUY);
    SideContainer& localSellContainer = getSideContainer(macSELL);
    if (localBuyContainer.top().first < localSellContainer.last().first) {
        //cout << "Stop matching as top bid price is less than last ask price." << endl;
        return;
    }
#ifdef DEBUG_MATCHING
    cout << "(matching) trading" << endl;
#endif
    for (int buyIndex = 0; buyIndex < localBuyContainer.size(); buyIndex++) {
        pair<float, int> pairTopBuy = localBuyContainer.getPriceVolPairAtIndex(buyIndex);
        float buyPrice = pairTopBuy.first;
        int volToBuy = pairTopBuy.second;
        if (volToBuy <= 0) {
            continue;
        }
        for (int sellIndex = 0; sellIndex < localSellContainer.size(); sellIndex++) {
            pair<float, int> pairTopSell = localSellContainer.getPriceVolPairAtIndex(sellIndex);
            float sellPrice = pairTopSell.first;
            int volToBeSold = pairTopSell.second;
            if (volToBeSold <= 0) {
                continue;
            }
            bool bIsSoldOut = max(volToBeSold, 0) == 0;
#ifdef DEBUG_MATCHING
            cout << "(matching) sellIndex= " << sellIndex << endl;
#endif
            if (buyPrice >= sellPrice && !bIsSoldOut) {
#ifdef DEBUG_MATCHING
                cout << "(matching) Print sell " << endl;
                localSellContainer.printChronologicalContainer();
                localSellContainer.printOrderContainer();
                cout << "(matching) sellPrice= " << sellPrice << ", volToBeSold= " << volToBeSold << endl;

                cout << "(matching) Print buy " << endl;
                localBuyContainer.printChronologicalContainer();
                localBuyContainer.printOrderContainer();
                cout << "(matching) buyPrice= " << buyPrice << ", volToBuy= " << volToBuy << endl;
#endif
                int soldVol = max(min(volToBuy, volToBeSold), 0);
#ifdef DEBUG_MATCHING
                cout << "(matching) soldVol= " << soldVol << endl;
#endif
                string sellOrderId = localSellContainer.getOrderIdFromManager(pairTopSell);
                string buyOrderId = localBuyContainer.getOrderIdFromManager(pairTopBuy);
                if (sellOrderId == "" || buyOrderId == "") {
                    continue;
                }

                // "price, volume, sell, buy"
                string strBuyPrice = util::stringProcessor(buyPrice);
                string tradeInfo = getEngineName() + "," + strBuyPrice + "," + to_string(soldVol) + "," + sellOrderId + "," + buyOrderId;

                //update
#ifdef DEBUG_SIDECONTAINER
                cout << "sell updateVolumeAt: " << endl;
#endif
                localSellContainer.updateVolumeAt(sellIndex, volToBeSold - soldVol);
#ifdef DEBUG_SIDECONTAINER
                cout << "buy updateVolumeAt: " << endl;
#endif
                localBuyContainer.updateVolumeAt(buyIndex, volToBuy - soldVol);
                volToBuy = volToBuy - soldVol;
#ifdef DEBUG_MATCHING
                cout << "(matching) sell order id: " << sellOrderId << ", trade info: " << tradeInfo << endl;
#endif
                addMatchedTrade(make_pair(stoi(sellOrderId), tradeInfo));

                pair<float, int> updatedBuyPair = localSellContainer.getPriceVolPairAtIndex(buyIndex);
                pairTopBuy = updatedBuyPair;
                float updatedBuyPrice = updatedBuyPair.first;
                int updatedBuyVol = updatedBuyPair.second;
                if (updatedBuyVol <= 0) {  // avoid redundent searching
#ifdef DEBUG_MATCHING
                    cout << "(matching) updatedBuyPrice= " << updatedBuyPrice << ", updatedBuyVol= " << updatedBuyVol << endl;
#endif
                    break;
                }
            }
        }
    }
}

//input:  containers for both buy and sell
//output: !sell.empty() && !buy.empty()
bool MatchingEngine::doesStartToMatch() {
    SideContainer localBuyContainer = getSideContainer(macBUY);
    SideContainer localSellContainer = getSideContainer(macSELL);
    if (!localBuyContainer.empty() && !localSellContainer.empty()) {
        return true;
    }
    // if (localBuyContainer.empty()) {
    //     cout << "Not enough elements in buy container." << endl;
    // }
    // if (localSellContainer.empty()) {
    //     cout << "Not enough elements in sell container." << endl;
    // }
    return false;
}

// (sellId, "price, volume, sell, buy")
void MatchingEngine::addMatchedTrade(const pair<int, string>& matchedTrade) {
    vecMatchedTrades_.emplace_back(matchedTrade);
}

const vector<pair<int, string>> MatchingEngine::getMatchedTrades() {
    return vecMatchedTrades_;
}

SideContainer& MatchingEngine::getSideContainer(const string& side) {
    if (side == macBUY) {
        return buyContainer;
    }
    return sellContainer;
}