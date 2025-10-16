#include "Types.h"
#include "MatchingEngine.h"
#include "OrderBook.h"
#include "Trade.h"
#include "Order.h"

#include <vector>
#include <unordered_map>
#include <memory>

MatchingEngine::MatchingEngine()
    : order_books_(),
      executed_trades_(),
      next_trade_id_() {
}

OrderBook* MatchingEngine::get_or_create_order_book(const Symbol& symbol) {
    auto it = order_books_.find(symbol);
    if (it != order_books_.end()) {
        return it->second.get();
    }
    auto order_book = std::make_unique<OrderBook>(symbol);
    OrderBook* ptr = order_book.get();
    order_books_[symbol] = std::move(order_book);
    return ptr;
}
OrderBook* MatchingEngine::find_order_book(const Symbol& symbol) {
    auto it = order_books_.find(symbol);
    if (it == order_books_.end()) {
        return nullptr;
    }
    return it->second.get();
}
TradeList MatchingEngine::match_order(Order* order, OrderBook* order_book) {
   TradeList trades;
   Side incoming_side = order->get_side();

   while (order->get_remaining_quantity() > 0) {
       auto* opposing_orders = order_book->get_best_orders(incoming_side);
       if (!opposing_orders || opposing_orders->empty()) {
           break;
       }
       Order* resting_order = opposing_orders->front();
       if (!order->can_match_with(*resting_order)) {
           break;
       }
       Quantity fill_qty = order->get_fillable_quantity(*resting_order);
       Price execution_price = resting_order->get_price();

       Order* buy_order = (incoming_side == Side::Buy) ? order : resting_order;
       Order* sell_order = (incoming_side == Side::Sell) ? order : resting_order;
       Trade trade = create_trade(buy_order, sell_order, execution_price, fill_qty, incoming_side);
       trades.push_back(trade);
       executed_trades_.push_back(trade);

       order->fill(fill_qty);
       resting_order->fill(fill_qty);
       if (resting_order->is_filled()) {
           opposing_orders->pop();
       }
   }
   return trades;
}
Trade MatchingEngine::create_trade(Order* buy_order, Order* sell_order, Price price, Quantity quantity, Side aggressor) {
    return Trade(next_trade_id_++, buy_order->get_order_id(), sell_order->get_order_id(), buy_order->get_symbol(),
                 price, quantity, get_current_timestamp(), aggressor);
}

TradeList MatchingEngine::submit_order(Order *order) {
    if(!order->is_valid()) {
        throw std::invalid_argument("Cannot submit invalid order");
    }
    const Symbol& symbol = order->get_symbol();
    OrderBook* ptr = get_or_create_order_book(symbol);
    ptr->add_order(order);
}
void MatchingEngine::cancel_order(const Symbol &symbol, OrderId order_id) {
    auto it = order_books_.find(symbol);
    if (it == order_books_.end()) {
        return;
    }
    OrderBook* order_book = it->second.get();
    order_book->cancel_order(order_id);
}
const OrderBook* MatchingEngine::get_order_book(const Symbol &symbol) {
    return const_cast<MatchingEngine*>(this)->find_order_book(symbol);
}
const TradeList& MatchingEngine::get_executed_trades() const {
    return const_cast<TradeList&>(executed_trades_);
}
TradeList MatchingEngine::get_trades_for_symbol(const Symbol& symbol) const {
    std::vector<Trade> trades_for_symbol;
    for (Trade t : executed_trades_) {
        if (t.get_symbol() == symbol) {
            trades_for_symbol.push_back(t);
        }
    }
    return trades_for_symbol;
}
bool MatchingEngine::has_order_book(const Symbol &symbol) const {
    auto it = order_books_.find(symbol);
    return it != order_books_.end();
}
OrderCount MatchingEngine::get_order_book_count() const {
    return order_books_.size();
}
TradeId MatchingEngine::get_next_trade_id() const {
    return next_trade_id_;
}
Timestamp MatchingEngine::get_current_timestamp() const {

}
Side MatchingEngine::determine_aggressor(Order* incoming_order) const {
    return incoming_order->get_side();
}
std::string MatchingEngine::to_string() const {

}

