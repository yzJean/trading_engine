#include <catch2/catch.hpp>

#include "../src/main.hpp"

TEST_CASE("insert") {
    auto input = std::vector<std::string>();

    input.emplace_back("INSERT,1,AAPL,BUY,12.2,5");

    auto result = run(input);

    REQUIRE(result.size() == 2);
    CHECK(result[0] == "===AAPL===");
    CHECK(result[1] == "12.2,5,,");
}

TEST_CASE("simple match") {
    auto input = std::vector<std::string>();

    input.emplace_back("INSERT,1,AAPL,BUY,12.2,5");
    input.emplace_back("INSERT,2,AAPL,SELL,12.1,8");

    auto result = run(input);

    REQUIRE(result.size() == 3);
    CHECK(result[0] == "AAPL,12.2,5,2,1");
    CHECK(result[1] == "===AAPL===");
    CHECK(result[2] == ",,12.1,3");
}

TEST_CASE("multi insert and multi match") {
    auto input = std::vector<std::string>();

    input.emplace_back("INSERT,1,AAPL,BUY,14.235,5");
    input.emplace_back("INSERT,2,AAPL,BUY,14.235,6");
    input.emplace_back("INSERT,3,AAPL,BUY,14.235,12");
    input.emplace_back("INSERT,4,AAPL,BUY,14.234,5");
    input.emplace_back("INSERT,5,AAPL,BUY,14.23,3");
    input.emplace_back("INSERT,6,AAPL,SELL,14.237,8");
    input.emplace_back("INSERT,7,AAPL,SELL,14.24,9");
    input.emplace_back("PULL,1");
    input.emplace_back("INSERT,8,AAPL,SELL,14.234,25");

    auto result = run(input);

    REQUIRE(result.size() == 7);
    CHECK(result[0] == "AAPL,14.235,6,8,2");
    CHECK(result[1] == "AAPL,14.235,12,8,3");
    CHECK(result[2] == "AAPL,14.234,5,8,4");
    CHECK(result[3] == "===AAPL===");
    CHECK(result[4] == "14.23,3,14.234,2");
    CHECK(result[5] == ",,14.237,8");
    CHECK(result[6] == ",,14.24,9");
}

TEST_CASE("multi symbol") {
    auto input = std::vector<std::string>();

    input.emplace_back("INSERT,1,WEBB,BUY,0.3854,5");
    input.emplace_back("INSERT,2,TSLA,BUY,412,31");
    input.emplace_back("INSERT,3,TSLA,BUY,410.5,27");
    input.emplace_back("INSERT,4,AAPL,SELL,21,8");
    input.emplace_back("INSERT,11,WEBB,SELL,0.3854,4");
    input.emplace_back("INSERT,13,WEBB,SELL,0.3853,6");

    auto result = run(input);

    REQUIRE(result.size() == 9);
    CHECK(result[0] == "WEBB,0.3854,4,11,1");
    CHECK(result[1] == "WEBB,0.3854,1,13,1");
    CHECK(result[2] == "===AAPL===");
    CHECK(result[3] == ",,21,8");
    CHECK(result[4] == "===TSLA===");
    CHECK(result[5] == "412,31,,");
    CHECK(result[6] == "410.5,27,,");
    CHECK(result[7] == "===WEBB===");
    CHECK(result[8] == ",,0.3853,5");
}

TEST_CASE("amend") {
    auto input = std::vector<std::string>();

    input.emplace_back("INSERT,1,WEBB,BUY,45.95,5");
    input.emplace_back("INSERT,2,WEBB,BUY,45.95,6");
    input.emplace_back("INSERT,3,WEBB,BUY,45.95,12");
    input.emplace_back("INSERT,4,WEBB,SELL,46,8");
    input.emplace_back("AMEND,2,46,3");
    input.emplace_back("INSERT,5,WEBB,SELL,45.95,1");
    input.emplace_back("AMEND,1,45.95,3");
    input.emplace_back("INSERT,6,WEBB,SELL,45.95,1");
    input.emplace_back("AMEND,1,45.95,5");
    input.emplace_back("INSERT,7,WEBB,SELL,45.95,1");

    auto result = run(input);

    REQUIRE(result.size() == 6);
    CHECK(result[0] == "WEBB,46,3,2,4");
    CHECK(result[1] == "WEBB,45.95,1,5,1");
    CHECK(result[2] == "WEBB,45.95,1,6,1");
    CHECK(result[3] == "WEBB,45.95,1,7,3");
    CHECK(result[4] == "===WEBB===");
    CHECK(result[5] == "45.95,16,46,5");
}
