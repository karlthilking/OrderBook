#pragma once

#include "Types.h"
#include <string>

class Trade {
private:
    TradeId trade_id_;
    OrderId buy_order_id_;
    OrderId sell_order_id_;
    Symbol symbol_;
    Price price_;
    Quantity quantity_;
    Timestamp timestamp_;
    Side aggressor_;
public:
    Trade(TradeId trade_id, OrderId buy_id, OrderId sell_id, const Symbol& symbol,
          Price price, Quantity quantity, Timestamp timestamp, Side aggressor);
    TradeId get_trade_id() const;
    OrderId get_buy_id() const;
    OrderId get_sell_id() const;
    const Symbol& get_symbol() const;
    Price get_price() const;
    Quantity get_quantity() const;
    Timestamp get_timestamp() const;
    Side get_aggressor_side() const;
    std::string to_string() const;
    bool operator==(const Trade& other) const;
};