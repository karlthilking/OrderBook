#pragma once

#include "Types.h"
#include <string>

class Order {
private:
    OrderId order_id_;
    Side side_;
    Price price_;
    Quantity quantity_;
    Quantity remaining_quantity_;
    const Symbol& symbol_;
    Timestamp timestamp_;
    OrderType order_type_;
    OrderStatus status_;
public:
    Order(OrderId order_id, Side side, Price price, Quantity quantity, const Symbol& symbol,
          Timestamp timestamp, OrderType order_type);
    OrderId get_order_id() const;
    Side get_side() const;
    Price get_price() const;
    Quantity get_quantity() const;
    Quantity get_remaining_quantity() const;
    Quantity get_filled_quantity() const;
    const Symbol& get_symbol() const;
    Timestamp get_timestamp() const;
    OrderType get_order_type() const;
    OrderStatus get_status() const;
    void fill(Quantity);
    void cancel();
    bool is_filled() const;
    bool is_partially_filled() const;
    bool is_valid() const;
    static bool is_valid_side(Side side);
    static bool is_valid_price(Price price);
    static bool is_valid_quantity(Quantity quantity);
    bool can_match_with(const Order& other) const;
    Quantity get_fillable_quantity(const Order& other) const;
    std::string to_string() const;
    bool operator==(const Order& other) const;
    bool operator<(const Order& other) const;
};