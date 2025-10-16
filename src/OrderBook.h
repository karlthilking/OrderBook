#pragma once

#include "Types.h"
#include <queue>
#include <unordered_map>
#include <optional>
#include <map>

class Order;
using Asks = std::map<Price, std::queue<Order*>, std::less<Price>>;
using Bids = std::map<Price, std::queue<Order*>, std::greater<Price>>;

class OrderBook {
private:
    Asks ask_levels_;
    Bids bid_levels_;
    std::unordered_map<OrderId, Order*> order_lookup_;
    Symbol symbol_;
    OrderCount total_orders_;
    void remove_empty_price_level(Price price, Side side);
    std::queue<Order*>& get_price_level(Price price, Side side);
public:
    OrderBook(const Symbol& symbol);
    void add_order(Order* order);
    void cancel_order(OrderId order_id);
    std::optional<Price> get_best_bid() const;
    std::optional<Price> get_best_ask() const;
    std::optional<Price> get_spread() const;
    std::vector<MarketDepthLevel> get_market_depth(int levels, Side side) const;
    bool is_empty() const;
    OrderCount get_order_count() const;
    const Symbol& get_symbol() const;
    OrderCount get_bid_level_count() const;
    OrderCount get_ask_level_count() const;
    std::queue<Order*>* get_best_orders(Side incoming_side);
    bool is_valid_order(const Order& order) const;
    std::string to_string() const;
    void cleanup_empty_price_level(Price price, Side side);
};