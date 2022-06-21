#ifndef MATCHING_ENGINE_HPP
#define MATCHING_ENGINE_HPP

#include "sideContainer.hpp"

using namespace std;
#define macBUY "BUY"
#define macSELL "SELL"

class OrderBook;
class MatchingEngine {
    friend OrderBook;

   public:
    MatchingEngine(const string& name = "");
    ~MatchingEngine();
    const string getEngineName();

    // data processing
    void addToSideContainer(const string& strId, const string& side, const float& price, const int& volume);
    void removeFromContainer(const string& strId);
    void reOrder(const string& strId, const float& price, const int& volume);

    // main matching
    //input: containers for both buy and sell
    //output: matched id pair(price, volume, sell, buy)
    void matching();

    //input:  containers for both buy and sell
    //output: !sell.empty() && !buy.empty()
    bool doesStartToMatch();
    void addMatchedTrade(const pair<int, string>& matchedTrade);  // (sellId, "price, volume, sell, buy")
    const vector<pair<int, string>> getMatchedTrades();

   private:
    string strEngineName_;

    SideContainer buyContainer;
    SideContainer sellContainer;
    SideContainer& getSideContainer(const string& side);
    const string& getSideWithOrderId(const string& orderId);
    void addToSideOrderIdManager(const string& orderId, const string& side);
    string getSideFromOrderIdManager(const string& orderId);
    map<string, string> mpSideOrderIdManger_;  //key: orderId; value: side

    vector<pair<int, string>> vecMatchedTrades_;  // (sellId, "price, volume, sellId, buyId")
};

class OrderBook {
   public:
    OrderBook(const MatchingEngine& engine);
    ~OrderBook();

    //input: SideContainer vector<price, volume>
    //output: vector<bid_price, bid_volume, ask_price, ask_volume)
    void calRemainedInfo();
    void addRemainedInfo(const string& info);
    const vector<string>& getRemainedInfo();  //(bid_price, bid_volume, ask_price, ask_volume)

   private:
    vector<string> vecRemainedInfo_;

    MatchingEngine engine_;
    const SideContainer& getSideContainer(const string& side);
};

#endif  //end of MATCHING_ENGINE_HPP