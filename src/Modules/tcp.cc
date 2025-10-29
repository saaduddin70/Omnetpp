#include <omnetpp.h>
using namespace omnetpp;

class TCP : public cSimpleModule
{
  private:
    int packetsReceived = 0;
    int packetsSent = 0;
    simtime_t totalDelay;

  protected:
    virtual void initialize() override {
        packetsReceived = 0;
        packetsSent = 0;
        totalDelay = 0;

        EV << "========================================\n";
        EV << "🔷 TCP MODULE INITIALIZED\n";
        EV << "========================================\n";
        EV << "Module: " << getFullPath() << "\n";
        EV << "========================================\n\n";
    }

    virtual void handleMessage(cMessage *msg) override {
        packetsReceived++;
        simtime_t arrivalTime = simTime();

        // Log detailed info
        EV << "========================================\n";
        EV << "🔷 TCP EVENT #" << packetsReceived << "\n";
        EV << "========================================\n";
        EV << "Time: " << simTime() << "s\n";
        EV << "Module: " << getFullPath() << "\n";
        EV << "Message: " << msg->getName() << "\n";
        EV << "Arrival Gate: " << msg->getArrivalGate()->getName() << "\n";

        if (msg->arrivedOn("fromApp")) {
            EV << "Direction: Application → Network\n";
            EV << "Action: Forwarding to network layer\n";
            EV << "========================================\n\n";

            packetsSent++;
            send(msg, "toNet");

        } else if (msg->arrivedOn("fromNet")) {
            EV << "Direction: Network → Application\n";
            EV << "Action: Forwarding to application\n";
            EV << "========================================\n\n";

            packetsSent++;
            send(msg, "toApp");

        } else {
            EV << "Direction: Unknown\n";
            EV << "Action: Deleting message\n";
            EV << "========================================\n\n";
            delete msg;
        }
    }

    virtual void finish() override {
        EV << "========================================\n";
        EV << "🔷 TCP STATISTICS\n";
        EV << "========================================\n";
        EV << "Module: " << getFullPath() << "\n";
        EV << "Packets Received: " << packetsReceived << "\n";
        EV << "Packets Sent: " << packetsSent << "\n";
        EV << "========================================\n\n";

        // Record statistics
        recordScalar("Packets Received", packetsReceived);
        recordScalar("Packets Sent", packetsSent);
    }
};

Define_Module(TCP);
