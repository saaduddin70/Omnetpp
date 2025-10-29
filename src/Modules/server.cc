#include <omnetpp.h>
#include "helpers.h"

using namespace omnetpp;

class Server : public cSimpleModule {
private:
    int address;
    std::string serviceName;
    int requestsServed = 0;

protected:
    void initialize() override {
        address = par("address");
        serviceName = par("serviceName").stdstringValue();

        EV << "========================================\n";
        EV << serviceName << " Server initialized\n";
        EV << "Address: " << address << "\n";
        EV << "========================================\n";
    }

    void handleMessage(cMessage *msg) override {
        long clientAddr = SRC(msg);
        requestsServed++;

        EV << serviceName << " Server: Request from " << clientAddr << "\n";

        // Simple echo response
        auto *reply = mk("REPLY", PONG, address, clientAddr);
        sendDelayed(reply, 0.01, "ppp$o");

        EV << serviceName << " Server: Response sent to " << clientAddr << "\n";

        delete msg;
    }

    void finish() override {
        EV << serviceName << " Server Statistics:\n";
        EV << "  Requests served: " << requestsServed << "\n";
        recordScalar("requestsServed", requestsServed);
    }
};

Define_Module(Server);
