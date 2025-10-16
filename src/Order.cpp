#include "Types.h"
#include "Order.h"

#include <stdexcept>
#include <string>

Order::Order(OrderId order_id, Side side, Price price, Quantity quantity, const Symbol& symbol,
        Timestamp timestamp, OrderType order_type)
    : order_id_(order_id),
      side_(side),
      price_(price),
      quantity_(quantity),
      remaining_quantity_(quantity),
      symbol_(symbol),
      timestamp_(timestamp),
      order_type_(order_type),
      status_(OrderStatus::Pending) {
}
OrderId Order::get_order_id() const { return order_id_; }
Side Order::get_side() const { return side_; }
Price Order::get_price() const { return price_; }
Quantity Order::get_quantity() const { return quantity_; }
Quantity Order::get_remaining_quantity() const { return remaining_quantity_; }
Quantity Order::get_filled_quantity() const { return quantity_ - remaining_quantity_; }
const Symbol& Order::get_symbol() const { return symbol_; }
Timestamp Order::get_timestamp() const { return timestamp_; }
OrderType Order::get_order_type() const { return order_type_; }
OrderStatus Order::get_status() const { return status_; }

void Order::fill(Quantity quantity) {
    if (quantity > remaining_quantity_) {
        throw std::invalid_argument("Fill quantity cannot exceed remaining quantity");
    }
    remaining_quantity_ -= quantity;
    if (remaining_quantity_ == 0) {
        status_ = OrderStatus::Filled;
    }
}
void Order::cancel() {
    status_ = OrderStatus::Cancelled;
    remaining_quantity_ = 0;
}
bool Order::is_filled() const {
    return remaining_quantity_ == 0;
}
bool Order::is_partially_filled() const {
    return remaining_quantity_ > 0 && remaining_quantity_ < quantity_;
}
bool Order::is_valid() const {
    return (
        is_valid_side(side_) && is_valid_price(price_) &&
        is_valid_quantity(quantity_) && remaining_quantity_ <= quantity_ &&
        !symbol_.empty()
    );
}
bool Order::is_valid_side(Side side) { return (side == Side::Sell || side == Side::Buy); }
bool Order::is_valid_price(Price price) { return price > 0; }
bool Order::is_valid_quantity(Quantity quantity) { return quantity > 0; }

bool Order::can_match_with(const Order& other) const {
    bool valid_price = false;
    if (side_ == Side::Buy && price_ >= other.price_) {
        valid_price = true;
    }
    else if (side_ == Side::Sell && price_ <= other.price_) {
        valid_price = true;
    }
    return (
        side_ != other.side_ && symbol_ == other.symbol_ && remaining_quantity_ > 0 &&
        other.remaining_quantity_ > 0 && valid_price
    );
}
Quantity Order::get_fillable_quantity(const Order& other) const {
    if (!can_match_with(other)) {
        return 0;
    }
    return std::min(remaining_quantity_, other.remaining_quantity_);
}
std::string Order::to_string() const {
    std::string side_str = (side_ == Side::Buy) ? "Buy" : "Sell";
    std::string type_str = (order_type_ == OrderType::Market) ? "Market" : "Limit";
    return (
        "Order [ID: " + std::to_string(order_id_) + ", Symbol: " + symbol_ + ", Price: " +
        std::to_string(price_) + ", Side: " + side_str + ", Quantity: " +
        std::to_string(quantity_) + ", Type: " + type_str + ", Timestamp: " +
        std::to_string(timestamp_) + "]\n"
    );
}
bool Order::operator==(const Order& other) const {
    return order_id_ == other.order_id_;
}
bool Order::operator<(const Order& other) const {
    if (side_ != other.side_) {
        throw std::logic_error("Can not compare the priority of differing sides");
    }
    if (side_ == Side::Buy && price_ != other.price_) {
        return price_ > other.price_;
    }
    else if (side_ == Side::Sell && price_ != other.price_) {
        return price_ < other.price_;
    }
    return timestamp_ < other.timestamp_;
}