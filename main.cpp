#include <bits/stdc++.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>  
#include <utility>
#include <vector>
using namespace std;

enum OrderType { BUY, SELL };

struct Order {
    int id;
    OrderType type;
    double price;
    int quantity;

    Order(int i, OrderType t, double p, int q) 
        : id(i), type(t), price(p), quantity(q) {}
};

struct CompareBuy {
    bool operator()(Order const& a, Order const& b) {
        return a.price < b.price; // Max-Heap
    }
};

struct CompareSell {
    bool operator()(Order const& a, Order const& b) {
        return a.price > b.price; // Min-Heap
    }
};

class OrderBook {
private:
    priority_queue<Order, vector<Order>, CompareBuy> buyOrders;
    priority_queue<Order, vector<Order>, CompareSell> sellOrders;
    std::mutex bookMutex; 
    atomic<int> totalMatches{0};
    atomic<long long> totalExecutedQty{0};

public:

    void addOrder(int id, OrderType type, double price, int quantity) {
        std::lock_guard<std::mutex> lock(bookMutex);
        Order newOrder(id, type, price, quantity);
        if (type == BUY) buyOrders.push(newOrder);
        else sellOrders.push(newOrder);     
        matchOrders(); 
    }
    void matchOrders() {
        while (!buyOrders.empty() && !sellOrders.empty()) {
            Order topBuy = buyOrders.top();
            Order topSell = sellOrders.top();
            if (topBuy.price < topSell.price) {
                break; 
            }
            int tradeQty = min(topBuy.quantity, topSell.quantity); 
            totalMatches.fetch_add(1, memory_order_relaxed);
            totalExecutedQty.fetch_add(tradeQty, memory_order_relaxed);
            buyOrders.pop();
            sellOrders.pop();
            if (topBuy.quantity > tradeQty) {
                topBuy.quantity -= tradeQty;
                buyOrders.push(topBuy);
            }
            if (topSell.quantity > tradeQty) {
                topSell.quantity -= tradeQty;
                sellOrders.push(topSell);
            }
        }
    }
    int getMatchCount() { return totalMatches.load(memory_order_relaxed); }
    long long getExecutedVolume() { return totalExecutedQty.load(memory_order_relaxed); }
    bool snapshotSpread(double& bestBid, double& bestAsk) {
        std::lock_guard<std::mutex> lock(bookMutex);
        if (buyOrders.empty() || sellOrders.empty()) return false;
        bestBid = buyOrders.top().price;
        bestAsk = sellOrders.top().price;
        return true;
    }
};
OrderBook book;

void tradingBot(int startId, int count, OrderType type) {
    // Thread-local RNG to avoid data races from rand()
    thread_local std::mt19937_64 rng(static_cast<uint64_t>(
        chrono::high_resolution_clock::now().time_since_epoch().count())
        ^ std::hash<std::thread::id>{}(this_thread::get_id()));
    uniform_real_distribution<double> priceDist(100.0, 110.0);

    for(int i = 0; i < count; i++) {
        double price = priceDist(rng);
        book.addOrder(startId + i, type, price, 10);
    }
}

int main() {
    srand(time(0));
    cout << "System Starting: Simulating High Frequency Trading" << endl;

    auto start = chrono::high_resolution_clock::now();
    std::thread t1(tradingBot, 1, 10000, BUY);
    std::thread t2(tradingBot, 10001, 10000, SELL);
    t1.join();
    t2.join();
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Simulation Complete" << endl;
    cout << "Total Matches Executed:" << book.getMatchCount() << endl;
    cout << "Total Executed Volume:" << book.getExecutedVolume() << endl;
    cout << "Time Taken:" << duration.count() << "microseconds" << endl;
    double seconds = duration.count() / 1000000.0;
    cout << "Throughput: " << (20000 / seconds) << " orders per second" << endl;
    double bestBid = 0.0, bestAsk = 0.0;
    if (book.snapshotSpread(bestBid, bestAsk)) {
        cout << "Final Spread (best bid / best ask): "
             << bestBid << " / " << bestAsk << endl;
    } else {
        cout << "Final Spread: book is empty or one-sided" << endl;
    }
    return 0;
}