//#include <omnetpp.h>
//#include "helpers.h"
//#include <map>
//#include <vector>
//
//using namespace omnetpp;
//
//class DatabaseApp : public cSimpleModule {
//private:
//    int address;
//    long myIP;
//    std::string role;
//    bool isActive;
//
//    // Database storage
//    std::map<long, long> accountBalances;  // accountId → balance
//    std::map<long, std::string> accountNames;
//
//    // Statistics
//    int readQueries = 0;
//    int writeQueries = 0;
//    int syncOperations = 0;
//    double totalResponseTime = 0;
//    int totalQueries = 0;
//
//    // Load tracking
//    cMessage *loadCheckTimer;
//    int queriesLastSecond = 0;
//    double currentLoad = 0.0;
//    double peakLoad = 0.0;
//
//protected:
//    void initialize() override {
//        address = par("address");
//        role = par("role").stdstringValue();
//        isActive = (role == "primary" || role == "secondary");
//
//        // Set IP based on role
//        if (role == "primary") myIP = makeIP(192, 168, 0, 20);
//        else if (role == "secondary") myIP = makeIP(192, 168, 0, 21);
//        else if (role == "backup") myIP = makeIP(192, 168, 0, 22);
//
//        // Initialize 24 sample accounts (one per PC)
//        for (int i = 1001; i <= 1024; i++) {
//            accountBalances[i] = 50000 + intuniform(0, 50000);
//
//            char name[32];
//            sprintf(name, "Customer_%d", i - 1000);
//            accountNames[i] = std::string(name);
//        }
//
//        // Start load monitoring
//        loadCheckTimer = new cMessage("loadCheck");
//        scheduleAt(simTime() + 1.0, loadCheckTimer);
//
//        EV << "========================================\n";
//        EV << "💾 DATABASE SERVER INITIALIZED\n";
//        EV << "========================================\n";
//        EV << "Role: " << role << " (" << (isActive ? "ACTIVE" : "STANDBY") << ")\n";
//        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
//        EV << "Initial accounts: " << accountBalances.size() << " accounts\n";
//
//        long totalBalance = 0;
//        for (auto &entry : accountBalances) totalBalance += entry.second;
//        EV << "Total balance in bank: " << totalBalance << " BDT\n";
//        EV << "Average balance: " << (totalBalance / accountBalances.size()) << " BDT\n";
//        EV << "========================================\n\n";
//    }
//
//    void handleMessage(cMessage *msg) override {
//        if (msg == loadCheckTimer) {
//            // Calculate current load
//            currentLoad = (queriesLastSecond / 100.0) * 100;
//            if (currentLoad > peakLoad) peakLoad = currentLoad;
//
//            if (simTime().dbl() >= 10.0 && queriesLastSecond > 0) {
//                EV << "⚙️  [" << role << " DB @ t=" << simTime() << "s] "
//                   << "Load: " << currentLoad << "% "
//                   << "(" << queriesLastSecond << " queries/sec)\n";
//
//                if (currentLoad > 80 && role == "primary") {
//                    EV << "⚠️  WARNING: Primary DB load > 80%! Consider scaling.\n";
//                }
//            }
//
//            queriesLastSecond = 0;
//            scheduleAt(simTime() + 1.0, loadCheckTimer);
//            return;
//        }
//
//        if (!isActive) {
//            EV << "❌ [" << role << " DB] Inactive (standby mode), dropping query\n";
//            delete msg;
//            return;
//        }
//
//        int kind = msg->getKind();
//        long clientAddr = SRC(msg);
//        long clientIP = SRCIP(msg);
//        simtime_t startTime = simTime();
//
//        if (kind == DB_QUERY_READ) {
//            readQueries++;
//            queriesLastSecond++;
//            totalQueries++;
//
//            long accountId = ACCOUNT_ID(msg);
//
//            EV << "\n📖 [" << role << " DB] READ query from client " << clientAddr
//               << " (IP: " << ipToStr(clientIP) << ")\n";
//            EV << "   Query: SELECT * FROM accounts WHERE id=" << accountId << "\n";
//
//            // Lookup account
//            long balance = accountBalances.count(accountId) ? accountBalances[accountId] : 0;
//            std::string name = accountNames.count(accountId) ? accountNames[accountId] : "Unknown";
//
//            if (balance > 0) {
//                EV << "   ✓ Found: Account " << accountId << " (" << name << ")\n";
//                EV << "   ✓ Balance: " << balance << " BDT\n";
//            } else {
//                EV << "   ✗ Account " << accountId << " not found in database\n";
//            }
//
//            // Send response
//            auto *response = mk("DB_RESPONSE", DB_RESPONSE_SUCCESS, address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("accountId") = accountId;
//            response->par("amount") = balance;
//            response->par("payload") = name.c_str();
//
//            double responseTime = 0.010;  // 10ms for reads
//            sendDelayed(response, responseTime, "ppp$o");
//            totalResponseTime += responseTime;
//
//            EV << "   → Response sent in " << (responseTime * 1000) << "ms\n";
//        }
//        else if (kind == DB_QUERY_WRITE) {
//            if (role != "primary") {
//                EV << "\n❌ [" << role << " DB] WRITE rejected (read-only replica)\n";
//                auto *error = mk("DB_ERROR", DB_RESPONSE_ERROR, address, clientAddr);
//                error->par("srcIP") = myIP;
//                error->par("dstIP") = clientIP;
//                error->par("payload") = "Only primary accepts writes";
//                sendDelayed(error, 0.005, "ppp$o");
//                delete msg;
//                return;
//            }
//
//            writeQueries++;
//            queriesLastSecond++;
//            totalQueries++;
//
//            long accountId = ACCOUNT_ID(msg);
//            long amount = AMOUNT(msg);
//            std::string queryType = QUERY_TYPE(msg);
//
//            EV << "\n✏️  [PRIMARY DB] WRITE query from client " << clientAddr << "\n";
//            EV << "   Operation: " << queryType << " on account " << accountId << "\n";
//            EV << "   Amount: " << amount << " BDT\n";
//
//            long oldBalance = accountBalances[accountId];
//            long newBalance = oldBalance;
//            bool success = true;
//
//            if (queryType == "WITHDRAW") {
//                if (oldBalance >= amount) {
//                    newBalance = oldBalance - amount;
//                    accountBalances[accountId] = newBalance;
//                    EV << "   ✓ Withdrawal successful\n";
//                } else {
//                    success = false;
//                    EV << "   ✗ Insufficient funds! (Balance: " << oldBalance
//                       << ", Requested: " << amount << ")\n";
//                }
//            } else if (queryType == "DEPOSIT") {
//                newBalance = oldBalance + amount;
//                accountBalances[accountId] = newBalance;
//                EV << "   ✓ Deposit successful\n";
//            } else if (queryType == "UPDATE") {
//                newBalance = amount;
//                accountBalances[accountId] = newBalance;
//                EV << "   ✓ Balance updated\n";
//            }
//
//            if (success) {
//                EV << "   📊 Account " << accountId << ": " << oldBalance
//                   << " → " << newBalance << " BDT (change: "
//                   << (newBalance - oldBalance) << ")\n";
//
//                // Simulate replication to secondary
//                EV << "   🔄 Replicating to secondary DB...\n";
//                syncOperations++;
//            }
//
//            // Send response
//            auto *response = mk("DB_RESPONSE",
//                              success ? DB_RESPONSE_SUCCESS : DB_RESPONSE_ERROR,
//                              address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("accountId") = accountId;
//            response->par("amount") = newBalance;
//
//            double responseTime = 0.025;  // 25ms for writes
//            sendDelayed(response, responseTime, "ppp$o");
//            totalResponseTime += responseTime;
//
//            EV << "   → Write completed in " << (responseTime * 1000) << "ms\n\n";
//        }
//        else if (kind == PING) {
//            // Health check
//            auto *reply = mk("PONG", PONG, address, clientAddr);
//            reply->par("srcIP") = myIP;
//            reply->par("dstIP") = clientIP;
//            sendDelayed(reply, 0.005, "ppp$o");
//        }
//
//        delete msg;
//    }
//
//    void finish() override {
//        double avgResponseTime = totalQueries > 0 ? (totalResponseTime / totalQueries) * 1000 : 0;
//
//        EV << "\n";
//        EV << "========================================\n";
//        EV << "💾 DATABASE SERVER [" << role << "] FINAL STATISTICS\n";
//        EV << "========================================\n";
//        EV << "Total queries processed: " << totalQueries << "\n";
//        EV << "  - READ queries: " << readQueries << " ("
//           << (totalQueries > 0 ? (readQueries * 100.0 / totalQueries) : 0) << "%)\n";
//        EV << "  - WRITE queries: " << writeQueries << " ("
//           << (totalQueries > 0 ? (writeQueries * 100.0 / totalQueries) : 0) << "%)\n";
//        EV << "Replication operations: " << syncOperations << "\n";
//        EV << "Average response time: " << avgResponseTime << " ms\n";
//        EV << "Peak load: " << peakLoad << "%\n";
//        EV << "Accounts in database: " << accountBalances.size() << "\n";
//
//        long totalBalance = 0;
//        for (auto &entry : accountBalances) totalBalance += entry.second;
//        EV << "Total balance: " << totalBalance << " BDT\n";
//        EV << "Average balance: " << (totalBalance / accountBalances.size()) << " BDT\n";
//        EV << "========================================\n\n";
//
//        recordScalar("totalQueries", totalQueries);
//        recordScalar("readQueries", readQueries);
//        recordScalar("writeQueries", writeQueries);
//        recordScalar("avgResponseTime_ms", avgResponseTime);
//        recordScalar("peakLoad_percent", peakLoad);
//        recordScalar("totalBalance_BDT", totalBalance);
//
//        cancelAndDelete(loadCheckTimer);
//    }
//};
//
//Define_Module(DatabaseApp);


//Claude
#include <omnetpp.h>
#include "helpers.h"
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace omnetpp;

class DatabaseApp : public cSimpleModule {
private:
    int address;
    long myIP;
    std::string role;
    bool isActive;

    // Database storage
    std::map<long, long> accountBalances;  // accountId → balance
    std::map<long, std::string> accountNames;

    // Statistics
    int readQueries = 0;
    int writeQueries = 0;
    int syncOperations = 0;
    double totalResponseTime = 0;
    int totalQueries = 0;

    // Load tracking
    cMessage *loadCheckTimer;
    int queriesLastSecond = 0;
    double currentLoad = 0.0;
    double peakLoad = 0.0;
    // Encryption
    int encryptedQueriesReceived = 0;
    int encryptedResponsesSent = 0;

protected:
    // Encryption
    int parseAccountId(const std::string& decryptedData) {
        size_t pos = decryptedData.find("ACCOUNT:");
        if (pos != std::string::npos) {
            size_t start = pos + 8;  // After "ACCOUNT:"
            size_t end = decryptedData.find("|", start);
            if (end != std::string::npos) {
                std::string accStr = decryptedData.substr(start, end - start);
                try {
                    return std::stoi(accStr);
                } catch (...) {
                    EV << "❌ Parse error: Invalid account ID\n";
                    return 0;
                }
            }
        }
        return 0;
    }

    void initialize() override {
        address = par("address");
        role = par("role").stdstringValue();
        isActive = (role == "primary" || role == "secondary");

        // Set IP based on role
        if (role == "primary") myIP = makeIP(192, 168, 0, 20);
        else if (role == "secondary") myIP = makeIP(192, 168, 0, 21);
        else if (role == "backup") myIP = makeIP(192, 168, 0, 22);

        // Initialize 24 sample accounts (one per PC)
        for (int i = 1001; i <= 1024; i++) {
            accountBalances[i] = 50000 + intuniform(0, 50000);

            char name[32];
            sprintf(name, "Customer_%d", i - 1000);
            accountNames[i] = std::string(name);
        }

        // Start load monitoring
        loadCheckTimer = new cMessage("loadCheck");
        scheduleAt(simTime() + 1.0, loadCheckTimer);

        EV << "========================================\n";
        EV << "💾 DATABASE SERVER INITIALIZED\n";
        EV << "========================================\n";
        EV << "Role: " << role << " (" << (isActive ? "ACTIVE" : "STANDBY") << ")\n";
        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
        EV << "Initial accounts: " << accountBalances.size() << " accounts\n";

        long totalBalance = 0;
        for (auto &entry : accountBalances) totalBalance += entry.second;
        EV << "Total balance in bank: " << totalBalance << " BDT\n";
        EV << "Average balance: " << (totalBalance / accountBalances.size()) << " BDT\n";
        EV << "========================================\n\n";
    }

    void handleMessage(cMessage *msg) override {
        if (msg == loadCheckTimer) {
            // Calculate current load
            currentLoad = (queriesLastSecond / 100.0) * 100;
            if (currentLoad > peakLoad) peakLoad = currentLoad;

            if (simTime().dbl() >= 10.0 && queriesLastSecond > 0) {
                EV << "⚙️  [" << role << " DB @ t=" << simTime() << "s] "
                   << "Load: " << currentLoad << "% "
                   << "(" << queriesLastSecond << " queries/sec)\n";

                if (currentLoad > 80 && role == "primary") {
                    EV << "⚠️  WARNING: Primary DB load > 80%! Consider scaling.\n";
                }
            }

            queriesLastSecond = 0;
            scheduleAt(simTime() + 1.0, loadCheckTimer);
            return;
        }

        if (!isActive) {
            EV << "❌ [" << role << " DB] Inactive (standby mode), dropping query\n";
            delete msg;
            return;
        }

        int kind = msg->getKind();
        long clientAddr = SRC(msg);
        long clientIP = SRCIP(msg);
        simtime_t startTime = simTime();

//        if (kind == DB_QUERY_READ) {
//            readQueries++;
//            queriesLastSecond++;
//            totalQueries++;
//
//            long accountId = ACCOUNT_ID(msg);
//
//            EV << "\n📖 [" << role << " DB] READ query from client " << clientAddr
//               << " (IP: " << ipToStr(clientIP) << ")\n";
//            EV << "   Query: SELECT * FROM accounts WHERE id=" << accountId << "\n";
//
//            // Lookup account
//            long balance = accountBalances.count(accountId) ? accountBalances[accountId] : 0;
//            std::string name = accountNames.count(accountId) ? accountNames[accountId] : "Unknown";
//
//            if (balance > 0) {
//                EV << "   ✓ Found: Account " << accountId << " (" << name << ")\n";
//                EV << "   ✓ Balance: " << balance << " BDT\n";
//            } else {
//                EV << "   ✗ Account " << accountId << " not found in database\n";
//            }
//
//            // Send response via TCP (not PPP directly!)
//            auto *response = mk("DB_RESPONSE", DB_RESPONSE_SUCCESS, address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("accountId") = accountId;
//            response->par("amount") = balance;
//            response->par("payload") = name.c_str();
//
//            double responseTime = 0.010;  // 10ms for reads
//            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
//            sendDelayed(response, responseTime, "tcpOut");
//            totalResponseTime += responseTime;
//
//            EV << "   → Response sent in " << (responseTime * 1000) << "ms\n";
//        }
//        if (kind == DB_QUERY_READ) {
//            readQueries++;
//            queriesLastSecond++;
//            totalQueries++;
//
//            // ============ DECRYPTION CHECK ============
//            bool isEncrypted = (msg->hasPar("encrypted") && msg->par("encrypted").boolValue());
//
//            long accountId = ACCOUNT_ID(msg);
//
//            if (isEncrypted) {
//                // Decrypt query
//                std::string encryptedPayload = msg->par("payload").stringValue();
//                std::string decryptedPayload = xorDecrypt(encryptedPayload);
//
//                encryptedQueriesReceived++;
//
//                EV << "\n========================================\n";
//                EV << "🔓 XOR DECRYPTION - " << role << " DB\n";
//                EV << "========================================\n";
//                EV << "Time: " << simTime() << "s\n";
//                EV << "Client: PC " << clientAddr << "\n";
//                EV << "Encrypted (Hex): " << toHex(encryptedPayload) << "\n";
//                EV << "Decrypted: " << decryptedPayload << "\n";
//                EV << "Total Encrypted Queries: " << encryptedQueriesReceived << "\n";
//                EV << "========================================\n\n";
//            }
//
//            EV << "\n📖 [" << role << " DB] READ query from client " << clientAddr
//               << " (IP: " << ipToStr(clientIP) << ")\n";
//            EV << "   Query: SELECT * FROM accounts WHERE id=" << accountId << "\n";
//
//            // Lookup account
//            long balance = accountBalances.count(accountId) ? accountBalances[accountId] : 0;
//            std::string name = accountNames.count(accountId) ? accountNames[accountId] : "Unknown";
//
//            if (balance > 0) {
//                EV << "   ✓ Found: Account " << accountId << " (" << name << ")\n";
//                EV << "   ✓ Balance: " << balance << " BDT\n";
//            } else {
//                EV << "   ✗ Account " << accountId << " not found in database\n";
//            }
//
//            // ============ RESPONSE ENCRYPTION ============
//            if (isEncrypted) {
//                // Send encrypted response
//                sendEncryptedResponse(clientAddr, clientIP, accountId, balance, name);
//            } else {
//                // Unencrypted response (backward compatibility)
//                auto *response = mk("DB_RESPONSE", DB_RESPONSE_SUCCESS, address, clientAddr);
//                response->par("srcIP") = myIP;
//                response->par("dstIP") = clientIP;
//                response->par("accountId") = accountId;
//                response->par("amount") = balance;
//                response->par("payload") = name.c_str();
//
//                double responseTime = 0.010;
//                sendDelayed(response, responseTime, "tcpOut");
//                totalResponseTime += responseTime;
//                EV << "   → Response sent in " << (responseTime * 1000) << "ms\n";
//            }
//        }
//        if (kind == DB_QUERY_READ) {
//            readQueries++;
//            queriesLastSecond++;
//            totalQueries++;
//
//            // ============ DECRYPTION CHECK ============
//            bool isEncrypted = (msg->hasPar("encrypted") && msg->par("encrypted").boolValue());
//
//            long accountId;
//            std::string decryptedPayload;
//
//            if (isEncrypted) {
//                // Decrypt query
//                std::string encryptedPayload = msg->par("payload").stringValue();
//                decryptedPayload = xorDecrypt(encryptedPayload);
//
//                // ✅ FIX: Parse accountId from DECRYPTED data
//                accountId = parseAccountId(decryptedPayload);
//
//                encryptedQueriesReceived++;
//
//                EV << "\n========================================\n";
//                EV << "🔓 XOR DECRYPTION - " << role << " DB\n";
//                EV << "========================================\n";
//                EV << "Time: " << simTime() << "s\n";
//                EV << "Client: PC " << clientAddr << "\n";
//                EV << "Encrypted (Hex): " << toHex(encryptedPayload) << "\n";
//                EV << "Decrypted: " << decryptedPayload << "\n";
//                EV << "Parsed Account ID: " << accountId << "\n";  // ✅ ADD THIS LINE
//                EV << "Total Encrypted Queries: " << encryptedQueriesReceived << "\n";
//                EV << "========================================\n\n";
//            } else {
//                // ✅ FIX: Fallback to message parameter for unencrypted
//                accountId = msg->hasPar("accountId") ? ACCOUNT_ID(msg) : 0;
//            }
//
//            EV << "\n📖 [" << role << " DB] READ query from client " << clientAddr
//               << " (IP: " << ipToStr(clientIP) << ")\n";
//            EV << "   Query: SELECT * FROM accounts WHERE id=" << accountId << "\n";
//
//            // Lookup account (NOW WILL WORK!)
//            long balance = accountBalances.count(accountId) ? accountBalances[accountId] : 0;
//            std::string name = accountNames.count(accountId) ? accountNames[accountId] : "Unknown";
//
//            if (balance > 0) {
//                EV << "   ✓ Found: Account " << accountId << " (" << name << ")\n";
//                EV << "   ✓ Balance: " << balance << " BDT\n";
//            } else {
//                EV << "   ✗ Account " << accountId << " not found in database\n";
//            }
//
//            // ============ RESPONSE ENCRYPTION ============
//            if (isEncrypted) {
//                // Send encrypted response
//                sendEncryptedResponse(clientAddr, clientIP, accountId, balance, name);
//            } else {
//                // Unencrypted response (backward compatibility)
//                auto *response = mk("DB_RESPONSE", DB_RESPONSE_SUCCESS, address, clientAddr);
//                response->par("srcIP") = myIP;
//                response->par("dstIP") = clientIP;
//                response->par("accountId") = accountId;
//                response->par("amount") = balance;
//                response->par("payload") = name.c_str();
//
//                double responseTime = 0.010;
//                sendDelayed(response, responseTime, "tcpOut");
//                totalResponseTime += responseTime;
//                EV << "   → Response sent in " << (responseTime * 1000) << "ms\n";
//            }
//        }
        if (kind == DB_QUERY_READ) {
            readQueries++;
            queriesLastSecond++;
            totalQueries++;

            // ============ DECRYPTION CHECK ============
            bool isEncrypted = (msg->hasPar("encrypted") && msg->par("encrypted").boolValue());

            long accountId;
            std::string decryptedPayload;

            if (isEncrypted) {
                // ✅ Check if payload exists
                if (!msg->hasPar("payload")) {
                    EV << "❌ ERROR: No 'payload' parameter in encrypted query!\n";
                    delete msg;
                    return;
                }

                // Decrypt query
                std::string encryptedPayload = msg->par("payload").stringValue();

                // ✅ Check if encrypted data is valid
                if (encryptedPayload.empty()) {
                    EV << "❌ ERROR: Encrypted query is EMPTY!\n";
                    delete msg;
                    return;
                }

                decryptedPayload = xorDecrypt(encryptedPayload);  // ✅ XOR_KEY automatically used

                // ✅ FIX: Parse accountId from DECRYPTED data
                accountId = parseAccountId(decryptedPayload);

                encryptedQueriesReceived++;

                EV << "\n========================================\n";
                EV << "🔓 XOR DECRYPTION - " << role << " DB\n";
                EV << "========================================\n";
                EV << "Time: " << simTime() << "s\n";
                EV << "Client: PC " << clientAddr << "\n";
                EV << "Encrypted Length: " << encryptedPayload.length() << " bytes\n";
                EV << "Encrypted (Hex): " << toHex(encryptedPayload) << "\n";
                EV << "Decrypted Length: " << decryptedPayload.length() << " bytes\n";
                EV << "Decrypted: " << decryptedPayload << "\n";
                EV << "Parsed Account ID: " << accountId << "\n";
                EV << "Total Encrypted Queries: " << encryptedQueriesReceived << "\n";
                EV << "========================================\n\n";
            } else {
                // ✅ FIX: Fallback to message parameter for unencrypted
                accountId = msg->hasPar("accountId") ? ACCOUNT_ID(msg) : 0;
            }

            EV << "\n📖 [" << role << " DB] READ query from client " << clientAddr
               << " (IP: " << ipToStr(clientIP) << ")\n";
            EV << "   Query: SELECT * FROM accounts WHERE id=" << accountId << "\n";

            // Lookup account (NOW WILL WORK!)
            long balance = accountBalances.count(accountId) ? accountBalances[accountId] : 0;
            std::string name = accountNames.count(accountId) ? accountNames[accountId] : "Unknown";

            if (balance > 0) {
                EV << "   ✓ Found: Account " << accountId << " (" << name << ")\n";
                EV << "   ✓ Balance: " << balance << " BDT\n";
            } else {
                EV << "   ✗ Account " << accountId << " not found in database\n";
            }

            // ============ RESPONSE ENCRYPTION ============
            if (isEncrypted) {
                // Send encrypted response
                sendEncryptedResponse(clientAddr, clientIP, accountId, balance, name);
            } else {
                // Unencrypted response (backward compatibility)
                auto *response = mk("DB_RESPONSE", DB_RESPONSE_SUCCESS, address, clientAddr);
                response->par("srcIP") = myIP;
                response->par("dstIP") = clientIP;
                response->par("accountId") = accountId;
                response->par("amount") = balance;
                response->par("payload") = name.c_str();

                double responseTime = 0.010;
                sendDelayed(response, responseTime, "tcpOut");
                totalResponseTime += responseTime;
                EV << "   → Response sent in " << (responseTime * 1000) << "ms\n";
            }
        }

        else if (kind == DB_QUERY_WRITE) {
            if (role != "primary") {
                EV << "\n❌ [" << role << " DB] WRITE rejected (read-only replica)\n";
                auto *error = mk("DB_ERROR", DB_RESPONSE_ERROR, address, clientAddr);
                error->par("srcIP") = myIP;
                error->par("dstIP") = clientIP;
                error->par("payload") = "Only primary accepts writes";
                // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
                sendDelayed(error, 0.005, "tcpOut");
                delete msg;
                return;
            }

            writeQueries++;
            queriesLastSecond++;
            totalQueries++;

            long accountId = ACCOUNT_ID(msg);
            long amount = AMOUNT(msg);
            std::string queryType = QUERY_TYPE(msg);

            EV << "\n✏️  [PRIMARY DB] WRITE query from client " << clientAddr << "\n";
            EV << "   Operation: " << queryType << " on account " << accountId << "\n";
            EV << "   Amount: " << amount << " BDT\n";

            long oldBalance = accountBalances[accountId];
            long newBalance = oldBalance;
            bool success = true;

            if (queryType == "WITHDRAW") {
                if (oldBalance >= amount) {
                    newBalance = oldBalance - amount;
                    accountBalances[accountId] = newBalance;
                    EV << "   ✓ Withdrawal successful\n";
                } else {
                    success = false;
                    EV << "   ✗ Insufficient funds! (Balance: " << oldBalance
                       << ", Requested: " << amount << ")\n";
                }
            } else if (queryType == "DEPOSIT") {
                newBalance = oldBalance + amount;
                accountBalances[accountId] = newBalance;
                EV << "   ✓ Deposit successful\n";
            } else if (queryType == "UPDATE") {
                newBalance = amount;
                accountBalances[accountId] = newBalance;
                EV << "   ✓ Balance updated\n";
            }

            if (success) {
                EV << "   📊 Account " << accountId << ": " << oldBalance
                   << " → " << newBalance << " BDT (change: "
                   << (newBalance - oldBalance) << ")\n";

                // Simulate replication to secondary
                EV << "   🔄 Replicating to secondary DB...\n";
                syncOperations++;
            }

            // Send response
            auto *response = mk("DB_RESPONSE",
                              success ? DB_RESPONSE_SUCCESS : DB_RESPONSE_ERROR,
                              address, clientAddr);
            response->par("srcIP") = myIP;
            response->par("dstIP") = clientIP;
            response->par("accountId") = accountId;
            response->par("amount") = newBalance;

            double responseTime = 0.025;  // 25ms for writes
            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(response, responseTime, "tcpOut");
            totalResponseTime += responseTime;

            EV << "   → Write completed in " << (responseTime * 1000) << "ms\n\n";
        }
        else if (kind == PING) {
            // Health check
            auto *reply = mk("PONG", PONG, address, clientAddr);
            reply->par("srcIP") = myIP;
            reply->par("dstIP") = clientIP;
            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(reply, 0.005, "tcpOut");
        }

        delete msg;
    }

    // Encryption
    void sendEncryptedResponse(long clientAddr, long clientIP, int accountId, long balance, const std::string& name) {
        // Prepare response data
        std::stringstream responseData;
        responseData << "ACCOUNT:" << accountId
                     << "|BALANCE:" << balance
                     << "|NAME:" << name
                     << "|STATUS:SUCCESS";

        std::string plaintext = responseData.str();
        std::string encrypted = xorEncrypt(plaintext);

        encryptedResponsesSent++;

        EV << "\n========================================\n";
        EV << "🔒 XOR ENCRYPTION - " << role << " DB\n";
        EV << "========================================\n";
        EV << "Client: PC " << clientAddr << "\n";
        EV << "Plaintext: " << plaintext << "\n";
        EV << "Plaintext Length: " << plaintext.length() << " bytes\n";
        EV << "Encrypted (Hex): " << toHex(encrypted) << "\n";
        EV << "Encrypted Length: " << encrypted.length() << " bytes\n";
        EV << "Total Encrypted Responses: " << encryptedResponsesSent << "\n";

        // ✅ ADD DEBUG: Test if string is valid before message creation
        EV << "DEBUG: encrypted string address: " << (void*)encrypted.c_str() << "\n";
        EV << "DEBUG: encrypted string content: [" << encrypted << "]\n";
        EV << "========================================\n\n";

        // Create response
        auto *response = mk("DB_RESPONSE", DB_RESPONSE_SUCCESS, address, clientAddr);

        // ✅ SET PARAMETERS IN CORRECT ORDER
        response->par("srcIP") = myIP;
        response->par("dstIP") = clientIP;
        response->par("accountId") = accountId;          // ← Add this!
        response->par("amount") = balance;                // ← Add this!
        response->par("payload").setStringValue(encrypted.c_str());
        response->addPar("encrypted") = true;

        // ✅ ADD DEBUG: Verify parameter was set
        EV << "DEBUG: Payload parameter set: " << response->par("payload").stringValue() << "\n";
        EV << "DEBUG: Payload length in message: " << strlen(response->par("payload").stringValue()) << "\n\n";

        double responseTime = 0.010;
        sendDelayed(response, responseTime, "tcpOut");
        totalResponseTime += responseTime;

        EV << "   → Encrypted response sent in " << (responseTime * 1000) << "ms\n";
    }


    void finish() override {
        double avgResponseTime = totalQueries > 0 ? (totalResponseTime / totalQueries) * 1000 : 0;

        EV << "\n";
        EV << "========================================\n";
        EV << "💾 DATABASE SERVER [" << role << "] FINAL STATISTICS\n";
        EV << "========================================\n";

        // ADD THIS SECTION
        EV << "🔒 ENCRYPTION STATISTICS:\n";
        EV << "----------------------------------------\n";
        EV << "Encrypted Queries Received: " << encryptedQueriesReceived << "\n";
        EV << "Encrypted Responses Sent: " << encryptedResponsesSent << "\n";
        EV << "Encryption Rate: "
           << (totalQueries > 0 ? (encryptedQueriesReceived * 100.0 / totalQueries) : 0)
           << "%\n\n";
        // END NEW SECTION

        EV << "Total queries processed: " << totalQueries << "\n";
        EV << "  - READ queries: " << readQueries << " ("
           << (totalQueries > 0 ? (readQueries * 100.0 / totalQueries) : 0) << "%)\n";
        EV << "  - WRITE queries: " << writeQueries << " ("
           << (totalQueries > 0 ? (writeQueries * 100.0 / totalQueries) : 0) << "%)\n";
        EV << "Replication operations: " << syncOperations << "\n";
        EV << "Average response time: " << avgResponseTime << " ms\n";
        EV << "Peak load: " << peakLoad << "%\n";
        EV << "Accounts in database: " << accountBalances.size() << "\n";

        long totalBalance = 0;
        for (auto &entry : accountBalances) totalBalance += entry.second;
        EV << "Total balance: " << totalBalance << " BDT\n";
        EV << "Average balance: " << (totalBalance / accountBalances.size()) << " BDT\n";
        EV << "========================================\n\n";

        recordScalar("totalQueries", totalQueries);
        recordScalar("readQueries", readQueries);
        recordScalar("writeQueries", writeQueries);
        recordScalar("avgResponseTime_ms", avgResponseTime);
        recordScalar("peakLoad_percent", peakLoad);
        recordScalar("totalBalance_BDT", totalBalance);

        cancelAndDelete(loadCheckTimer);
    }
};

Define_Module(DatabaseApp);
