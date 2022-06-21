#include "main.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "matchingEngine.hpp"

using namespace std;

vector<string> run(vector<string> const& input) {
    map<string, shared_ptr<MatchingEngine>> mpMatchingEngines;  //key: companyName; value: matchingEngine
    map<string, string> mpOrderIdCompany;                       // for mapping between orderId and companyName, key: orderId; value: companyName;

    // process incoming commands
    for (auto&& strCmdInput : input) {
        cout << "Cmd: " << strCmdInput << endl;
        vector<string> spiltedInput = util::getSpiltedCmd(strCmdInput);
        util::CMD cmdCate = util::getCmdCategory(spiltedInput);
        if (cmdCate == util::CMD::PULL) {
            const string strOderId = spiltedInput[1];
            string strCompanyName = mpOrderIdCompany[strOderId];
            shared_ptr<MatchingEngine> engine = mpMatchingEngines.at(strCompanyName);
            engine->removeFromContainer(strOderId);

        } else if (cmdCate == util::CMD::AMEND) {
            const string strOderId = spiltedInput[1];
            const float fPrice = stof(spiltedInput[2]);
            const int nVolume = stoi(spiltedInput[3]);
            string strCompanyName = mpOrderIdCompany[strOderId];
            shared_ptr<MatchingEngine> engine = mpMatchingEngines.at(strCompanyName);
            engine->reOrder(strOderId, fPrice, nVolume);

        } else {  // insert
            const string strOperation = spiltedInput[0];
            const string strOderId = spiltedInput[1];
            const string strCompanyName = spiltedInput[2];
            const string strSide = spiltedInput[3];
            const float fPrice = stof(spiltedInput[4]);
            const int nVolume = stoi(spiltedInput[5]);

            mpOrderIdCompany.insert(make_pair(strOderId, strCompanyName));

            // if company engine doesn't exist, then create and add new engine to the container
            if (mpMatchingEngines.find(strCompanyName) == mpMatchingEngines.end()) {
                shared_ptr<MatchingEngine> ptrEng = shared_ptr<MatchingEngine>(new MatchingEngine(strCompanyName));
                mpMatchingEngines.insert(make_pair(strCompanyName, ptrEng));
            }
            shared_ptr<MatchingEngine> engine = mpMatchingEngines.at(strCompanyName);
            engine->addToSideContainer(strOderId, strSide, fPrice, nVolume);
            engine->matching();
        }
    }

    // cout << "Start to parse trades in chronological order..." << endl;
    vector<pair<int, string>> allTrades;  //chronological, key: sellOrderId, value: trades(<company_name>,<price>,<volume>,<aggressive_order_id>,<passive_order_id>)
    allTrades.reserve(100);
    for (auto&& enginePair : mpMatchingEngines) {
        string companyName = enginePair.first;
        auto matchedTrades = enginePair.second->getMatchedTrades();
        allTrades.insert(allTrades.end(), matchedTrades.begin(), matchedTrades.end());
    }
    sort(allTrades.begin(), allTrades.end(), [](const pair<int, string>& p1, const pair<int, string>& p2) {
        return p1.first < p2.first;
    });
    vector<string> ans;
    for (auto&& pairTrade : allTrades) {
        ans.emplace_back(pairTrade.second);
    }

    // cout << "Start to parse order books..." << endl;
    vector<string> allOrderBooks;  // element = "<bid_price>,<bid_volume>,<ask_price>,<ask_volume>"
    allOrderBooks.reserve(100);
    for (auto&& enginePair : mpMatchingEngines) {
        string companyName = "===" + enginePair.first + "===";
        OrderBook orderBook(*(enginePair.second));
        orderBook.calRemainedInfo();
        ans.emplace_back(companyName);
        ans.insert(ans.end(), orderBook.getRemainedInfo().begin(), orderBook.getRemainedInfo().end());
    }

    cout << "---------- Answer: Start ----------" << endl;
    for (auto&& a : ans) {
        cout << a << endl;
    }
    cout << "---------- Answer: End ------------" << endl;
    return ans;
}

/**
if (amend) reOrder();
if (pull) remove();
add();

matching();
input: containers for both buy and sell
output: matched id pair(price, sell, buy)

result();
input: matched id pair, vector<volume>, vector<price>
output: 
    getMatchedIdPair();
    getRemainedInfo();
        input: vector<price, volume>, vector<>
        output: (bid_price, bid_volume) or (ask_price, ask_volume)
            if (volume != 0) {
                
            }

company:
buy
mp_id_pos <id, pos> 
key: 1, 2, 3, 4, 5, 6, 7
val: 0, 1, 2, 3, 4, 5, 6

vector<price, volume>
10, 10, 7, 4, 4, 3, 2, 1
10, 10, 10, 4, 4, 3, 2, 1

vector<volume>
1,  2, 3, 4, 6, 7, 8, 9
1,  2, 3, 4, 6, 7, 8, 9

sell
mp_id_pos <id, pos> 
key: 1, 2, 3, 4, 5, 6, 7
val: 0, 1, 2, 3, 4, 5, 6

vector<price, volume>
9, 9, 5, 3, 4, 5, 6, 2

vector<volume>
2, 1, 4, 5, 6, 7, 8, 9


1. AMEND: determine the sequence in the buy container
a) both price and volume are up: 
    - lose time priority under the same newly price if there're higher price
b) price remained as the same but volume increase:
    - lose the time priority under the same price
c) price remained as the same but volume decrese: no updates
d) price is down and volume is up (or down): lose time priority

Time priority loss:
if (volume_increase) {
    re-roder
} else { //remained or descrease
     if (price_decrease || price_increase) {
         re-order
     }
}

void reOrder(int id){
    find the tail position of new_price and insert new_price, meanwhile remove one
    
}
*/

/**
matching rules:
Priority:
best price
early time matched first

operations
1. pull: removes the order from the order book

2. amend: causes the order to lose time priority in the order book, unless the only change to the order is that the volume is decreased.
--> re-evaluate for the potential matches

aggresive order: when a trader executes the order to buy or sell straightaway.
passive order: when traders set a price that stock must reach before they go ahead with buying or selling

bid price/ask price:
The term "bid" refers to the highest price a buyer will pay to buy a specified number of shares of a stock at any given time.  The term "ask"  refers to the lowest price at which a seller will sell the stock.

returns:
1. matched trades
2. the situation of the rest orders which has exluded the matched results from the book
*/