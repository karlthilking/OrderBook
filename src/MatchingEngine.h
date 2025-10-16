#pragma once

#include "Types.h"
#include "OrderBook.h"
#include "Trade.h"
#include "Order.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Order;
class OrderBook;
class Trade;

using OrderBookMap = std::unordered_map<Symbol, std::unique_ptr<OrderBook>>;
using TradeList = std::vector<Trade>;

class MatchingEngine {
private:
    OrderBookMap order_books_;
    TradeList executed_trades_;
    TradeId next_trade_id_;
    Timestamp current_timestamp_;
    OrderBook* create_order_book(const Symbol& symbol);
    OrderBook* find_order_book(const Symbol& symbol);
    TradeList match_order(Order* order, OrderBook* order_book);
    Trade create_trade(Order* buy_order, Order* sell_order, Price price, Quantity quantity);
    Timestamp get_current_timestamp() const;
    static Side determine_aggressor(Order* incoming_order) const;
public:
    MatchingEngine();
    TradeList submit_order(Order* order);
    void cancel_order(const Symbol& symbol, OrderId order_id);
    const OrderBook* get_order_book(const Symbol& symbol);
    const TradeList& get_executed_trades() const;
    TradeList get_trades_for_symbol(const Symbol& symbol) const;
    bool has_order_book(const Symbol& symbol) const;
    OrderCount get_order_book_count() const;
    TradeId get_next_trade_id() const;
    std::string to_string() const;
};
