//#include <omnetpp.h>
//#include "helpers.h"
//#include <map>
//
//using namespace omnetpp;
//
//class PC : public cSimpleModule {
//private:
//    int address;
//    int vlanId;
//    long myIP = 0;
//    bool ipConfigured = false;
//    int dhcpState = 0;
//    int appState = 0;  // 0: DNS, 1: DB, 2: HTTP, 3: SMTP
//
//    std::map<std::string, long> dnsCache;
//    long dbPrimaryIP = 0;
//    long dbSecondaryIP = 0;
//
//    int dnsQueries = 0;
//    int httpRequests = 0;
//    int emailsSent = 0;
//    int dbQueries = 0;
//
//    cMessage *appTestTimer = nullptr;
//
//protected:
//    void initialize() override {
//        address = par("address");
//        vlanId = par("vlanId").intValue();
//
//        std::string location;
//        if (address >= 111 && address <= 118) location = "Branch A";
//        else if (address >= 211 && address <= 218) location = "Branch B";
//        else if (address >= 311 && address <= 318) location = "Branch C";
//        else location = "HQ";
//
//        EV << "💻 PC " << address << " (" << location << ", VLAN " << vlanId << ") initialized\n";
//
//        scheduleAt(simTime() + uniform(0.5, 2.0), new cMessage("startDHCP"));
//    }
//
//    void handleMessage(cMessage *msg) override {
//        if (msg->isSelfMessage()) {
//            if (strcmp(msg->getName(), "startDHCP") == 0) {
//                dhcpState = 1;
//                auto *discover = mk("DHCP_DISCOVER", DHCP_DISCOVER, address, 10);
//                discover->par("vlanId") = vlanId;
//                send(discover, "ppp$o");
//                delete msg;
//            }
//            else if (strcmp(msg->getName(), "appTest") == 0) {
//                if (!ipConfigured) {
//                    scheduleAt(simTime() + 1.0, msg);
//                    return;
//                }
//
//                if (appState == 0) {
//                    // DNS Query for DB servers
//                    EV << "\n→ PC " << address << ": DNS query for 'db-primary.server.com'\n";
//                    auto *query = mk("DNS_QUERY", DNS_QUERY, address, 11);
//                    query->par("srcIP") = myIP;
//                    query->par("requestedName") = "db-primary.server.com";
//                    send(query, "ppp$o");
//                    dnsQueries++;
//                    appState = 1;
//                }
//                else if (appState == 1) {
//                    // Database Query
//                    if (dbPrimaryIP == 0 && dbSecondaryIP == 0) {
//                        EV << "⚠️  PC " << address << ": No DB server IP yet, skipping DB query\n";
//                        appState = 2;
//                    } else {
//                        // Decide: 30% READ to primary, 70% READ to secondary
//                        bool usePrimary = (uniform(0, 1) < 0.3);
//                        long targetDB = usePrimary ? dbPrimaryIP : dbSecondaryIP;
//                        int targetAddr = usePrimary ? 20 : 21;
//
//                        if (targetDB == 0) {
//                            targetDB = dbPrimaryIP;
//                            targetAddr = 20;
//                        }
//
//                        // Account ID = address offset (111 → 1001, 211 → 1011, etc.)
//                        long accountId = 1000 + (address % 100);
//
//                        // 80% READ, 20% WRITE
//                        bool isWrite = (uniform(0, 1) < 0.2);
//
//                        if (isWrite && !usePrimary) {
//                            // Force write to primary
//                            targetDB = dbPrimaryIP;
//                            targetAddr = 20;
//                        }
//
//                        if (isWrite) {
//                            EV << "\n→ PC " << address << ": DB WRITE query to PRIMARY (account " << accountId << ")\n";
//                            auto *writeQuery = mk("DB_WRITE", DB_QUERY_WRITE, address, targetAddr);
//                            writeQuery->par("srcIP") = myIP;
//                            writeQuery->par("dstIP") = targetDB;
//                            writeQuery->par("accountId") = accountId;
//                            writeQuery->par("amount") = intuniform(1000, 5000);
//                            writeQuery->par("queryType") = "WITHDRAW";
//                            send(writeQuery, "ppp$o");
//                        } else {
//                            EV << "\n→ PC " << address << ": DB READ query to "
//                               << (usePrimary ? "PRIMARY" : "SECONDARY")
//                               << " (account " << accountId << ")\n";
//                            auto *readQuery = mk("DB_READ", DB_QUERY_READ, address, targetAddr);
//                            readQuery->par("srcIP") = myIP;
//                            readQuery->par("dstIP") = targetDB;
//                            readQuery->par("accountId") = accountId;
//                            send(readQuery, "ppp$o");
//                        }
//
//                        dbQueries++;
//                        appState = 2;
//                    }
//                }
//                else if (appState == 2) {
//                    // HTTP GET
//                    EV << "\n→ PC " << address << ": HTTP GET to web.server.com\n";
//                    auto *httpGet = mk("HTTP_GET", HTTP_GET, address, 13);
//                    httpGet->par("srcIP") = myIP;
//                    httpGet->par("payload") = "GET /index.html";
//                    send(httpGet, "ppp$o");
//                    httpRequests++;
//                    appState = 3;
//                }
//                else if (appState == 3) {
//                    // SMTP Send
//                    EV << "\n→ PC " << address << ": Sending email via SMTP\n";
//                    auto *email = mk("SMTP_SEND", SMTP_SEND, address, 12);
//                    email->par("srcIP") = myIP;
//                    email->par("payload") = "Email from PC";
//                    send(email, "ppp$o");
//                    emailsSent++;
//                    appState = 0;  // Cycle back
//                }
//
//                // Reschedule (reduced interval for more traffic)
//                scheduleAt(simTime() + uniform(8.0, 15.0), msg);
//            }
//        } else {
//            int kind = msg->getKind();
//
//            if (kind == DHCP_OFFER && dhcpState == 1) {
//                long offeredIP = OFFERED_IP(msg);
//                dhcpState = 2;
//
//                auto *request = mk("DHCP_REQUEST", DHCP_REQUEST, address, 10);
//                request->par("offeredIP") = offeredIP;
//                request->par("vlanId") = vlanId;
//                send(request, "ppp$o");
//            }
//            else if (kind == DHCP_ACK && dhcpState == 2) {
//                myIP = OFFERED_IP(msg);
//                ipConfigured = true;
//                dhcpState = 3;
//
//                EV << "✓ PC " << address << ": IP configured - " << ipToStr(myIP) << "\n";
//
//                if (!appTestTimer) {
//                    appTestTimer = new cMessage("appTest");
//                    scheduleAt(simTime() + uniform(3.0, 6.0), appTestTimer);
//                }
//            }
//            else if (kind == DNS_RESPONSE) {
//                std::string name = REQUESTED_NAME(msg);
//                long resolvedIP = OFFERED_IP(msg);
//
//                if (resolvedIP != 0) {
//                    dnsCache[name] = resolvedIP;
//
//                    if (name == "db-primary.server.com") {
//                        dbPrimaryIP = resolvedIP;
//                        EV << "✓ PC " << address << ": DNS resolved PRIMARY DB → "
//                           << ipToStr(resolvedIP) << "\n";
//
//                        // Query secondary too
//                        auto *query2 = mk("DNS_QUERY", DNS_QUERY, address, 11);
//                        query2->par("srcIP") = myIP;
//                        query2->par("requestedName") = "db-secondary.server.com";
//                        send(query2, "ppp$o");
//                    } else if (name == "db-secondary.server.com") {
//                        dbSecondaryIP = resolvedIP;
//                        EV << "✓ PC " << address << ": DNS resolved SECONDARY DB → "
//                           << ipToStr(resolvedIP) << "\n";
//                    } else {
//                        EV << "✓ PC " << address << ": DNS resolved " << name
//                           << " → " << ipToStr(resolvedIP) << "\n";
//                    }
//                }
//            }
//            else if (kind == DB_RESPONSE_SUCCESS) {
//                long accountId = ACCOUNT_ID(msg);
//                long balance = AMOUNT(msg);
//                EV << "✓ PC " << address << ": DB response - Account " << accountId
//                   << " balance: " << balance << " BDT\n";
//            }
//            else if (kind == DB_RESPONSE_ERROR) {
//                std::string error = PAYLOAD(msg);
//                EV << "✗ PC " << address << ": DB error - " << error << "\n";
//            }
//            else if (kind == HTTP_RESPONSE) {
//                EV << "✓ PC " << address << ": HTTP response received\n";
//            }
//            else if (kind == SMTP_ACK) {
//                EV << "✓ PC " << address << ": Email sent successfully\n";
//            }
//
//            delete msg;
//        }
//    }
//
//    void finish() override {
//        EV << "\n💻 PC " << address << " FINAL STATISTICS:\n";
//        EV << "  IP: " << ipToStr(myIP) << "\n";
//        EV << "  DNS queries: " << dnsQueries << "\n";
//        EV << "  DB queries: " << dbQueries << "\n";
//        EV << "  HTTP requests: " << httpRequests << "\n";
//        EV << "  Emails sent: " << emailsSent << "\n\n";
//
//        if (appTestTimer && appTestTimer->isScheduled()) {
//            cancelAndDelete(appTestTimer);
//        }
//    }
//};
//
//Define_Module(PC);






//#include <omnetpp.h>
//#include "helpers.h"
//#include <map>
//
//using namespace omnetpp;
//
//class PCApp : public cSimpleModule {
//private:
//    int address;
//    int vlanId;
//    long myIP = 0;
//    bool ipConfigured = false;
//    int dhcpState = 0;
//    int appState = 0;  // 0: DNS, 1: DB, 2: HTTP, 3: SMTP
//
//    std::map<std::string, long> dnsCache;
//    long dbPrimaryIP = 0;
//    long dbSecondaryIP = 0;
//
//    int dnsQueries = 0;
//    int httpRequests = 0;
//    int emailsSent = 0;
//    int dbQueries = 0;
//
//    cMessage *appTestTimer = nullptr;
//
//protected:
//    void initialize() override {
//        address = par("address");
//        vlanId = par("vlanId").intValue();
//
//        std::string location;
//        if (address >= 111 && address <= 118) location = "Branch A";
//        else if (address >= 211 && address <= 218) location = "Branch B";
//        else if (address >= 311 && address <= 318) location = "Branch C";
//        else location = "HQ";
//
//        EV << "========================================\n";
//        EV << "💻 PC MODULE INITIALIZED\n";
//        EV << "========================================\n";
//        EV << "Address: " << address << "\n";
//        EV << "Location: " << location << "\n";
//        EV << "VLAN: " << vlanId << "\n";
//        EV << "Module Path: " << getFullPath() << "\n";
//        EV << "========================================\n\n";
//
//        scheduleAt(simTime() + uniform(0.5, 2.0), new cMessage("startDHCP"));
//    }
//
//    void handleMessage(cMessage *msg) override {
//        if (msg->isSelfMessage()) {
//            if (strcmp(msg->getName(), "startDHCP") == 0) {
//                dhcpState = 1;
//
//                EV << "→ PC " << address << ": Starting DHCP process\n";
//
//                auto *discover = mk("DHCP_DISCOVER", DHCP_DISCOVER, address, 10);
//                discover->par("vlanId") = vlanId;
//                send(discover, "tcpOut");  // DHCP uses PPP directly
//                delete msg;
//            }
//            else if (strcmp(msg->getName(), "appTest") == 0) {
//                if (!ipConfigured) {
//                    scheduleAt(simTime() + 1.0, msg);
//                    return;
//                }
//
//                if (appState == 0) {
//                    // DNS Query for DB servers - USE TCP
//                    EV << "\n========================================\n";
//                    EV << "→ PC " << address << ": DNS QUERY [via TCP]\n";
//                    EV << "========================================\n";
//                    EV << "Target: db-primary.server.com\n";
//                    EV << "Using: tcpOut gate\n";
//                    EV << "========================================\n\n";
//
//                    auto *query = mk("DNS_QUERY", DNS_QUERY, address, 11);
//                    query->par("srcIP") = myIP;
//                    query->par("requestedName") = "db-primary.server.com";
//                    send(query, "tcpOut");  // ✅ Changed to TCP
//                    dnsQueries++;
//                    appState = 1;
//                }
//                else if (appState == 1) {
//                    // Database Query - USE TCP
//                    if (dbPrimaryIP == 0 && dbSecondaryIP == 0) {
//                        EV << "⚠️  PC " << address << ": No DB server IP yet, skipping DB query\n";
//                        appState = 2;
//                    } else {
//                        // Decide: 30% READ to primary, 70% READ to secondary
//                        bool usePrimary = (uniform(0, 1) < 0.3);
//                        long targetDB = usePrimary ? dbPrimaryIP : dbSecondaryIP;
//                        int targetAddr = usePrimary ? 20 : 21;
//
//                        if (targetDB == 0) {
//                            targetDB = dbPrimaryIP;
//                            targetAddr = 20;
//                        }
//
//                        // Account ID = address offset (111 → 1001, 211 → 1011, etc.)
//                        long accountId = 1000 + (address % 100);
//
//                        // 80% READ, 20% WRITE
//                        bool isWrite = (uniform(0, 1) < 0.2);
//
//                        if (isWrite && !usePrimary) {
//                            // Force write to primary
//                            targetDB = dbPrimaryIP;
//                            targetAddr = 20;
//                        }
//
//                        if (isWrite) {
//                            EV << "\n========================================\n";
//                            EV << "→ PC " << address << ": DB WRITE [via TCP]\n";
//                            EV << "========================================\n";
//                            EV << "Target: PRIMARY DB\n";
//                            EV << "Account ID: " << accountId << "\n";
//                            EV << "Using: tcpOut gate\n";
//                            EV << "========================================\n\n";
//
//                            auto *writeQuery = mk("DB_WRITE", DB_QUERY_WRITE, address, targetAddr);
//                            writeQuery->par("srcIP") = myIP;
//                            writeQuery->par("dstIP") = targetDB;
//                            writeQuery->par("accountId") = accountId;
//                            writeQuery->par("amount") = intuniform(1000, 5000);
//                            writeQuery->par("queryType") = "WITHDRAW";
//                            send(writeQuery, "tcpOut");  // ✅ Changed to TCP
//                        } else {
//                            EV << "\n========================================\n";
//                            EV << "→ PC " << address << ": DB READ [via TCP]\n";
//                            EV << "========================================\n";
//                            EV << "Target: " << (usePrimary ? "PRIMARY" : "SECONDARY") << " DB\n";
//                            EV << "Account ID: " << accountId << "\n";
//                            EV << "Using: tcpOut gate\n";
//                            EV << "========================================\n\n";
//
//                            auto *readQuery = mk("DB_READ", DB_QUERY_READ, address, targetAddr);
//                            readQuery->par("srcIP") = myIP;
//                            readQuery->par("dstIP") = targetDB;
//                            readQuery->par("accountId") = accountId;
//                            send(readQuery, "tcpOut");  // ✅ Changed to TCP
//                        }
//
//                        dbQueries++;
//                        appState = 2;
//                    }
//                }
//                else if (appState == 2) {
//                    // HTTP GET - USE TCP
//                    EV << "\n========================================\n";
//                    EV << "→ PC " << address << ": HTTP GET [via TCP]\n";
//                    EV << "========================================\n";
//                    EV << "Target: web.server.com\n";
//                    EV << "Request: GET /index.html\n";
//                    EV << "Using: tcpOut gate\n";
//                    EV << "========================================\n\n";
//
//                    auto *httpGet = mk("HTTP_GET", HTTP_GET, address, 13);
//                    httpGet->par("srcIP") = myIP;
//                    httpGet->par("payload") = "GET /index.html";
//                    send(httpGet, "tcpOut");  // ✅ Changed to TCP
//                    httpRequests++;
//                    appState = 3;
//                }
//                else if (appState == 3) {
//                    // SMTP Send - USE TCP
//                    EV << "\n========================================\n";
//                    EV << "→ PC " << address << ": SMTP SEND [via TCP]\n";
//                    EV << "========================================\n";
//                    EV << "Target: mail.server.com\n";
//                    EV << "Using: tcpOut gate\n";
//                    EV << "========================================\n\n";
//
//                    auto *email = mk("SMTP_SEND", SMTP_SEND, address, 12);
//                    email->par("srcIP") = myIP;
//                    email->par("payload") = "Email from PC";
//                    send(email, "tcpOut");  // ✅ Changed to TCP
//                    emailsSent++;
//                    appState = 0;  // Cycle back
//                }
//
//                // Reschedule (reduced interval for more traffic)
//                scheduleAt(simTime() + uniform(8.0, 15.0), msg);
//            }
//        } else {
//            int kind = msg->getKind();
//
//            if (kind == DHCP_OFFER && dhcpState == 1) {
//                long offeredIP = OFFERED_IP(msg);
//                dhcpState = 2;
//
//                EV << "← PC " << address << ": DHCP OFFER received - " << ipToStr(offeredIP) << "\n";
//
//                auto *request = mk("DHCP_REQUEST", DHCP_REQUEST, address, 10);
//                request->par("offeredIP") = offeredIP;
//                request->par("vlanId") = vlanId;
//                send(request, "tcpOut");  // DHCP uses PPP directly
//            }
//            else if (kind == DHCP_ACK && dhcpState == 2) {
//                myIP = OFFERED_IP(msg);
//                ipConfigured = true;
//                dhcpState = 3;
//
//                EV << "========================================\n";
//                EV << "✓ PC " << address << ": IP CONFIGURED\n";
//                EV << "========================================\n";
//                EV << "IP Address: " << ipToStr(myIP) << "\n";
//                EV << "Ready for application traffic\n";
//                EV << "========================================\n\n";
//
//                if (!appTestTimer) {
//                    appTestTimer = new cMessage("appTest");
//                    scheduleAt(simTime() + uniform(3.0, 6.0), appTestTimer);
//                }
//            }
//            else if (kind == DNS_RESPONSE) {
//                std::string name = REQUESTED_NAME(msg);
//                long resolvedIP = OFFERED_IP(msg);
//
//                if (resolvedIP != 0) {
//                    dnsCache[name] = resolvedIP;
//
//                    EV << "✓ PC " << address << ": DNS RESPONSE - " << name
//                       << " → " << ipToStr(resolvedIP) << "\n";
//
//                    if (name == "db-primary.server.com") {
//                        dbPrimaryIP = resolvedIP;
//
//                        // Query secondary too - USE TCP
//                        auto *query2 = mk("DNS_QUERY", DNS_QUERY, address, 11);
//                        query2->par("srcIP") = myIP;
//                        query2->par("requestedName") = "db-secondary.server.com";
//                        send(query2, "tcpOut");  // ✅ Changed to TCP
//                    } else if (name == "db-secondary.server.com") {
//                        dbSecondaryIP = resolvedIP;
//                    }
//                }
//            }
//            else if (kind == DB_RESPONSE_SUCCESS) {
//                long accountId = ACCOUNT_ID(msg);
//                long balance = AMOUNT(msg);
//                EV << "✓ PC " << address << ": DB RESPONSE - Account " << accountId
//                   << " balance: " << balance << " BDT\n";
//            }
//            else if (kind == DB_RESPONSE_ERROR) {
//                std::string error = PAYLOAD(msg);
//                EV << "✗ PC " << address << ": DB ERROR - " << error << "\n";
//            }
//            else if (kind == HTTP_RESPONSE) {
//                EV << "✓ PC " << address << ": HTTP RESPONSE received\n";
//            }
//            else if (kind == SMTP_ACK) {
//                EV << "✓ PC " << address << ": Email sent successfully\n";
//            }
//
//            delete msg;
//        }
//    }
//
//    void finish() override {
//        EV << "\n========================================\n";
//        EV << "💻 PC " << address << " FINAL STATISTICS\n";
//        EV << "========================================\n";
//        EV << "Module: " << getFullPath() << "\n";
//        EV << "IP Address: " << ipToStr(myIP) << "\n";
//        EV << "DNS Queries: " << dnsQueries << "\n";
//        EV << "DB Queries: " << dbQueries << "\n";
//        EV << "HTTP Requests: " << httpRequests << "\n";
//        EV << "Emails Sent: " << emailsSent << "\n";
//        EV << "========================================\n\n";
//
//        // Record statistics
//        recordScalar("DNS Queries", dnsQueries);
//        recordScalar("DB Queries", dbQueries);
//        recordScalar("HTTP Requests", httpRequests);
//        recordScalar("Emails Sent", emailsSent);
//
//        if (appTestTimer && appTestTimer->isScheduled()) {
//            cancelAndDelete(appTestTimer);
//        }
//    }
//};
//
//Define_Module(PCApp);
//
//
//



//Claude

#include <omnetpp.h>
#include "helpers.h"
#include <map>

using namespace omnetpp;

class PCApp : public cSimpleModule {
private:
    int address;
    int vlanId;
    long myIP = 0;
    bool ipConfigured = false;

    // === DHCP স্টেট ===
    int dhcpState = 0;  // 0:শুরু, 1:ডিসকভার পাঠিয়েছে, 2:অফার পেয়েছে, 3:সম্পন্ন

    // === অ্যাপ্লিকেশন স্টেট ===
    int appState = 0;   // 0:DNS, 1:DB, 2:HTTP, 3:SMTP
    cMessage *appTestTimer = nullptr;

    // === ক্যাশ এবং কনফিগারেশন ===
    std::map<std::string, long> dnsCache;
    long dbPrimaryIP = 0;
    long dbSecondaryIP = 0;

    // === স্ট্যাটিস্টিকস ===
    int dnsQueries = 0;
    int httpRequests = 0;
    int emailsSent = 0;
    int dbQueries = 0;

protected:
    void initialize() override {
        address = par("address");
        vlanId = par("vlanId").intValue();

        EV << "========================================\n";
        EV << "💻 PC MODULE INITIALIZED\n";
        EV << "========================================\n";
        EV << "Address: " << address << "\n";
        EV << "VLAN: " << vlanId << "\n";
        EV << "========================================\n\n";

        // DHCP শুরু করুন
        scheduleAt(simTime() + uniform(0.1, 0.5), new cMessage("startDHCP"));
    }

    void handleMessage(cMessage *msg) override {
        if (msg->isSelfMessage()) {
            if (strcmp(msg->getName(), "startDHCP") == 0) {
                // === DHCP DISCOVER পাঠান ===
                dhcpState = 1;
                EV << "→ PC " << address << ": DHCP DISCOVER শুরু করছি\n";

                auto *discover = mk("DHCP_DISCOVER", DHCP_DISCOVER, address, 10);
                discover->par("vlanId") = vlanId;
                send(discover, "tcpOut");  // TCP দিয়ে DHCP পাঠান
                delete msg;
            }
            else if (strcmp(msg->getName(), "appTest") == 0) {
                // === অ্যাপ্লিকেশন ট্রাফিক ===

                if (!ipConfigured) {
                    // IP এখনো বরাদ্দ হয়নি
                    scheduleAt(simTime() + 1.0, msg);
                    return;
                }

                if (appState == 0) {
                    // === DNS QUERY ===
                    EV << "\n========================================\n";
                    EV << "→ PC " << address << ": DNS QUERY [via TCP]\n";
                    EV << "========================================\n";
                    EV << "Target: db-primary.server.com\n";
                    EV << "========================================\n\n";

                    auto *query = mk("DNS_QUERY", DNS_QUERY, address, 11);
                    query->par("srcIP") = myIP;
                    query->par("requestedName") = "db-primary.server.com";
                    send(query, "tcpOut");
                    dnsQueries++;
                    appState = 1;
                }
                else if (appState == 1) {
                    // === DATABASE QUERY ===
                    if (dbPrimaryIP == 0 && dbSecondaryIP == 0) {
                        EV << "⚠️  PC " << address << ": DB সার্ভার IP এখনো পাওয়া যায়নি\n";
                        appState = 2;
                    } else {
                        bool usePrimary = (uniform(0, 1) < 0.3);
                        long targetDB = usePrimary ? dbPrimaryIP : dbSecondaryIP;
                        int targetAddr = usePrimary ? 20 : 21;

                        long accountId = 1000 + (address % 100);
                        bool isWrite = (uniform(0, 1) < 0.2);

                        if (isWrite && !usePrimary) {
                            targetDB = dbPrimaryIP;
                            targetAddr = 20;
                        }

                        if (isWrite) {
                            EV << "\n========================================\n";
                            EV << "→ PC " << address << ": DB WRITE [via TCP]\n";
                            EV << "========================================\n";
                            EV << "Target: PRIMARY DB\n";
                            EV << "Account ID: " << accountId << "\n";
                            EV << "========================================\n\n";

                            auto *writeQuery = mk("DB_WRITE", DB_QUERY_WRITE, address, targetAddr);
                            writeQuery->par("srcIP") = myIP;
                            writeQuery->par("dstIP") = targetDB;
                            writeQuery->par("accountId") = accountId;
                            writeQuery->par("amount") = intuniform(1000, 5000);
                            writeQuery->par("queryType") = "WITHDRAW";
                            send(writeQuery, "tcpOut");
                        } else {
                            EV << "\n========================================\n";
                            EV << "→ PC " << address << ": DB READ [via TCP]\n";
                            EV << "========================================\n";
                            EV << "Target: " << (usePrimary ? "PRIMARY" : "SECONDARY") << " DB\n";
                            EV << "Account ID: " << accountId << "\n";
                            EV << "========================================\n\n";

                            auto *readQuery = mk("DB_READ", DB_QUERY_READ, address, targetAddr);
                            readQuery->par("srcIP") = myIP;
                            readQuery->par("dstIP") = targetDB;
                            readQuery->par("accountId") = accountId;
                            send(readQuery, "tcpOut");
                        }
                        dbQueries++;
                        appState = 2;
                    }
                }
                else if (appState == 2) {
                    // === HTTP REQUEST ===
                    EV << "\n========================================\n";
                    EV << "→ PC " << address << ": HTTP GET [via TCP]\n";
                    EV << "========================================\n";
                    EV << "Target: web.server.com\n";
                    EV << "========================================\n\n";

                    auto *httpGet = mk("HTTP_GET", HTTP_GET, address, 13);
                    httpGet->par("srcIP") = myIP;
                    httpGet->par("payload") = "GET /index.html";
                    send(httpGet, "tcpOut");
                    httpRequests++;
                    appState = 3;
                }
                else if (appState == 3) {
                    // === SMTP EMAIL ===
                    EV << "\n========================================\n";
                    EV << "→ PC " << address << ": SMTP SEND [via TCP]\n";
                    EV << "========================================\n";
                    EV << "Target: mail.server.com\n";
                    EV << "========================================\n\n";

                    auto *email = mk("SMTP_SEND", SMTP_SEND, address, 12);
                    email->par("srcIP") = myIP;
                    email->par("payload") = "Email from PC";
                    send(email, "tcpOut");
                    emailsSent++;
                    appState = 0;  // সাইকেল শুরু
                }

                // পরবর্তী অ্যাপ্লিকেশন চেষ্টা
                scheduleAt(simTime() + uniform(8.0, 15.0), msg);
            }
        }
        else {
            // === নেটওয়ার্ক থেকে বার্তা পান ===
            int kind = msg->getKind();

            if (kind == DHCP_OFFER && dhcpState == 1) {
                // === DHCP OFFER পাওয়া ===
                long offeredIP = OFFERED_IP(msg);
                dhcpState = 2;

                EV << "← PC " << address << ": DHCP OFFER - " << ipToStr(offeredIP) << "\n";

                auto *request = mk("DHCP_REQUEST", DHCP_REQUEST, address, 10);
                request->par("offeredIP") = offeredIP;
                request->par("vlanId") = vlanId;
                send(request, "tcpOut");
            }
            else if (kind == DHCP_ACK && dhcpState == 2) {
                // === DHCP ACK পাওয়া - IP কনফিগার ===
                myIP = OFFERED_IP(msg);
                ipConfigured = true;
                dhcpState = 3;

                EV << "========================================\n";
                EV << "✓ PC " << address << ": IP CONFIGURED\n";
                EV << "========================================\n";
                EV << "IP Address: " << ipToStr(myIP) << "\n";
                EV << "Ready for application traffic\n";
                EV << "========================================\n\n";

                // অ্যাপ্লিকেশন ট্রাফিক শুরু করুন
                if (!appTestTimer) {
                    appTestTimer = new cMessage("appTest");
                    scheduleAt(simTime() + uniform(3.0, 6.0), appTestTimer);
                }
            }
            else if (kind == DNS_RESPONSE) {
                // === DNS রেসপন্স ===
                std::string name = REQUESTED_NAME(msg);
                long resolvedIP = OFFERED_IP(msg);

                if (resolvedIP != 0) {
                    dnsCache[name] = resolvedIP;
                    EV << "✓ PC " << address << ": DNS RESPONSE - " << name
                       << " → " << ipToStr(resolvedIP) << "\n";

                    if (name == "db-primary.server.com") {
                        dbPrimaryIP = resolvedIP;
                        // সেকেন্ডারি DNS ক্যোয়ারি
                        auto *query2 = mk("DNS_QUERY", DNS_QUERY, address, 11);
                        query2->par("srcIP") = myIP;
                        query2->par("requestedName") = "db-secondary.server.com";
                        send(query2, "tcpOut");
                    }
                    else if (name == "db-secondary.server.com") {
                        dbSecondaryIP = resolvedIP;
                    }
                }
            }
            else if (kind == DB_RESPONSE_SUCCESS) {
                // === DB সফল রেসপন্স ===
                long accountId = ACCOUNT_ID(msg);
                long balance = AMOUNT(msg);
                EV << "✓ PC " << address << ": DB RESPONSE - Account " << accountId
                   << " balance: " << balance << " BDT\n";
            }
            else if (kind == DB_RESPONSE_ERROR) {
                // === DB এরর ===
                std::string error = PAYLOAD(msg);
                EV << "✗ PC " << address << ": DB ERROR - " << error << "\n";
            }
            else if (kind == HTTP_RESPONSE) {
                // === HTTP রেসপন্স ===
                EV << "✓ PC " << address << ": HTTP RESPONSE received\n";
            }
            else if (kind == SMTP_ACK) {
                // === SMTP এক্সেপ্টেড ===
                EV << "✓ PC " << address << ": Email sent successfully\n";
            }

            delete msg;
        }
    }

    void finish() override {
        EV << "\n========================================\n";
        EV << "💻 PC " << address << " FINAL STATISTICS\n";
        EV << "========================================\n";
        EV << "IP Address: " << ipToStr(myIP) << "\n";
        EV << "DNS Queries: " << dnsQueries << "\n";
        EV << "DB Queries: " << dbQueries << "\n";
        EV << "HTTP Requests: " << httpRequests << "\n";
        EV << "Emails Sent: " << emailsSent << "\n";
        EV << "========================================\n\n";

        recordScalar("DNS Queries", dnsQueries);
        recordScalar("DB Queries", dbQueries);
        recordScalar("HTTP Requests", httpRequests);
        recordScalar("Emails Sent", emailsSent);

        if (appTestTimer && appTestTimer->isScheduled()) {
            cancelAndDelete(appTestTimer);
        }
    }
};

Define_Module(PCApp);

