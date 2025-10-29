//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package zpractive.Modules;

module DatabaseServer
{
    parameters:
        int address;
        string role = default("primary");
        @display("i=device/server");

    gates:
        inout ppp @labels(PPPFrame);

    submodules:
        app: DatabaseApp {
            address = parent.address;
            role = parent.role;
        }
        tcp: TCP;

    connections:
        app.tcpOut --> tcp.fromApp;
        app.tcpIn <-- tcp.toApp;
        tcp.toNet --> ppp$o;
        tcp.fromNet <-- ppp$i;
}