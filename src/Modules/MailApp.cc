///*
// * MailServer.cc
// *
// *  Created on: Oct 4, 2025
// *      Author: USER
// */
//#include <omnetpp.h>
//#include "helpers.h"
//#include <vector>
//
//using namespace omnetpp;
//
//class MailApp : public cSimpleModule {
//private:
//    int address;
//    long myIP;
//    std::vector<std::string> inbox;
//    int emailsSent = 0;
//    int emailsReceived = 0;
//    int pop3Fetches = 0;
//
//protected:
//    void initialize() override {
//        address = par("address");
//        myIP = makeIP(192, 168, 0, 12);  // Mail server IP
//
//        EV << "========================================\n";
//        EV << "Mail Server initialized\n";
//        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
//        EV << "========================================\n";
//    }
//
//    void handleMessage(cMessage *msg) override {
//        int kind = msg->getKind();
//        long clientAddr = SRC(msg);
//        long clientIP = SRCIP(msg);
//        std::string payload = PAYLOAD(msg);
//
//        if (kind == SMTP_SEND) {
//            emailsReceived++;
//
//            EV << "Mail Server: SMTP email from " << clientAddr
//               << " (IP: " << ipToStr(clientIP) << ")\n";
//            EV << "  Email: " << payload << "\n";
//
//            // Store in inbox
//            inbox.push_back(payload);
//
//            // Send SMTP ACK
//            auto *ack = mk("SMTP_ACK", SMTP_ACK, address, clientAddr);
//            ack->par("srcIP") = myIP;
//            ack->par("dstIP") = clientIP;
//            ack->par("payload") = "Email delivered";
//
//            sendDelayed(ack, 0.01, "ppp$o");
//
//            EV << "Mail Server: Email stored, ACK sent\n";
//        }
//        else if (kind == POP3_FETCH) {
//            pop3Fetches++;
//
//            EV << "Mail Server: POP3 fetch from " << clientAddr << "\n";
//
//            // Send inbox contents
//            std::string inboxData = "Inbox: " + std::to_string(inbox.size()) + " messages";
//
//            auto *response = mk("POP3_RESPONSE", POP3_RESPONSE, address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("payload") = inboxData.c_str();
//
//            sendDelayed(response, 0.01, "ppp$o");
//
//            EV << "Mail Server: Inbox sent (" << inbox.size() << " messages)\n";
//        }
//        else if (kind == PING) {
//            // Backward compatibility
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
//        EV << "Mail Server Statistics:\n";
//        EV << "  Emails received: " << emailsReceived << "\n";
//        EV << "  POP3 fetches: " << pop3Fetches << "\n";
//        EV << "  Inbox size: " << inbox.size() << "\n";
//        EV << "========================================\n";
//
//        recordScalar("emailsReceived", emailsReceived);
//        recordScalar("pop3Fetches", pop3Fetches);
//        recordScalar("inboxSize", inbox.size());
//    }
//};
//
//Define_Module(MailApp);
//
//
//
//Claude
// ============================================
// MailApp.cc
// ============================================
#include <omnetpp.h>
#include "helpers.h"
#include <vector>

using namespace omnetpp;

class MailApp : public cSimpleModule {
private:
    int address;
    long myIP;
    std::vector<std::string> inbox;
    int emailsSent = 0;
    int emailsReceived = 0;
    int pop3Fetches = 0;

protected:
    void initialize() override {
        address = par("address");
        myIP = makeIP(192, 168, 0, 12); // Mail server IP

        EV << "========================================\n";
        EV << "Mail Server initialized\n";
        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
        EV << "========================================\n";
    }

    void handleMessage(cMessage *msg) override {
        int kind = msg->getKind();
        long clientAddr = SRC(msg);
        long clientIP = SRCIP(msg);
        std::string payload = PAYLOAD(msg);

        if (kind == SMTP_SEND) {
            emailsReceived++;

            EV << "Mail Server: SMTP email from " << clientAddr
               << " (IP: " << ipToStr(clientIP) << ")\n";
            EV << "  Email: " << payload << "\n";

            // Store in inbox
            inbox.push_back(payload);

            // Send SMTP ACK via TCP (not PPP directly!)
            auto *ack = mk("SMTP_ACK", SMTP_ACK, address, clientAddr);
            ack->par("srcIP") = myIP;
            ack->par("dstIP") = clientIP;
            ack->par("payload") = "Email delivered";

            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(ack, 0.01, "tcpOut");

            EV << "Mail Server: Email stored, ACK sent\n";
        }
        else if (kind == POP3_FETCH) {
            pop3Fetches++;

            EV << "Mail Server: POP3 fetch from " << clientAddr << "\n";

            // Send inbox contents
            std::string inboxData = "Inbox: " + std::to_string(inbox.size()) + " messages";

            auto *response = mk("POP3_RESPONSE", POP3_RESPONSE, address, clientAddr);
            response->par("srcIP") = myIP;
            response->par("dstIP") = clientIP;
            response->par("payload") = inboxData.c_str();

            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(response, 0.01, "tcpOut");

            EV << "Mail Server: Inbox sent (" << inbox.size() << " messages)\n";
        }
        else if (kind == PING) {
            // Backward compatibility
            auto *reply = mk("PONG", PONG, address, clientAddr);
            reply->par("srcIP") = myIP;
            reply->par("dstIP") = clientIP;
            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(reply, 0.005, "tcpOut");
        }

        delete msg;
    }

    void finish() override {
        EV << "========================================\n";
        EV << "Mail Server Statistics:\n";
        EV << "  Emails received: " << emailsReceived << "\n";
        EV << "  POP3 fetches: " << pop3Fetches << "\n";
        EV << "  Inbox size: " << inbox.size() << "\n";
        EV << "========================================\n";

        recordScalar("emailsReceived", emailsReceived);
        recordScalar("pop3Fetches", pop3Fetches);
        recordScalar("inboxSize", inbox.size());
    }
};

Define_Module(MailApp);
