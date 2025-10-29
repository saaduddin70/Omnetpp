#include <omnetpp.h>
#include "helpers.h"
#include <map>

using namespace omnetpp;

class Router : public cSimpleModule {
private:
    int address;
    std::map<int, int> routingTable;
    int packetsRouted = 0;

protected:
    void initialize() override {
        address = par("address");

        EV << "========================================\n";
        EV << "🔀 ROUTER " << address << " INITIALIZED\n";
        EV << "========================================\n";
        EV << "Total gates: " << gateSize("ppp") << "\n";
        EV << "========================================\n\n";
    }

    void handleMessage(cMessage *msg) override {
        int arrivalGate = msg->getArrivalGate()->getIndex();
        int kind = msg->getKind();
        int dest = DST(msg);  // ✓ FIXED: Changed from DEST to DST

        // Determine output gate
        int outGate = -1;

        if (address == 1) {
            // Core router routing
            if (dest == 10) outGate = 0;      // DHCP
            else if (dest == 11) outGate = 1; // DNS
            else if (dest == 12) outGate = 2; // Mail
            else if (dest == 13) outGate = 3; // Web
            else if (dest == 20) outGate = 4; // DB Primary
            else if (dest == 21) outGate = 5; // DB Secondary
            else if (dest == 22) outGate = 6; // DB Backup
            else if (dest >= 100 && dest < 200) outGate = 7;  // Branch A
            else if (dest >= 200 && dest < 300) outGate = 8;  // Branch B
            else if (dest >= 300 && dest < 400) outGate = 9;  // Branch C
        }
        else if (address == 100) {
            // Branch A router
            if (dest >= 111 && dest <= 118) outGate = 1;  // To switch
            else outGate = 0;  // To core
        }
        else if (address == 200) {
            // Branch B router
            if (dest >= 211 && dest <= 218) outGate = 1;  // To switch
            else outGate = 0;  // To core
        }
        else if (address == 300) {
            // Branch C router
            if (dest >= 311 && dest <= 318) outGate = 1;  // To switch
            else outGate = 0;  // To core
        }

        if (outGate >= 0 && outGate < gateSize("ppp")) {
            send(msg, "ppp$o", outGate);
            packetsRouted++;
        } else {
            EV << "❌ Router " << address << ": No route for dest " << dest << "\n";
            delete msg;
        }
    }

    void finish() override {
        EV << "========================================\n";
        EV << "Router " << address << " Statistics:\n";
        EV << "  Total packets routed: " << packetsRouted << "\n";
        EV << "========================================\n";

        recordScalar("packetsRouted", packetsRouted);
    }
};

Define_Module(Router);
