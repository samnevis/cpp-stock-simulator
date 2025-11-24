#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>

struct Stock {
    std::string name;
    double price;
    std::vector<double> history; // Last 5 days
    int sharesOwned;
};

struct NewsEvent {
    int stockIndex;
    double dailyPercentImpact; // Percentage change per day
    int daysRemaining;
    std::string description;
};

struct Transaction {
    int day;
    std::string type; // "BUY" or "SELL"
    std::string stockName;
    int shares;
    double price;
    double totalValue;
};

struct PortfolioSnapshot {
    int day;
    double cash;
    double stockValue;
    double totalValue;
};

class TradingSimulator {
private:
    Stock stocks[2];
    double balance;
    double initialBalance;
    int currentDay;
    std::vector<NewsEvent> activeNews;
    std::vector<Transaction> transactionHistory;
    std::vector<PortfolioSnapshot> portfolioHistory;
    std::vector<std::pair<int, double>> fullPriceHistory[2]; // {day, price}

public:
    TradingSimulator() {
        srand(time(0));
        stocks[0] = {"TechCorp", 10.0, {}, 0};
        stocks[1] = {"FinanceInc", 10.0, {}, 0};
        balance = 1000.0;
        initialBalance = 1000.0;
        currentDay = 0;
        
        // Initialize history with starting prices
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 5; j++) {
                stocks[i].history.push_back(stocks[i].price);
            }
            // Record initial price in full history
            fullPriceHistory[i].push_back({0, stocks[i].price});
        }
        
        // Record initial portfolio state
        recordPortfolioSnapshot();
    }
    
    void recordPortfolioSnapshot() {
        PortfolioSnapshot snapshot;
        snapshot.day = currentDay;
        snapshot.cash = balance;
        snapshot.stockValue = 0.0;
        for (int i = 0; i < 2; i++) {
            snapshot.stockValue += stocks[i].sharesOwned * stocks[i].price;
        }
        snapshot.totalValue = snapshot.cash + snapshot.stockValue;
        portfolioHistory.push_back(snapshot);
    }

    void updatePrices() {
        // Apply active news impacts (percentage based)
        for (auto& news : activeNews) {
            double change = stocks[news.stockIndex].price * news.dailyPercentImpact / 100.0;
            stocks[news.stockIndex].price += change;
            if (stocks[news.stockIndex].price < 1.0) stocks[news.stockIndex].price = 1.0;
            news.daysRemaining--;
        }
        
        // Remove expired news
        activeNews.erase(
            std::remove_if(activeNews.begin(), activeNews.end(),
                [](const NewsEvent& n) { return n.daysRemaining <= 0; }),
            activeNews.end()
        );

        // Random price change (normal day) - percentage based
        for (int i = 0; i < 2; i++) {
            double percentChange = (rand() % 11 - 5) / 1.0; // -5% to +5%
            double change = stocks[i].price * percentChange / 100.0;
            stocks[i].price += change;
            if (stocks[i].price < 1.0) stocks[i].price = 1.0;
        }


        if (rand() % 100 < 40) {
            int stockIndex = rand() % 2;
            bool isGoodNews = rand() % 2 == 0;
            std::string newsMsg;
            double totalPercentImpact;
            
            if (stockIndex == 0) { // TechCorp
                if (isGoodNews) {
                    newsMsg = "Tech breakthrough discovered!";
                    totalPercentImpact = 20.0; // +20% over 5 days
                } else {
                    newsMsg = "Data breach discovered!";
                    totalPercentImpact = -20.0; // -20% over 5 days
                }
            } else { // FinanceInc
                if (isGoodNews) {
                    newsMsg = "Record profits announced!";
                    totalPercentImpact = 20.0; // +20% over 5 days
                } else {
                    newsMsg = "Market crash warning!";
                    totalPercentImpact = -20.0; // -20% over 5 days
                }
            }
            
            double dailyPercentImpact = totalPercentImpact / 5.0; // 4% per day
            double estimatedTotalChange = stocks[stockIndex].price * totalPercentImpact / 100.0;
            
            NewsEvent news;
            news.stockIndex = stockIndex;
            news.dailyPercentImpact = dailyPercentImpact;
            news.daysRemaining = 5;
            news.description = newsMsg;
            activeNews.push_back(news);
            
            std::cout << "\n*** BREAKING NEWS: " << newsMsg << " ***\n";
            std::cout << stocks[stockIndex].name << " will ";
            if (isGoodNews) {
                std::cout << "rise by " << std::fixed << std::setprecision(1) 
                          << totalPercentImpact << "% (approx $" 
                          << std::setprecision(2) << estimatedTotalChange 
                          << ") over the next 5 days";
            } else {
                std::cout << "fall by " << std::fixed << std::setprecision(1) 
                          << -totalPercentImpact << "% (approx $" 
                          << std::setprecision(2) << -estimatedTotalChange 
                          << ") over the next 5 days";
            }
            std::cout << " (~" << std::fixed << std::setprecision(1) 
                      << fabs(dailyPercentImpact) << "% per day)\n\n";
        }

        // Update history (remove oldest, add current)
        for (int i = 0; i < 2; i++) {
            stocks[i].history.erase(stocks[i].history.begin());
            stocks[i].history.push_back(stocks[i].price);
        }
        currentDay++;
        // Record prices at the new day
        for (int i = 0; i < 2; i++) {
            fullPriceHistory[i].push_back({currentDay, stocks[i].price});
        }
        recordPortfolioSnapshot();
    }

    void displayPrices() {
        std::cout << "\n=== STOCK PRICES (Last 5 Days) ===\n";
        for (int i = 0; i < 2; i++) {
            std::cout << "\n" << stocks[i].name << ":\n";
            std::cout << "  Current: $" << std::fixed << std::setprecision(2) 
                      << stocks[i].price << "\n";
            std::cout << "  History: ";
            for (double price : stocks[i].history) {
                std::cout << "$" << std::fixed << std::setprecision(2) << price << " ";
            }
            std::cout << "\n";
        }
        
        // Display active news
        if (!activeNews.empty()) {
            std::cout << "\n=== ACTIVE NEWS ===\n";
            for (const auto& news : activeNews) {
                std::cout << stocks[news.stockIndex].name << ": " << news.description 
                          << " (" << news.daysRemaining << " days remaining, ";
                if (news.dailyPercentImpact > 0) {
                    std::cout << "+" << std::fixed << std::setprecision(1) 
                              << news.dailyPercentImpact << "%/day)\n";
                } else {
                    std::cout << std::fixed << std::setprecision(1) 
                              << news.dailyPercentImpact << "%/day)\n";
                }
            }
        }
        std::cout << "\n";
    }

    void displayPortfolio() {
        std::cout << "\n=== YOUR PORTFOLIO ===\n";
        std::cout << "Cash: $" << std::fixed << std::setprecision(2) << balance << "\n";
        double totalValue = balance;
        for (int i = 0; i < 2; i++) {
            double value = stocks[i].sharesOwned * stocks[i].price;
            totalValue += value;
            std::cout << stocks[i].name << ": " << stocks[i].sharesOwned 
                      << " shares @ $" << stocks[i].price 
                      << " = $" << value << "\n";
        }
        std::cout << "Total Value: $" << totalValue << "\n\n";
    }

    void buyStock(int stockIndex) {
        int shares;
        std::cout << "How many shares of " << stocks[stockIndex].name 
                  << " ($" << stocks[stockIndex].price << ") to buy? ";
        std::cin >> shares;
        
        double cost = shares * stocks[stockIndex].price;
        if (cost > balance) {
            std::cout << "Not enough cash!\n";
        } else {
            balance -= cost;
            stocks[stockIndex].sharesOwned += shares;
            std::cout << "Bought " << shares << " shares for $" << cost << "\n";
            
            // Record transaction
            Transaction t;
            t.day = currentDay;
            t.type = "BUY";
            t.stockName = stocks[stockIndex].name;
            t.shares = shares;
            t.price = stocks[stockIndex].price;
            t.totalValue = cost;
            transactionHistory.push_back(t);
        }
    }

    void sellStock(int stockIndex) {
        int shares;
        std::cout << "How many shares of " << stocks[stockIndex].name 
                  << " ($" << stocks[stockIndex].price << ") to sell? ";
        std::cin >> shares;
        
        if (shares > stocks[stockIndex].sharesOwned) {
            std::cout << "You don't have that many shares!\n";
        } else {
            double revenue = shares * stocks[stockIndex].price;
            balance += revenue;
            stocks[stockIndex].sharesOwned -= shares;
            std::cout << "Sold " << shares << " shares for $" << revenue << "\n";
            
            // Record transaction
            Transaction t;
            t.day = currentDay;
            t.type = "SELL";
            t.stockName = stocks[stockIndex].name;
            t.shares = shares;
            t.price = stocks[stockIndex].price;
            t.totalValue = revenue;
            transactionHistory.push_back(t);
        }
    }
    
    void displayAnalytics() {
        std::cout << "\n=== TRADING ANALYTICS ===\n\n";
        
        // Portfolio performance
        double currentTotal = balance;
        for (int i = 0; i < 2; i++) {
            currentTotal += stocks[i].sharesOwned * stocks[i].price;
        }
        double totalReturn = currentTotal - initialBalance;
        double roi = (totalReturn / initialBalance) * 100.0;
        
        std::cout << "Portfolio Performance:\n";
        std::cout << "  Initial Balance: $" << std::fixed << std::setprecision(2) 
                  << initialBalance << "\n";
        std::cout << "  Current Value: $" << currentTotal << "\n";
        std::cout << "  Total Return: $" << totalReturn;
        if (totalReturn >= 0) {
            std::cout << " (+" << std::setprecision(2) << roi << "%)\n";
        } else {
            std::cout << " (" << std::setprecision(2) << roi << "%)\n";
        }
        std::cout << "\n";
        
        // Transaction statistics
        if (!transactionHistory.empty()) {
            int buyCount = 0, sellCount = 0;
            double totalBuyValue = 0, totalSellValue = 0;
            double bestProfit = -999999, worstProfit = 999999;
            std::string bestTrade = "", worstTrade = "";
            
            // Track buy prices for each stock
            std::vector<std::pair<double, int>> buyPrices[2]; // {price, shares}
            
            for (const auto& t : transactionHistory) {
                int stockIdx = (t.stockName == "TechCorp") ? 0 : 1;
                
                if (t.type == "BUY") {
                    buyCount++;
                    totalBuyValue += t.totalValue;
                    buyPrices[stockIdx].push_back({t.price, t.shares});
                } else { // SELL
                    sellCount++;
                    totalSellValue += t.totalValue;
                    
                    // Calculate profit/loss for this sale
                    double avgBuyPrice = 0;
                    int remainingShares = t.shares;
                    
                    // FIFO: match with oldest buys first
                    for (auto& buy : buyPrices[stockIdx]) {
                        if (remainingShares <= 0) break;
                        int sharesUsed = (remainingShares < buy.second) ? remainingShares : buy.second;
                        avgBuyPrice += buy.first * sharesUsed;
                        buy.second -= sharesUsed;
                        remainingShares -= sharesUsed;
                    }
                    
                    if (remainingShares < t.shares) {
                        avgBuyPrice /= (t.shares - remainingShares);
                        double profit = (t.price - avgBuyPrice) * (t.shares - remainingShares);
                        
                        if (profit > bestProfit) {
                            bestProfit = profit;
                            bestTrade = t.stockName + " (Day " + std::to_string(t.day) + ")";
                        }
                        if (profit < worstProfit) {
                            worstProfit = profit;
                            worstTrade = t.stockName + " (Day " + std::to_string(t.day) + ")";
                        }
                    }
                }
            }
            
            std::cout << "Transaction Statistics:\n";
            std::cout << "  Total Transactions: " << transactionHistory.size() << "\n";
            std::cout << "  Buys: " << buyCount << " | Sells: " << sellCount << "\n";
            std::cout << "  Total Spent: $" << std::fixed << std::setprecision(2) 
                      << totalBuyValue << "\n";
            std::cout << "  Total Received: $" << totalSellValue << "\n";
            
            if (bestProfit > -999999) {
                std::cout << "  Best Trade: " << bestTrade << " (+$" 
                          << std::fixed << std::setprecision(2) << bestProfit << ")\n";
            }
            if (worstProfit < 999999) {
                std::cout << "  Worst Trade: " << worstTrade << " ($" 
                          << std::fixed << std::setprecision(2) << worstProfit << ")\n";
            }
        } else {
            std::cout << "No transactions yet.\n";
        }
        
        std::cout << "\n";
    }
    
    void exportToCSV() {
        // Export transactions
        std::ofstream transFile("transactions.csv");
        if (transFile.is_open()) {
            transFile << "Day,Type,Stock,Shares,Price,Total Value\n";
            for (const auto& t : transactionHistory) {
                transFile << t.day << "," << t.type << "," << t.stockName 
                         << "," << t.shares << "," << std::fixed << std::setprecision(2)
                         << t.price << "," << t.totalValue << "\n";
            }
            transFile.close();
            std::cout << "Transactions exported to transactions.csv\n";
        }
        
        // Export portfolio history
        std::ofstream portfolioFile("portfolio_history.csv");
        if (portfolioFile.is_open()) {
            portfolioFile << "Day,Cash,Stock Value,Total Value\n";
            for (const auto& p : portfolioHistory) {
                portfolioFile << p.day << "," << std::fixed << std::setprecision(2)
                             << p.cash << "," << p.stockValue << "," << p.totalValue << "\n";
            }
            portfolioFile.close();
            std::cout << "Portfolio history exported to portfolio_history.csv\n";
        }
        
        // Export stock prices
        std::ofstream pricesFile("stock_prices.csv");
        if (pricesFile.is_open()) {
            pricesFile << "Day,TechCorp,FinanceInc\n";
            // Find max day to align both stocks
            int maxDay = 0;
            for (int i = 0; i < 2; i++) {
                if (!fullPriceHistory[i].empty()) {
                    maxDay = std::max(maxDay, fullPriceHistory[i].back().first);
                }
            }
            
            // Create a map for quick lookup
            std::map<int, double> prices[2];
            for (int i = 0; i < 2; i++) {
                for (const auto& entry : fullPriceHistory[i]) {
                    prices[i][entry.first] = entry.second;
                }
            }
            
            // Write all days
            for (int day = 0; day <= maxDay; day++) {
                pricesFile << day << "," << std::fixed << std::setprecision(2);
                if (prices[0].count(day)) {
                    pricesFile << prices[0][day];
                } else {
                    pricesFile << "N/A";
                }
                pricesFile << ",";
                if (prices[1].count(day)) {
                    pricesFile << prices[1][day];
                } else {
                    pricesFile << "N/A";
                }
                pricesFile << "\n";
            }
            pricesFile.close();
            std::cout << "Stock prices exported to stock_prices.csv\n";
        }
        
        std::cout << "\nAll data exported successfully!\n\n";
    }

    void run() {
        std::cout << "=== STOCK TRADING SIMULATOR ===\n";
        
        while (true) {
            displayPrices();
            displayPortfolio();
            
            std::cout << "Options:\n";
            std::cout << "1. Buy TechCorp\n";
            std::cout << "2. Sell TechCorp\n";
            std::cout << "3. Buy FinanceInc\n";
            std::cout << "4. Sell FinanceInc\n";
            std::cout << "5. Advance to next day\n";
            std::cout << "6. View Analytics\n";
            std::cout << "7. Export to CSV\n";
            std::cout << "8. Quit\n";
            std::cout << "Choice: ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1) buyStock(0);
            else if (choice == 2) sellStock(0);
            else if (choice == 3) buyStock(1);
            else if (choice == 4) sellStock(1);
            else if (choice == 5) {
                updatePrices();
                std::cout << "\n--- Day " << currentDay << " ---\n";
            }
            else if (choice == 6) displayAnalytics();
            else if (choice == 7) exportToCSV();
            else if (choice == 8) break;
            else std::cout << "Invalid choice!\n";
        }
    }
};

int main() {
    TradingSimulator sim;
    sim.run();
    return 0;
}

