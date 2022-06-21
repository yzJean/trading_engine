#include <iostream>

#include "matchingEngine.hpp"

using namespace std;

OrderBook::OrderBook(const MatchingEngine& engine) : engine_(engine) {
    vecRemainedInfo_.clear();
}

OrderBook::~OrderBook() {}

//input: SideContainer vector<price, volume>
//output: vector<bid_price, bid_volume, ask_price, ask_volume)
// bid price/ask price:
// The term "bid" refers to the highest price a buyer will pay to buy a specified number of shares of a stock at any given time.
// The term "ask"  refers to the lowest price at which a seller will sell the stock.
void OrderBook::calRemainedInfo() {
    SideContainer buyContainer = getSideContainer(macBUY);
    SideContainer sellContainer = getSideContainer(macSELL);
    int sizeBuy = buyContainer.size();
    int sizeSell = sellContainer.size();
#ifdef DEBUG_CAL_ORDERBOOK
    cout << "(calRemainedInfo) mp sizeBuy= " << sizeBuy << ", sizeSell= " << sizeSell << endl;
    cout << "(calRemainedInfo) vec sizeBuy= " << buyContainer.vecSize() << ", sizeSell= " << sellContainer.vecSize() << endl;
#endif
    // merge same price into one slot with volume updated
    auto mergePrice = [](const size_t& size, SideContainer& container) {
        for (size_t currentInex = 1u; currentInex < size; currentInex++) {
            size_t prevIndex = currentInex - 1u;
            pair<float, int> prevPiar = container.getPriceVolPairAtIndex(currentInex - 1u);
            pair<float, int> currentPiar = container.getPriceVolPairAtIndex(currentInex);
            float prevPrice = prevPiar.first;
            int prevVol = prevPiar.second;
            float price = currentPiar.first;
            int vol = currentPiar.second;
            if (price == prevPrice) {
                container.updateVolumeAt(prevIndex, prevVol + vol);
                container.updateVolumeAt(currentInex, 0);
            }
        }
    };
    mergePrice(sizeBuy, buyContainer);
    mergePrice(sizeSell, sellContainer);

    int sizeToIterate = max(sizeBuy, sizeSell);
    int lastBuyIndex = 0, lastSellIndex = sizeSell - 1;
    for (int i = 0; i < sizeToIterate; i++) {
        //premise: both buy and sell containers are sorted in the greater order.
        //bid price: refers to the highest price a buyer will pay to buy a specified number of shares of a stock at any given time.
        string bidPrice = "", bidVol = "", askPrice = "", askVol = "";

        bool bIsBidEmpty = true;
        for (int buyIndex = lastBuyIndex; buyIndex < sizeBuy; buyIndex++) {
            pair<float, int> buyPair = buyContainer.getPriceVolPairAtIndex(buyIndex);
            float fBidPrice = buyPair.first;
            int nBidVol = buyPair.second;
            bIsBidEmpty = nBidVol <= 0;
            if (bIsBidEmpty) {
                lastBuyIndex++;
                continue;
            }
            bidPrice = util::stringProcessor(fBidPrice);
            bidVol = to_string(nBidVol);
            lastBuyIndex++;
            break;
        }

        bool bIsAskEmpty = true;
        for (int sellIndex = lastSellIndex; sellIndex >= 0; sellIndex--) {
            //ask price: refers to the lowest price at which a seller will sell the stock.
            pair<float, int> sellPair = sellContainer.getPriceVolPairAtIndex(sellIndex);
            float fAskPrice = sellPair.first;
            int nAskVol = sellPair.second;
            bIsAskEmpty = nAskVol <= 0;
            if (bIsAskEmpty) {
                lastSellIndex--;
                continue;
            }
            askPrice = util::stringProcessor(fAskPrice);
            askVol = to_string(nAskVol);
            lastSellIndex--;
            break;
        }

        string strBidAskInfo = bidPrice + "," + bidVol + "," + askPrice + "," + askVol;
#ifdef DEBUG_CAL_ORDERBOOK
        cout << "Information about bid and ask price= " << strBidAskInfo << endl;
#endif
        bool bAreBothSidesEmpty = bIsBidEmpty && bIsAskEmpty;
        if (bAreBothSidesEmpty) {
#ifdef DEBUG_CAL_ORDERBOOK
            cout << "skipped" << endl;
#endif
            continue;
        }
        addRemainedInfo(strBidAskInfo);
    }
}

//(bid_price, bid_volume, ask_price, ask_volume)
void OrderBook::addRemainedInfo(const string& info) {
    vecRemainedInfo_.emplace_back(info);
}

const vector<string>& OrderBook::getRemainedInfo() {
    return vecRemainedInfo_;
}

const SideContainer& OrderBook::getSideContainer(const string& side) {
    return engine_.getSideContainer(side);
}