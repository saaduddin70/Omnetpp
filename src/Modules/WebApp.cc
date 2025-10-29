///*
// * WebServer.cc
// *
// *  Created on: Oct 4, 2025
// *      Author: USER
// */
//#include <omnetpp.h>
//#include "helpers.h"
//
//using namespace omnetpp;
//
//class WebApp : public cSimpleModule {
//private:
//    int address;
//    long myIP;
//    int getRequests = 0;
//    int postRequests = 0;
//    int responsesSet = 0;
//
//protected:
//    void initialize() override {
//        address = par("address");
//        myIP = makeIP(192, 168, 0, 13);  // Web server IP
//
//        EV << "========================================\n";
//        EV << "Web Server (Farm) initialized\n";
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
//        if (kind == HTTP_GET) {
//            getRequests++;
//
//            EV << "Web Server: GET request from " << clientAddr
//               << " (IP: " << ipToStr(clientIP) << ")\n";
//            EV << "  Request: " << payload << "\n";
//
//            // Send HTTP response
//            auto *response = mk("HTTP_RESPONSE", HTTP_RESPONSE, address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("payload") = "<html>Banking Portal</html>";
//
//            sendDelayed(response, 0.01, "ppp$o");
//            responsesSet++;
//
//            EV << "Web Server: Response sent to " << clientAddr << "\n";
//        }
//        else if (kind == HTTP_POST) {
//            postRequests++;
//
//            EV << "Web Server: POST request from " << clientAddr << "\n";
//            EV << "  Data: " << payload << "\n";
//
//            auto *response = mk("HTTP_RESPONSE", HTTP_RESPONSE, address, clientAddr);
//            response->par("srcIP") = myIP;
//            response->par("dstIP") = clientIP;
//            response->par("payload") = "POST received";
//
//            sendDelayed(response, 0.01, "ppp$o");
//            responsesSet++;
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
//        EV << "Web Server Statistics:\n";
//        EV << "  GET requests: " << getRequests << "\n";
//        EV << "  POST requests: " << postRequests << "\n";
//        EV << "  Responses sent: " << responsesSet << "\n";
//        EV << "========================================\n";
//
//        recordScalar("getRequests", getRequests);
//        recordScalar("postRequests", postRequests);
//        recordScalar("responsesSent", responsesSet);
//    }
//};
//
//Define_Module(WebApp);
//
//
//
//Claude
// ============================================
// WebApp.cc
// ============================================
#include <omnetpp.h>
#include "helpers.h"

using namespace omnetpp;

class WebApp : public cSimpleModule {
private:
    int address;
    long myIP;
    int getRequests = 0;
    int postRequests = 0;
    int responsesSet = 0;

protected:
    void initialize() override {
        address = par("address");
        myIP = makeIP(192, 168, 0, 13); // Web server IP

        EV << "========================================\n";
        EV << "Web Server (Farm) initialized\n";
        EV << "Address: " << address << " | IP: " << ipToStr(myIP) << "\n";
        EV << "========================================\n";
    }

    void handleMessage(cMessage *msg) override {
        int kind = msg->getKind();
        long clientAddr = SRC(msg);
        long clientIP = SRCIP(msg);
        std::string payload = PAYLOAD(msg);

        if (kind == HTTP_GET) {
            getRequests++;

            EV << "Web Server: GET request from " << clientAddr
               << " (IP: " << ipToStr(clientIP) << ")\n";
            EV << "  Request: " << payload << "\n";

            // Send HTTP response via TCP (not PPP directly!)
            auto *response = mk("HTTP_RESPONSE", HTTP_RESPONSE, address, clientAddr);
            response->par("srcIP") = myIP;
            response->par("dstIP") = clientIP;
            response->par("payload") = "<html>Banking Portal</html>";

            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(response, 0.01, "tcpOut");
            responsesSet++;

            EV << "Web Server: Response sent to " << clientAddr << "\n";
        }
        else if (kind == HTTP_POST) {
            postRequests++;

            EV << "Web Server: POST request from " << clientAddr << "\n";
            EV << "  Data: " << payload << "\n";

            auto *response = mk("HTTP_RESPONSE", HTTP_RESPONSE, address, clientAddr);
            response->par("srcIP") = myIP;
            response->par("dstIP") = clientIP;
            response->par("payload") = "POST received";

            // ✅ পরিবর্তন: tcpOut দিয়ে পাঠান
            sendDelayed(response, 0.01, "tcpOut");
            responsesSet++;
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
        EV << "Web Server Statistics:\n";
        EV << "  GET requests: " << getRequests << "\n";
        EV << "  POST requests: " << postRequests << "\n";
        EV << "  Responses sent: " << responsesSet << "\n";
        EV << "========================================\n";

        recordScalar("getRequests", getRequests);
        recordScalar("postRequests", postRequests);
        recordScalar("responsesSent", responsesSet);
    }
};

Define_Module(WebApp);
