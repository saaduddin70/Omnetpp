\# 🏦 Banking Network System - OMNeT++ Simulation



A comprehensive \*\*banking network simulation\*\* built with \*\*OMNeT++ 6.2.0\*\*, featuring multi-branch architecture, database clustering, and realistic network protocols.



\## 📊 Project Overview



This project simulates a realistic banking network infrastructure with:

\- \*\*Central Headquarters\*\* with server infrastructure

\- \*\*3 Branch Offices\*\* (Branch A, B, C)

\- \*\*24 Client PCs\*\* (8 per branch)

\- \*\*Database Cluster\*\* with Primary-Secondary-Backup replication

\- \*\*Full Protocol Stack\*\* (DHCP, DNS, HTTP, SMTP, Custom DB protocol)



\## 🏗️ Network Architecture



&nbsp;               Core Router (1)

&nbsp;                     │

&nbsp;   ┌─────────────────┼──────────────────┐

&nbsp;   │                 │                  │

DHCP (10) DNS (11) Mail (12)

│ │ │

Web (13) ┌─────────┴─────────┐ │

│ │ │

DB Primary (20) DB Secondary (21)

│ │

DB Backup (22) │

│

┌───────────────────────────┴────────────────┐

│ │ │

Branch A Router Branch B Router Branch C Router

(100) (200) (300)

│ │ │

Switch (101) Switch (210) Switch (301)

│ │ │

8 PCs 8 PCs 8 PCs

(111-118) (211-218) (311-318)

VLAN 10 VLAN 20 VLAN 30



\## ✨ Key Features



\### 🌐 Network Protocols

\- ✅ \*\*DHCP\*\* - Dynamic IP allocation with VLAN support

\- ✅ \*\*DNS\*\* - Domain name resolution service

\- ✅ \*\*HTTP\*\* - Web banking portal

\- ✅ \*\*SMTP/POP3\*\* - Email communication

\- ✅ \*\*Custom Database Protocol\*\* - Banking transactions with READ/WRITE operations



\### 🔌 Network Infrastructure

\- ✅ \*\*VLAN Segmentation\*\* (10, 20, 30)

\- ✅ \*\*MAC Address Learning\*\* (Layer 2 switches)

\- ✅ \*\*Static Routing\*\* with address-based forwarding

\- ✅ \*\*Database Load Balancing\*\* (80% READ, 20% WRITE distribution)

\- ✅ \*\*Primary-Secondary Replication\*\* for data consistency



\### 💼 Application Layer

\- ✅ \*\*Account Balance Queries\*\*

\- ✅ \*\*Deposit/Withdrawal Transactions\*\*

\- ✅ \*\*Multi-step Workflow\*\* (DHCP → DNS → Database → HTTP → Email)

\- ✅ \*\*Realistic Network Delays\*\* and timing



\### 📈 Database System

\- \*\*Primary DB\*\* (Address 20) - Handles all WRITE + 30% READ queries

\- \*\*Secondary DB\*\* (Address 21) - Handles 70% READ queries with replication

\- \*\*Backup DB\*\* (Address 22) - Standby mode for failover

\- \*\*24 Bank Accounts\*\* (1001-1024) with realistic balances (50K-100K BDT)



\## 🛠️ Requirements



\- \*\*OMNeT++ 6.2.0\*\* or higher

\- \*\*GCC/Clang\*\* C++ compiler

\- \*\*Windows/Linux/macOS\*\* compatible



\## 📦 Installation \& Build





