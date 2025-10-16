#include "Types.h"
#include "OrderBook.h"
#include "Order.h"

#include <queue>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <sstream>

OrderBook::OrderBook(const Symbol &symbol)
    : ask_levels_(),
      bid_levels_(),
      order_lookup_(),
      symbol_(symbol),
      total_orders_(0) {
}

void OrderBook::remove_empty_price_level(Price price, Side side) {
    if (side == Side::Buy) {
        auto it = bid_levels_.find(price);
        if (it != bid_levels_.end() && it->second.empty()) {
            bid_levels_.erase(it);
        }
        else {
            throw std::logic_error("Price level does not exist in Bids or price level is not empty");
        }
    }
    else {
        auto it = ask_levels_.find(price);
        if (it != ask_levels_.end() && it->second.empty()) {
            ask_levels_.erase(it);
        }
        else {
            throw std::logic_error("Price level does not exist in Asks or price level is not empty");
        }
    }
}
std::queue<Order*>& OrderBook::get_price_level(Price price, Side side) {
    if (side == Side::Buy) {
        auto it = bid_levels_.find(price);
        if (it != bid_levels_.end()) {
            return it->second;
        }
    }
    else {
        auto it = ask_levels_.find(price);
        if (it != ask_levels_.end()) {
            return it->second;
        }
    }
}

void OrderBook::add_order(Order* order) {
    if (!order) {
        throw std::invalid_argument("Order can not be null");
    }
    if (!order->is_valid()) {
        throw std::invalid_argument("Invalid order");
    }
    if (order_lookup_[order->get_order_id()]) {
        throw std::invalid_argument("OrderId already exists");
    }

    if (order->get_side() == Side::Buy) {
        bid_levels_[order->get_price()].push(order);
    }
    else {
        ask_levels_[order->get_price()].push(order);
    }
    order_lookup_[order->get_order_id()] = order;

}
void OrderBook::cancel_order(OrderId order_id) {
    auto it = order_lookup_.find(order_id);
    if (it == order_lookup_.end()) {
        throw std::invalid_argument("Can't cancel an nonexistent order");
    }
    Side side = it->second->get_side();
    Price price = it->second->get_price();
    if (side == Side::Buy) {
        std::queue<Order*> cur_bids_queue = get_price_level(price, side);
        std::queue<Order*> new_bids_queue;
        while (!cur_bids_queue.empty()) {
            Order* order = cur_bids_queue.front();
            cur_bids_queue.pop();
            if (order->get_order_id() != order_id) {
                new_bids_queue.push(order);
            }
        }
        if (new_bids_queue.empty()) {
            remove_empty_price_level(price, side);
        }
        else {
            bid_levels_[price] = new_bids_queue;
        }
    }
    else {
        std::queue<Order*> cur_asks_queue = get_price_level(price, side);
        std::queue<Order*> new_asks_queue;
        while (!cur_asks_queue.empty()) {
            Order* order = cur_asks_queue.front();
            cur_asks_queue.pop();
            if (order->get_order_id() != order_id) {
                new_asks_queue.push(order);
            }
        }
        if (new_asks_queue.empty()) {
            remove_empty_price_level(price, side);
        }
        else {
            ask_levels_[price] = new_asks_queue;
        }
    }
    order_lookup_.erase(it);
}

std::optional<Price> OrderBook::get_best_bid() const {
    if (bid_levels_.empty()) {
        return std::nullopt;
    }
    return bid_levels_.begin()->first;
}
std::optional<Price> OrderBook::get_best_ask() const {
    if (ask_levels_.empty()) {
        return std::nullopt;
    }
    return ask_levels_.begin()->first;
}
std::optional<Price> OrderBook::get_spread() const {
    auto best_bid = get_best_bid();
    auto best_ask = get_best_ask();

    if (!get_best_ask().has_value() || !get_best_bid().has_value()) {
        return std::nullopt;
    }
    return best_ask.value() - best_bid.value();
}

std::vector<MarketDepthLevel> OrderBook::get_market_depth(int levels, Side side) const {
    std::vector<MarketDepthLevel> depth;

    if (side == Side::Buy) {
        auto it = bid_levels_.begin();
        for (int i = 0; i < levels && it != bid_levels_.end(); ++i, ++it) {
            Quantity total_qty = 0;
            OrderCount order_count = 0;

            std::queue<Order*> temp_queue = it->second;
            while(!temp_queue.empty()) {
                total_qty += temp_queue.front()->get_remaining_quantity();
                order_count++;
                temp_queue.pop();
            }
            depth.emplace_back(it->first, total_qty, order_count);
        }
    }
    else {
        auto it = ask_levels_.begin();
        for (int i = 0; i < levels && it != ask_levels_.end(); ++i, ++it) {
            Quantity total_qty = 0;
            OrderCount order_count = 0;

            std::queue<Order*> temp_queue = it->second;
            while(!temp_queue.empty()) {
                total_qty += temp_queue.front()->get_remaining_quantity();
                order_count++;
                temp_queue.pop();
            }
            depth.emplace_back(it->first, total_qty, order_count);
        }
    }
    return depth;
}

bool OrderBook::is_empty() const { return order_lookup_.empty(); }
OrderCount OrderBook::get_order_count() const { return total_orders_; }
const Symbol& OrderBook::get_symbol() const { return symbol_; }
OrderCount OrderBook::get_bid_level_count() const { return bid_levels_.size(); }
OrderCount OrderBook::get_ask_level_count() const { return ask_levels_.size(); }

std::queue<Order*>* OrderBook::get_best_orders(Side incoming_side) {
    if (incoming_side == Side::Buy) {
        if (!ask_levels_.empty()) {
            return &ask_levels_.begin()->second;
        }
    }
    else {
        if (!bid_levels_.empty()) {
            return &bid_levels_.begin()->second;
        }
    }
    return nullptr;
}

bool OrderBook::is_valid_order(const Order& order) const {
    if (!order.is_valid() || order.get_symbol() != symbol_ ||
        order_lookup_.find(order.get_order_id()) != order_lookup_.end() ||
        order.get_remaining_quantity() <= 0) {
        return false;
    }
    return true;
}
std::string OrderBook::to_string() const {
    std::ostringstream oss;
    oss << "OrderBook[Symbol: " << symbol_
        << ", Total Order " << total_orders_
        << ", Bid Levels: " << get_bid_level_count()
        << ", Ask Levels: " << get_ask_level_count() << "]\n";

    auto best_bid = get_best_bid();
    auto best_ask = get_best_ask();

    if (best_bid.has_value() && best_ask.has_value()) {
        oss << "Best Bid: " << best_bid.value()
            << ", Best Ask: " << best_ask.value()
            << ", Market Spread: " << get_spread().value() << "\n";
    }
    else if (best_bid.has_value()) {
        oss << "Best Bid: " << best_bid.value() << " (No asks)\n";
    }
    else if (best_ask.has_value()) {
        oss << "Best Ask: " << best_ask.value() << " (No bids)\n";
    }
    else {
        oss << "Orderbook is empty\n";
    }
    oss << "\nBIDS:\n";
    auto bid_depth = get_market_depth(5, Side::Buy);
    for (const auto& level : bid_depth) {
        oss << "Price: " << level.price << ", Qty: " << level.total_qty
            << ", Count: " << level.order_count << "\n";
    }
    if (bid_depth.empty()) {
        oss << " No bids\n";
    }

    oss << "\nASKS:\n";
    auto ask_depth = get_market_depth(5, Side::Sell);
    for (const auto& level : ask_depth) {
        oss << "Price: " << level.price << ", Qty: " << level.total_qty
            << ", Count: " << level.order_count << "\n";
    }
    if (ask_depth.empty()) {
        oss << " No asks\n";
    }
    return oss.str();
}