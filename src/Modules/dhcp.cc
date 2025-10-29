#include <omnetpp.h>
#include "helpers.h"
#include <map>

using namespace omnetpp;

class DHCP : public cSimpleModule {
private:
    int address;
    std::map<int, long> ipPool;  // VLAN -> next available IP
    std::map<long, long> leases; // Device address -> assigned IP
    int discoversReceived = 0;
    int offersSent = 0;           // FIXED: Changed from 'offerssent' to 'offersSent'
    int requestsReceived = 0;
    int acksSent = 0;             // FIXED: Changed from 'acksSet' to 'acksSent'

protected:
    void initialize() override {
        address = par("address");

        // Initialize IP pools for each VLAN
        ipPool[10] = makeIP(192, 168, 10, 100);  // VLAN 10: 192.168.10.100
        ipPool[20] = makeIP(192, 168, 20, 100);  // VLAN 20: 192.168.20.100
        ipPool[30] = makeIP(192, 168, 30, 100);  // VLAN 30: 192.168.30.100

        EV << "========================================\n";
        EV << "DHCP Server initialized\n";
        EV << "Address: " << address << "\n";
        EV << "IP Pools:\n";
        EV << "  VLAN 10: " << ipToStr(ipPool[10]) << " - .150\n";
        EV << "  VLAN 20: " << ipToStr(ipPool[20]) << " - .150\n";
        EV << "  VLAN 30: " << ipToStr(ipPool[30]) << " - .150\n";
        EV << "========================================\n";
    }

    void handleMessage(cMessage *msg) override {
        int kind = msg->getKind();
        long clientAddr = SRC(msg);
        int clientVlan = VLAN(msg);

        if (kind == DHCP_DISCOVER) {
            discoversReceived++;

            EV << "DHCP: DISCOVER from client " << clientAddr
               << " (VLAN " << clientVlan << ")\n";

            // Assign IP from appropriate pool
            long offeredIP = 0;

            if (ipPool.find(clientVlan) != ipPool.end()) {
                offeredIP = ipPool[clientVlan];
                ipPool[clientVlan]++;  // Increment for next client

                EV << "DHCP: Offering IP " << ipToStr(offeredIP)
                   << " to client " << clientAddr << "\n";

                auto *offer = mk("DHCP_OFFER", DHCP_OFFER, address, clientAddr);
                offer->par("offeredIP") = offeredIP;
                offer->par("vlanId") = clientVlan;

                sendDelayed(offer, 0.01, "ppp$o");
                offersSent++;  // FIXED: Changed from 'offersSet++'
            } else {
                EV << "DHCP: No IP pool for VLAN " << clientVlan << "\n";
            }

            delete msg;
        }
        else if (kind == DHCP_REQUEST) {
            requestsReceived++;
            long requestedIP = OFFERED_IP(msg);

            EV << "DHCP: REQUEST from client " << clientAddr
               << " for IP " << ipToStr(requestedIP) << "\n";

            // Confirm assignment
            leases[clientAddr] = requestedIP;

            auto *ack = mk("DHCP_ACK", DHCP_ACK, address, clientAddr);
            ack->par("offeredIP") = requestedIP;
            ack->par("vlanId") = clientVlan;

            sendDelayed(ack, 0.01, "ppp$o");
            acksSent++;  // FIXED: Changed from 'acksSet++'

            EV << "✓ DHCP: IP " << ipToStr(requestedIP)
               << " assigned to client " << clientAddr << "\n";

            delete msg;
        }
        else {
            // Handle regular ping (backward compatibility)
            auto *reply = mk("REPLY", PONG, address, clientAddr);
            sendDelayed(reply, 0.01, "ppp$o");
            delete msg;
        }
    }

    void finish() override {
        EV << "========================================\n";
        EV << "DHCP Server Statistics:\n";
        EV << "  DISCOVER received: " << discoversReceived << "\n";
        EV << "  OFFER sent: " << offersSent << "\n";       // FIXED
        EV << "  REQUEST received: " << requestsReceived << "\n";
        EV << "  ACK sent: " << acksSent << "\n";           // FIXED
        EV << "  Active leases: " << leases.size() << "\n";
        EV << "========================================\n";

        recordScalar("discoversReceived", discoversReceived);
        recordScalar("offersSent", offersSent);             // FIXED
        recordScalar("requestsReceived", requestsReceived);
        recordScalar("acksSent", acksSent);                 // FIXED
        recordScalar("activeLeases", leases.size());
    }
};

Define_Module(DHCP);
