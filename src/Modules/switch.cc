/*
 * switch.cc
 *
 *  Created on: Oct 3, 2025
 *      Author: USER
 */
#include <omnetpp.h>
#include "helpers.h"
#include <map>

using namespace omnetpp;

class Switch : public cSimpleModule {
private:
    int address;
    int vlanId;
    std::map<long, int> macTable; // MAC -> port
    int packetsForwarded = 0;
    int broadcastPackets = 0;

protected:
    void initialize() override {
        address = par("address");
        vlanId = par("vlanId").intValue();

        EV << "========================================\n";
        EV << "Switch Stack " << address << " initialized\n";
        EV << "VLAN: " << (vlanId > 0 ? std::to_string(vlanId) : "None") << "\n";
        EV << "Ports: " << gateSize("ppp") << "\n";
        EV << "========================================\n";
    }

    void handleMessage(cMessage *msg) override {
        long srcAddr = SRC(msg);
        long dstAddr = DST(msg);
        int arrivalPort = msg->getArrivalGate()->getIndex();

        // Learn source MAC
        if (macTable.find(srcAddr) == macTable.end()) {
            macTable[srcAddr] = arrivalPort;
            EV << "Switch " << address << ": Learned MAC " << srcAddr
               << " on port " << arrivalPort << "\n";
        }

        // Forward based on destination
        if (macTable.find(dstAddr) != macTable.end()) {
            // Unicast - known destination
            int outPort = macTable[dstAddr];
            if (outPort != arrivalPort) {
                EV << "Switch " << address << ": Unicast to port " << outPort << "\n";
                send(msg, "ppp$o", outPort);
                packetsForwarded++;
            } else {
                delete msg;
            }
        } else {
            // Flood - unknown destination
            EV << "Switch " << address << ": Flooding (unknown MAC " << dstAddr << ")\n";
            bool sent = false;
            for (int i = 0; i < gateSize("ppp$o"); i++) {
                if (i != arrivalPort) {
                    send(msg->dup(), "ppp$o", i);
                    sent = true;
                }
            }
            delete msg;
            if (sent) {
                packetsForwarded++;
                broadcastPackets++;
            }
        }
    }

    void finish() override {
        EV << "========================================\n";
        EV << "Switch " << address << " Statistics:\n";
        EV << "  MAC table entries: " << macTable.size() << "\n";
        EV << "  Packets forwarded: " << packetsForwarded << "\n";
        EV << "  Broadcast packets: " << broadcastPackets << "\n";
        EV << "========================================\n";

        recordScalar("macTableSize", macTable.size());
        recordScalar("packetsForwarded", packetsForwarded);
        recordScalar("broadcastPackets", broadcastPackets);
    }
};

Define_Module(Switch);
