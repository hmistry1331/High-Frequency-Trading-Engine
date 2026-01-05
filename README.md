# High-Frequency Trading Engine Simulation

A multi-threaded order book simulation written in C++ that mimics the core logic of a high-frequency trading (HFT) matching engine. This project demonstrates low-latency order matching, thread safety, and market simulation.

## 🚀 Key Features
* **High Performance:** Capable of processing over **700,000 orders per second**.
* **Concurrency:** Uses `std::thread` to simulate simultaneous buying and selling agents.
* **Thread Safety:** Implements `std::mutex` and `std::lock_guard` to prevent race conditions during order book updates.
* **Market Analysis:** Calculates real-time Bid-Ask spread and market snapshots.

## 📊 Performance Benchmarks
* **Total Executed Volume:** 67,100 units
* **Time Taken:** ~27,100 microseconds
* **Throughput:** ~738,007 orders/second (on local machine)

## 🛠️ Tech Stack
* **Language:** C++ (C++17 Standard)
* **Libraries:** `thread`, `mutex`, `vector`, `chrono`

