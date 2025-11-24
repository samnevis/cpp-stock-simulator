# Stock Trading Simulator

## Features

• Includes real-time stock price simulation with daily fluctuations (-5% to +5%) and multi-day news event impacts
• Has comprehensive portfolio tracking system with cash management, share ownership, and total value calculations
• Contains analytics dashboard featuring ROI calculations, transaction statistics, and best/worst trade identification
• Uses FIFO (First-In-First-Out) accounting method for accurate profit/loss calculations on stock sales
• Uses CSV export functionality for transactions, portfolio history, and stock price data for external analysis

## Example game behavior

```
--- Day 4 ---

=== STOCK PRICES (Last 5 Days) ===

TechCorp:
  Current: $9.29
  History: $10.00 $10.50 $10.08 $9.78 $9.29

FinanceInc:
  Current: $9.79
  History: $10.00 $9.50 $9.60 $9.69 $9.79

=== ACTIVE NEWS ===
FinanceInc: Market crash warning! (5 days remaining, -4.0%/day)


=== YOUR PORTFOLIO ===
Cash: $500.00
TechCorp: 50 shares @ $9.29 = $464.44
FinanceInc: 0 shares @ $9.79 = $0.00
Total Value: $964.44

Options:
1. Buy TechCorp
2. Sell TechCorp
3. Buy FinanceInc
4. Sell FinanceInc
5. Advance to next day
6. View Analytics
7. Export to CSV
8. Quit
Choice:
```