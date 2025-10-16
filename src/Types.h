#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <queue>
#include <map>

using OrderId = uint64_t;
using TradeId = uint64_t;
using Price = uint32_t;
using Quantity = uint64_t;
using Timestamp = uint64_t;
using Symbol = std::string;
using OrderCount = size_t;

enum class Side {
    Buy, Sell
};

enum class OrderStatus {
    Pending, Partially_Filled, Filled, Cancelled
};

enum class OrderType {
    Market, Limit
};

struct MarketDepthLevel {
    Price price;
    Quantity total_qty;
    OrderCount order_count;

    MarketDepthLevel(Price p, Quantity qty, OrderCount count) :
        price(p),
        total_qty(qty),
        order_count(count) {
    }
};
