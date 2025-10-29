//#include <omnetpp.h>
//#include "helpers.h"
//#include <map>
//
//using namespace omnetpp;
//
//class DNSApp : public cSimpleModule {
//private:
//    int address;
//    long myIP;
//    std::map<std::string, long> dnsTable;
//    int queriesReceived = 0;
//    int responsesSet = 0;
//
//protected:
//    void initialize() override {
//        address = par("address");
//        myIP = makeIP(192, 168, 0, 11);
//
//        // Build DNS table with ALL servers
//        dnsTable["mail.server.com"] = makeIP(192, 168, 0, 12);
//        dnsTable["web.server.com"] = makeIP(192, 168, 0, 13);
//        dnsTable["dhcp.server.com"] = makeIP(192, 168, 0, 10);
//        dnsTable["dns.server.com"] = myIP;
//
//        // DATABASE CLUSTER (NEW!)
//        dnsTable["db-primary.server.com"] = makeIP(192, 168, 0, 20);
//        dnsTable["db-secondary.server.com"] = makeIP(192, 168, 0, 21);
//        dnsTable["db-backup.server.com"] = makeIP(192, 168, 0, 22);
//
//        EV << "========================================\n";
//        EV << "🌐 DNS SERVER INITIALIZED\n";
//        EV << "========================================\n";
//        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
//        EV << "DNS Records:\n";
//        for (auto &entry : dnsTable) {
//            EV << "  " << entry.first << " → " << ipToStr(entry.second) << "\n";
//        }
//        EV << "========================================\n\n";
//    }
//
//    void handleMessage(cMessage *msg) override {
//        int kind = msg->getKind();
//        long clientAddr = SRC(msg);
//        long clientIP = SRCIP(msg);
//
//        if (kind == DNS_QUERY) {
//            queriesReceived++;
//            std::string requestedName = REQUESTED_NAME(msg);
//
//            EV << "🔍 DNS: Query from client " << clientAddr
//               << " for \"" << requestedName << "\"\n";
//
//            // Lookup in DNS table
//            auto it = dnsTable.find(requestedName);
//            long resolvedIP = 0;
//
//            if (it != dnsTable.end()) {
//                resolvedIP = it->second;
//                EV << "   ✓ Resolved: \"" << requestedName << "\" → "
//                   << ipToStr(resolvedIP) << "\n";
//            } else {
//                EV << "   ✗ Name \"" << requestedName << "\" not found\n";
//            }
//
//            // Send DNS response
//            auto *response = mk("DNS_RESPONSE", DNS_RESPONSE, address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("offeredIP") = resolvedIP;
//            response->par("requestedName") = requestedName.c_str();
//
//            sendDelayed(response, 0.005, "ppp$o");
//            responsesSet++;
//        }
//        else if (kind == PING) {
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
//        EV << "========================================\n";
//        EV << "DNS Server Statistics:\n";
//        EV << "  Queries received: " << queriesReceived << "\n";
//        EV << "  Responses sent: " << responsesSet << "\n";
//        EV << "========================================\n";
//
//        recordScalar("queriesReceived", queriesReceived);
//        recordScalar("responsesSent", responsesSet);
//    }
//};
//
//Define_Module(DNSApp);


//Claude

#include <omnetpp.h>
#include "helpers.h"
#include <map>

using namespace omnetpp;

class DNSApp : public cSimpleModule {
private:
    int address;
    long myIP;
    std::map<std::string, long> dnsTable;
    int queriesReceived = 0;
    int responsesSet = 0;

protected:
    void initialize() override {
        address = par("address");
        myIP = makeIP(192, 168, 0, 11);

        // Build DNS table with ALL servers
        dnsTable["mail.server.com"] = makeIP(192, 168, 0, 12);
        dnsTable["web.server.com"] = makeIP(192, 168, 0, 13);
        dnsTable["dhcp.server.com"] = makeIP(192, 168, 0, 10);
        dnsTable["dns.server.com"] = myIP;

        // DATABASE CLUSTER
        dnsTable["db-primary.server.com"] = makeIP(192, 168, 0, 20);
        dnsTable["db-secondary.server.com"] = makeIP(192, 168, 0, 21);
        dnsTable["db-backup.server.com"] = makeIP(192, 168, 0, 22);

        EV << "========================================\n";
        EV << "🌐 DNS SERVER INITIALIZED\n";
        EV << "========================================\n";
        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
        EV << "DNS Records:\n";
        for (auto &entry : dnsTable) {
            EV << "  " << entry.first << " → " << ipToStr(entry.second) << "\n";
        }
        EV << "========================================\n\n";
    }

    void handleMessage(cMessage *msg) override {
        int kind = msg->getKind();
        long clientAddr = SRC(msg);
        long clientIP = SRCIP(msg);

        if (kind == DNS_QUERY) {
            queriesReceived++;
            std::string requestedName = REQUESTED_NAME(msg);

            EV << "🔍 DNS: Query from client " << clientAddr
               << " for \"" << requestedName << "\"\n";

            // Lookup in DNS table
            auto it = dnsTable.find(requestedName);
            long resolvedIP = 0;

            if (it != dnsTable.end()) {
                resolvedIP = it->second;
                EV << "  ✓ Resolved: \"" << requestedName << "\" → "
                   << ipToStr(resolvedIP) << "\n";
            } else {
                EV << "  ✗ Name \"" << requestedName << "\" not found\n";
            }

            // Send DNS response via TCP (not PPP directly!)
            auto *response = mk("DNS_RESPONSE", DNS_RESPONSE, address, clientAddr);
            response->par("srcIP") = myIP;
            response->par("dstIP") = clientIP;
            response->par("offeredIP") = resolvedIP;
            response->par("requestedName") = requestedName.c_str();

            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            send(response, "tcpOut");
            responsesSet++;
        }
        else if (kind == PING) {
            auto *reply = mk("PONG", PONG, address, clientAddr);
            reply->par("srcIP") = myIP;
            reply->par("dstIP") = clientIP;

            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            send(reply, "tcpOut");
        }

        delete msg;
    }

    void finish() override {
        EV << "========================================\n";
        EV << "DNS Server Statistics:\n";
        EV << "  Queries received: " << queriesReceived << "\n";
        EV << "  Responses sent: " << responsesSet << "\n";
        EV << "========================================\n";

        recordScalar("queriesReceived", queriesReceived);
        recordScalar("responsesSent", responsesSet);
    }
};

Define_Module(DNSApp);
