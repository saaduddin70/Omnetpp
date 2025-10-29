//
// Mail Server Module with TCP integration
//
package zpractive.Modules;

module MailServer
{
    parameters:
        int address;
        @display("i=device/server");

    gates:
        inout ppp @labels(PPPFrame);

    submodules:
        app: MailApp {
            address = parent.address;
        }
        tcp: TCP;

    connections:
        app.tcpOut --> tcp.fromApp;
        app.tcpIn <-- tcp.toApp;
        tcp.toNet --> ppp$o;
        tcp.fromNet <-- ppp$i;
}