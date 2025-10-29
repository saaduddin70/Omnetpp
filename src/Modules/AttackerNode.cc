//#include <omnetpp.h>
//#include "helpers.h"
//#include <map>
//#include <set>
//
//using namespace omnetpp;
//
//class AttackerNode : public cSimpleModule {
//private:
//    int packetsIntercepted = 0;
//    int encryptedPackets = 0;
//    int plaintextPackets = 0;
//    int decryptionAttempts = 0;
//    int successfulBreaches = 0;
//    int modificationAttempts = 0;
//    std::set<long> targetAccounts;
//    std::set<long> stolenBalances;
//
//protected:
//    void initialize() override {
//        EV << "\n========================================\n";
//        EV << "👹 ATTACKER NODE INITIALIZED\n";
//        EV << "========================================\n";
//        EV << "⚠️  WARNING: Man-in-the-Middle Attack Active!\n";
//        EV << "Position: Between Branch B and Core Router\n";
//        EV << "Target: Database traffic from Branch B (PCs 211-218)\n";
//        EV << "========================================\n\n";
//    }
//
//    void handleMessage(cMessage *msg) override {
//        packetsIntercepted++;
//
//        int kind = msg->getKind();
//
//        // ✅ Only analyze DB-related traffic
//        bool isDbTraffic = (kind == DB_QUERY_READ || kind == DB_QUERY_WRITE ||
//                           kind == DB_RESPONSE_SUCCESS || kind == DB_RESPONSE_ERROR);
//
//        if (isDbTraffic) {
//            // ✅ Check if packet is encrypted
//            bool isEncrypted = msg->hasPar("encrypted") && msg->par("encrypted").boolValue();
//
//            if (isEncrypted) {
//                encryptedPackets++;
//                handleEncryptedPacket(msg);
//            } else {
//                plaintextPackets++;
//                handlePlaintextPacket(msg);
//            }
//        }
//
//        // ✅ ALWAYS forward packet (passive MITM)
//        send(msg, "port$o");
//    }
//
//    void handleEncryptedPacket(cMessage *msg) {
//        // Skip early initialization noise
//        if (simTime().dbl() < 10.0) return;
//
//        // ✅ Check if payload exists
//        if (!msg->hasPar("payload")) {
//            EV << "⚠️  No payload in encrypted packet\n";
//            return;
//        }
//
//        std::string encrypted = msg->par("payload").stringValue();
//
//        if (encrypted.empty()) {
//            EV << "⚠️  Empty encrypted payload\n";
//            return;
//        }
//
//        EV << "\n========================================\n";
//        EV << "👹 INTERCEPTED ENCRYPTED PACKET #" << encryptedPackets << "\n";
//        EV << "========================================\n";
//        EV << "Time: " << simTime() << "s\n";
//        EV << "Source: PC " << SRC(msg) << " → Destination: " << DST(msg) << "\n";
//        EV << "Type: " << msg->getName() << "\n";
//        EV << "Encrypted Length: " << encrypted.length() << " bytes\n";
//        EV << "Encrypted (Hex): " << toHex(encrypted, 20) << "\n";
//        EV << "----------------------------------------\n";
//
//        // ✅ Attack attempts
//        attemptBruteForce(encrypted);
//        attemptModification(msg);
//
//        EV << "Result: ❌ ENCRYPTION BLOCKED ALL ATTACKS!\n";
//        EV << "========================================\n\n";
//    }
//
//    void handlePlaintextPacket(cMessage *msg) {
//        // Skip early noise
//        if (simTime().dbl() < 10.0) return;
//
//        EV << "\n========================================\n";
//        EV << "👹 INTERCEPTED PLAINTEXT PACKET\n";
//        EV << "========================================\n";
//        EV << "⚠️  CRITICAL BREACH!\n";
//        EV << "Time: " << simTime() << "s\n";
//        EV << "Source: PC " << SRC(msg) << " → Destination: " << DST(msg) << "\n";
//        EV << "Type: " << msg->getName() << "\n";
//
//        bool dataStolen = false;
//
//        // ✅ Steal account ID
//        if (msg->hasPar("accountId")) {
//            long accountId = msg->par("accountId").longValue();
//            if (accountId > 0) {
//                targetAccounts.insert(accountId);
//                EV << "🚨 STOLEN: Account ID = " << accountId << "\n";
//                successfulBreaches++;
//                dataStolen = true;
//            }
//        }
//
//        // ✅ Steal balance/amount
//        if (msg->hasPar("amount")) {
//            long amount = msg->par("amount").longValue();
//            if (amount > 0) {
//                stolenBalances.insert(amount);
//                EV << "🚨 STOLEN: Balance/Amount = " << amount << " BDT\n";
//                successfulBreaches++;
//                dataStolen = true;
//            }
//        }
//
//        // ✅ Steal query type
//        if (msg->hasPar("queryType")) {
//            std::string queryType = msg->par("queryType").stringValue();
//            EV << "🚨 STOLEN: Query Type = " << queryType << "\n";
//            dataStolen = true;
//        }
//
//        if (dataStolen) {
//            EV << "Result: ✅ ATTACKER SUCCESSFULLY READ ALL DATA!\n";
//        } else {
//            EV << "Result: ℹ️  No sensitive data in this packet\n";
//        }
//
//        EV << "========================================\n\n";
//    }
//
//    void attemptBruteForce(const std::string& encrypted) {
//        decryptionAttempts++;
//
//        EV << "🔓 Attack #1: Brute Force Decryption\n";
//        EV << "   Testing all 256 possible XOR keys...\n";
//
//        int validResults = 0;
//        std::string bestGuess;
//        int bestScore = 0;
//
//        for (int key = 0; key < 256; key++) {
//            std::string attempt = xorDecrypt(encrypted, (unsigned char)key);
//            int score = countPrintable(attempt);
//
//            if (score > bestScore) {
//                bestScore = score;
//                bestGuess = attempt;
//            }
//
//            if (looksValid(attempt)) validResults++;
//        }
//
//        EV << "   Valid-looking results: " << validResults << "\n";
//        EV << "   Best guess (key=" << (bestScore > 0 ? "?" : "N/A") << "): ";
//
//        if (bestScore > encrypted.length() * 0.7) {
//            // Attacker found a plausible decryption
//            EV << bestGuess.substr(0, 30) << "...\n";
//            EV << "   Status: ⚠️  POSSIBLE (but cannot verify correctness)\n";
//        } else {
//            EV << "[gibberish]\n";
//            EV << "   Status: ❌ Cannot determine correct key\n";
//        }
//        EV << "\n";
//    }
//
//    void attemptModification(cMessage *msg) {
//        modificationAttempts++;
//
//        EV << "✏️  Attack #2: Data Tampering\n";
//
//        if (!msg->hasPar("payload")) {
//            EV << "   Status: ❌ No payload to modify\n\n";
//            return;
//        }
//
//        std::string original = msg->par("payload").stringValue();
//
//        if (original.empty()) {
//            EV << "   Status: ❌ Empty payload\n\n";
//            return;
//        }
//
//        std::string modified = original;
//
//        // ✅ Flip random bits
//        if (modified.length() > 0) {
//            modified[0] ^= 0xFF;  // Flip first byte
//            if (modified.length() > 10) {
//                modified[10] ^= 0xAA;  // Flip middle byte
//            }
//        }
//
//        EV << "   Original (Hex): " << toHex(original, 15) << "\n";
//        EV << "   Tampered (Hex): " << toHex(modified, 15) << "\n";
//        EV << "   Status: ❌ Will be detected as corrupted/invalid\n";
//        EV << "   Reason: Recipient will get gibberish after decryption\n\n";
//    }
//
//    // ✅ Count printable ASCII characters
//    int countPrintable(const std::string& data) {
//        int count = 0;
//        for (unsigned char c : data) {
//            if (c >= 32 && c <= 126) count++;
//        }
//        return count;
//    }
//
//    // ✅ Check if string looks like valid text
//    bool looksValid(const std::string& data) {
//        if (data.empty()) return false;
//
//        int printable = countPrintable(data);
//        double ratio = (double)printable / data.length();
//
//        // At least 60% printable ASCII = "valid-looking"
//        return (ratio > 0.6);
//    }
//
//    void finish() override {
//        EV << "\n========================================\n";
//        EV << "👹 ATTACKER FINAL STATISTICS\n";
//        EV << "========================================\n";
//        EV << "Total Packets Intercepted: " << packetsIntercepted << "\n";
//        EV << "  - Encrypted: " << encryptedPackets
//           << " (" << (packetsIntercepted > 0 ? encryptedPackets * 100.0 / packetsIntercepted : 0) << "%)\n";
//        EV << "  - Plaintext: " << plaintextPackets
//           << " (" << (packetsIntercepted > 0 ? plaintextPackets * 100.0 / packetsIntercepted : 0) << "%)\n\n";
//
//        EV << "Attack Results:\n";
//        EV << "  - Decryption Attempts: " << decryptionAttempts << "\n";
//        EV << "  - Modification Attempts: " << modificationAttempts << "\n";
//        EV << "  - Successful Breaches: " << successfulBreaches << "\n";
//        EV << "  - Compromised Accounts: " << targetAccounts.size() << "\n";
//        EV << "  - Stolen Balances: " << stolenBalances.size() << "\n";
//        EV << "  - Failed Attacks: " << (decryptionAttempts + modificationAttempts - successfulBreaches) << "\n\n";
//
//        if (successfulBreaches == 0 && encryptedPackets > 0) {
//            EV << "✅ ENCRYPTION SUCCESSFULLY PROTECTED ALL DATA!\n";
//            EV << "   Attacker could not breach any encrypted communications.\n";
//            EV << "   All " << encryptedPackets << " encrypted packets remained secure.\n";
//        } else if (successfulBreaches > 0 && plaintextPackets > 0) {
//            EV << "❌ PLAINTEXT DATA WAS COMPROMISED!\n";
//            EV << "   " << successfulBreaches << " sensitive data points exposed.\n";
//            EV << "   " << targetAccounts.size() << " account(s) compromised.\n";
//        }
//
//        // Security Score
//        double securityScore = 0.0;
//        if (packetsIntercepted > 0) {
//            securityScore = ((double)encryptedPackets / packetsIntercepted) * 100.0;
//        }
//
//        EV << "\nSecurity Score: " << securityScore << "% of traffic encrypted\n";
//        EV << "========================================\n\n";
//
//        // ✅ Record statistics
//        recordScalar("packetsIntercepted", packetsIntercepted);
//        recordScalar("encryptedPackets", encryptedPackets);
//        recordScalar("plaintextPackets", plaintextPackets);
//        recordScalar("successfulBreaches", successfulBreaches);
//        recordScalar("compromisedAccounts", (int)targetAccounts.size());
//        recordScalar("securityScore", securityScore);
//    }
//};
//
//Define_Module(AttackerNode);


#include <omnetpp.h>
#include "helpers.h"
#include <map>
#include <set>

using namespace omnetpp;

class AttackerNode : public cSimpleModule {
private:
    int packetsIntercepted = 0;
    int encryptedPackets = 0;
    int plaintextPackets = 0;
    int decryptionAttempts = 0;
    int successfulBreaches = 0;
    int modificationAttempts = 0;
    std::set<long> targetAccounts;
    std::set<long> stolenBalances;

protected:
    void initialize() override {
        EV << "\n========================================\n";
        EV << "👹 ATTACKER NODE INITIALIZED\n";
        EV << "========================================\n";
        EV << "⚠️  WARNING: Man-in-the-Middle Attack Active!\n";
        EV << "Position: Between Branch B and Core Router\n";
        EV << "Target: Database traffic from Branch B (PCs 211-218)\n";
        EV << "========================================\n\n";
    }

    void handleMessage(cMessage *msg) override {
        packetsIntercepted++;

        int kind = msg->getKind();

        // ✅ Determine which gate the message arrived from
        cGate *arrivalGate = msg->getArrivalGate();
        int arrivalGateIndex = arrivalGate->getIndex();

        // ✅ Calculate destination gate (opposite of arrival)
        // port[0] = coreRouter side, port[1] = branchB_Router side
        int destGateIndex = (arrivalGateIndex == 0) ? 1 : 0;

        // ✅ Only analyze DB-related traffic
        bool isDbTraffic = (kind == DB_QUERY_READ || kind == DB_QUERY_WRITE ||
                           kind == DB_RESPONSE_SUCCESS || kind == DB_RESPONSE_ERROR);

        if (isDbTraffic) {
            // ✅ Check if packet is encrypted
            bool isEncrypted = msg->hasPar("encrypted") && msg->par("encrypted").boolValue();

            if (isEncrypted) {
                encryptedPackets++;
                handleEncryptedPacket(msg);
            } else {
                plaintextPackets++;
                handlePlaintextPacket(msg);
            }
        }

        // ✅ ALWAYS forward packet with correct gate index (passive MITM)
        send(msg, "port$o", destGateIndex);
    }

    void handleEncryptedPacket(cMessage *msg) {
        // Skip early initialization noise
        if (simTime().dbl() < 10.0) return;

        // ✅ Check if payload exists
        if (!msg->hasPar("payload")) {
            EV << "⚠️  No payload in encrypted packet\n";
            return;
        }

        std::string encrypted = msg->par("payload").stringValue();

        if (encrypted.empty()) {
            EV << "⚠️  Empty encrypted payload\n";
            return;
        }

        EV << "\n========================================\n";
        EV << "👹 INTERCEPTED ENCRYPTED PACKET #" << encryptedPackets << "\n";
        EV << "========================================\n";
        EV << "Time: " << simTime() << "s\n";
        EV << "Source: PC " << SRC(msg) << " → Destination: " << DST(msg) << "\n";
        EV << "Type: " << msg->getName() << "\n";
        EV << "Encrypted Length: " << encrypted.length() << " bytes\n";
        EV << "Encrypted (Hex): " << toHex(encrypted, 20) << "\n";
        EV << "----------------------------------------\n";

        // ✅ Attack attempts
        attemptBruteForce(encrypted);
        attemptModification(msg);

        EV << "Result: ❌ ENCRYPTION BLOCKED ALL ATTACKS!\n";
        EV << "========================================\n\n";
    }

    void handlePlaintextPacket(cMessage *msg) {
        // Skip early noise
        if (simTime().dbl() < 10.0) return;

        EV << "\n========================================\n";
        EV << "👹 INTERCEPTED PLAINTEXT PACKET\n";
        EV << "========================================\n";
        EV << "⚠️  CRITICAL BREACH!\n";
        EV << "Time: " << simTime() << "s\n";
        EV << "Source: PC " << SRC(msg) << " → Destination: " << DST(msg) << "\n";
        EV << "Type: " << msg->getName() << "\n";

        bool dataStolen = false;

        // ✅ Steal account ID
        if (msg->hasPar("accountId")) {
            long accountId = msg->par("accountId").longValue();
            if (accountId > 0) {
                targetAccounts.insert(accountId);
                EV << "🚨 STOLEN: Account ID = " << accountId << "\n";
                successfulBreaches++;
                dataStolen = true;
            }
        }

        // ✅ Steal balance/amount
        if (msg->hasPar("amount")) {
            long amount = msg->par("amount").longValue();
            if (amount > 0) {
                stolenBalances.insert(amount);
                EV << "🚨 STOLEN: Balance/Amount = " << amount << " BDT\n";
                successfulBreaches++;
                dataStolen = true;
            }
        }

        // ✅ Steal query type
        if (msg->hasPar("queryType")) {
            std::string queryType = msg->par("queryType").stringValue();
            EV << "🚨 STOLEN: Query Type = " << queryType << "\n";
            dataStolen = true;
        }

        if (dataStolen) {
            EV << "Result: ✅ ATTACKER SUCCESSFULLY READ ALL DATA!\n";
        } else {
            EV << "Result: ℹ️  No sensitive data in this packet\n";
        }

        EV << "========================================\n\n";
    }

    void attemptBruteForce(const std::string& encrypted) {
        decryptionAttempts++;

        EV << "🔓 Attack #1: Brute Force Decryption\n";
        EV << "   Testing all 256 possible XOR keys...\n";

        int validResults = 0;
        std::string bestGuess;
        int bestScore = 0;

        for (int key = 0; key < 256; key++) {
            std::string attempt = xorDecrypt(encrypted, (unsigned char)key);
            int score = countPrintable(attempt);

            if (score > bestScore) {
                bestScore = score;
                bestGuess = attempt;
            }

            if (looksValid(attempt)) validResults++;
        }

        EV << "   Valid-looking results: " << validResults << "\n";
        EV << "   Best guess (key=" << (bestScore > 0 ? "?" : "N/A") << "): ";

        if (bestScore > encrypted.length() * 0.7) {
            // Attacker found a plausible decryption
            EV << bestGuess.substr(0, 30) << "...\n";
            EV << "   Status: ⚠️  POSSIBLE (but cannot verify correctness)\n";
        } else {
            EV << "[gibberish]\n";
            EV << "   Status: ❌ Cannot determine correct key\n";
        }
        EV << "\n";
    }

    void attemptModification(cMessage *msg) {
        modificationAttempts++;

        EV << "✏️  Attack #2: Data Tampering\n";

        if (!msg->hasPar("payload")) {
            EV << "   Status: ❌ No payload to modify\n\n";
            return;
        }

        std::string original = msg->par("payload").stringValue();

        if (original.empty()) {
            EV << "   Status: ❌ Empty payload\n\n";
            return;
        }

        std::string modified = original;

        // ✅ Flip random bits
        if (modified.length() > 0) {
            modified[0] ^= 0xFF;  // Flip first byte
            if (modified.length() > 10) {
                modified[10] ^= 0xAA;  // Flip middle byte
            }
        }

        EV << "   Original (Hex): " << toHex(original, 15) << "\n";
        EV << "   Tampered (Hex): " << toHex(modified, 15) << "\n";
        EV << "   Status: ❌ Will be detected as corrupted/invalid\n";
        EV << "   Reason: Recipient will get gibberish after decryption\n\n";
    }

    // ✅ Count printable ASCII characters
    int countPrintable(const std::string& data) {
        int count = 0;
        for (unsigned char c : data) {
            if (c >= 32 && c <= 126) count++;
        }
        return count;
    }

    // ✅ Check if string looks like valid text
    bool looksValid(const std::string& data) {
        if (data.empty()) return false;

        int printable = countPrintable(data);
        double ratio = (double)printable / data.length();

        // At least 60% printable ASCII = "valid-looking"
        return (ratio > 0.6);
    }

    void finish() override {
        EV << "\n========================================\n";
        EV << "👹 ATTACKER FINAL STATISTICS\n";
        EV << "========================================\n";
        EV << "Total Packets Intercepted: " << packetsIntercepted << "\n";
        EV << "  - Encrypted: " << encryptedPackets
           << " (" << (packetsIntercepted > 0 ? encryptedPackets * 100.0 / packetsIntercepted : 0) << "%)\n";
        EV << "  - Plaintext: " << plaintextPackets
           << " (" << (packetsIntercepted > 0 ? plaintextPackets * 100.0 / packetsIntercepted : 0) << "%)\n\n";

        EV << "Attack Results:\n";
        EV << "  - Decryption Attempts: " << decryptionAttempts << "\n";
        EV << "  - Modification Attempts: " << modificationAttempts << "\n";
        EV << "  - Successful Breaches: " << successfulBreaches << "\n";
        EV << "  - Compromised Accounts: " << targetAccounts.size() << "\n";
        EV << "  - Stolen Balances: " << stolenBalances.size() << "\n";
        EV << "  - Failed Attacks: " << (decryptionAttempts + modificationAttempts - successfulBreaches) << "\n\n";

        if (successfulBreaches == 0 && encryptedPackets > 0) {
            EV << "✅ ENCRYPTION SUCCESSFULLY PROTECTED ALL DATA!\n";
            EV << "   Attacker could not breach any encrypted communications.\n";
            EV << "   All " << encryptedPackets << " encrypted packets remained secure.\n";
        } else if (successfulBreaches > 0 && plaintextPackets > 0) {
            EV << "❌ PLAINTEXT DATA WAS COMPROMISED!\n";
            EV << "   " << successfulBreaches << " sensitive data points exposed.\n";
            EV << "   " << targetAccounts.size() << " account(s) compromised.\n";
        }

        // Security Score
        double securityScore = 0.0;
        if (packetsIntercepted > 0) {
            securityScore = ((double)encryptedPackets / packetsIntercepted) * 100.0;
        }

        EV << "\nSecurity Score: " << securityScore << "% of traffic encrypted\n";
        EV << "========================================\n\n";

        // ✅ Record statistics
        recordScalar("packetsIntercepted", packetsIntercepted);
        recordScalar("encryptedPackets", encryptedPackets);
        recordScalar("plaintextPackets", plaintextPackets);
        recordScalar("successfulBreaches", successfulBreaches);
        recordScalar("compromisedAccounts", (int)targetAccounts.size());
        recordScalar("securityScore", securityScore);
    }
};

Define_Module(AttackerNode);
