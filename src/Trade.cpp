#include "Types.h"
#include "Trade.h"

#include <string>
#include <stdexcept>

Trade::Trade(TradeId trade_id, OrderId buy_id, OrderId sell_id, const Symbol& symbol,
             Price price, Quantity quantity, Timestamp timestamp, Side aggressor)
             : trade_id_(trade_id),
               buy_order_id_(buy_id),
               sell_order_id_(sell_id),
               symbol_(symbol),
               price_(price),
               quantity_(quantity),
               timestamp_(timestamp),
               aggressor_(aggressor) {

}

TradeId Trade::get_trade_id() const { return trade_id_; }
OrderId Trade::get_buy_id() const { return buy_order_id_; }
OrderId Trade::get_sell_id() const { return sell_order_id_; }
const Symbol& Trade::get_symbol() const { return symbol_; }
Price Trade::get_price() const { return price_; }
Quantity Trade::get_quantity() const { return quantity_; }
Timestamp Trade::get_timestamp() const { return timestamp_; }
Side Trade::get_aggressor_side() const { return aggressor_; }

std::string Trade::to_string() const {
    std::string side_str = (aggressor_ == Side::Buy) ? "Buyer" : "Seller";
    return (
        "Trade: [ID: " + std::to_string(trade_id_) + ", Symbol: " + symbol_ +
        ", Price: " + std::to_string(price_) + ", Qty: " + std::to_string(quantity_) +
        ", Buy Order ID: " + std::to_string(buy_order_id_) + ", Sell Order ID: " +
        std::to_string(sell_order_id_) + ", Aggressor: " + side_str + ", Timestamp: " +
        std::to_string(timestamp_)
    );
}

bool Trade::operator==(const Trade &other) const {
    return trade_id_ == other.trade_id_;
}