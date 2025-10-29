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

//package simulations;
//
//import zpractive.Modules.Router;
//import zpractive.Modules.Switch;
//import zpractive.Modules.Server;
//import zpractive.Modules.PC;
//import zpractive.Modules.DHCP;
//import zpractive.Modules.DNSServer;
//import zpractive.Modules.MailServer;
//import zpractive.Modules.WebServer;
//import zpractive.Modules.DatabaseServer;
//
//
//network BankingNetwork {
//    submodules:
//        // HQ Core Infrastructure
//        coreRouter: Router {
//            address = 1;
//            @display("p=700,300");
//        }
//
//        // HQ Servers
//        dhcpPool: DHCP {
//            address = 10;
//            @display("p=500,150");
//        }
//
//        dnsServer: DNSServer {
//            address = 11;
//            @display("p=650,150");
//        }
//
//        mailServer: MailServer {
//            address = 12;
//            @display("p=800,150");
//        }
//
//        webFarm: WebServer {
//            address = 13;
//            @display("p=950,150");
//        }
//
//        // DATABASE CLUSTER
//        dbPrimary: DatabaseServer {
//            address = 20;
//            role = "primary";
//            @display("p=500,450;i=device/server,gold");
//        }
//
//        dbSecondary: DatabaseServer {
//            address = 21;
//            role = "secondary";
//            @display("p=650,450;i=device/server,cyan");
//        }
//
//        dbBackup: DatabaseServer {
//            address = 22;
//            role = "backup";
//            @display("p=800,450;i=device/server,grey");
//        }
//
//        // Branch A Infrastructure
//        branchA_Router: Router {
//            address = 100;
//            @display("p=200,300");
//        }
//
//        switchStackA: Switch {
//            address = 101;
//            @display("p=200,450");
//        }
//
//        // Branch A Devices
//        workstationA1: PC { address = 111; vlanId = 10; @display("p=50,600"); }
//        workstationA2: PC { address = 112; vlanId = 10; @display("p=120,600"); }
//        workstationA3: PC { address = 113; vlanId = 10; @display("p=190,600"); }
//        terminalA1: PC { address = 114; vlanId = 10; @display("p=260,600"); }
//        terminalA2: PC { address = 115; vlanId = 10; @display("p=330,600"); }
//        terminalA3: PC { address = 116; vlanId = 10; @display("p=400,600"); }
//        printServerA1: PC { address = 117; vlanId = 10; @display("p=50,670"); }
//        printServerA2: PC { address = 118; vlanId = 10; @display("p=120,670"); }
//
//        // Branch B Infrastructure
//        branchB_Router: Router {
//            address = 200;
//            @display("p=700,600");
//        }
//
//        switchStackB: Switch {
//            address = 210;
//            @display("p=700,750");
//        }
//
//        // Branch B Devices
//        workstationB1: PC { address = 211; vlanId = 20; @display("p=550,900"); }
//        workstationB2: PC { address = 212; vlanId = 20; @display("p=620,900"); }
//        workstationB3: PC { address = 213; vlanId = 20; @display("p=690,900"); }
//        terminalB1: PC { address = 214; vlanId = 20; @display("p=760,900"); }
//        terminalB2: PC { address = 215; vlanId = 20; @display("p=830,900"); }
//        terminalB3: PC { address = 216; vlanId = 20; @display("p=900,900"); }
//        printServerB1: PC { address = 217; vlanId = 20; @display("p=550,970"); }
//        printServerB2: PC { address = 218; vlanId = 20; @display("p=620,970"); }
//
//        // Branch C Infrastructure
//        branchC_Router: Router {
//            address = 300;
//            @display("p=1200,300");
//        }
//
//        switchStackC: Switch {
//            address = 301;
//            @display("p=1200,450");
//        }
//
//        // Branch C Devices
//        workstationC1: PC { address = 311; vlanId = 30; @display("p=1050,600"); }
//        workstationC2: PC { address = 312; vlanId = 30; @display("p=1120,600"); }
//        workstationC3: PC { address = 313; vlanId = 30; @display("p=1190,600"); }
//        terminalC1: PC { address = 314; vlanId = 30; @display("p=1260,600"); }
//        terminalC2: PC { address = 315; vlanId = 30; @display("p=1330,600"); }
//        terminalC3: PC { address = 316; vlanId = 30; @display("p=1400,600"); }
//        printServerC1: PC { address = 317; vlanId = 30; @display("p=1050,670"); }
//        printServerC2: PC { address = 318; vlanId = 30; @display("p=1120,670"); }
//
//    connections:
//        // HQ Core to Servers
//        coreRouter.ppp++ <--> dhcpPool.ppp;
//        coreRouter.ppp++ <--> dnsServer.ppp;
//        coreRouter.ppp++ <--> mailServer.ppp;
//        coreRouter.ppp++ <--> webFarm.ppp;
//
//        // HQ Core to Database Cluster
//        coreRouter.ppp++ <--> dbPrimary.ppp;
//        coreRouter.ppp++ <--> dbSecondary.ppp;
//        coreRouter.ppp++ <--> dbBackup.ppp;
//
//        // Core Router to Branch Routers
//        coreRouter.ppp++ <--> branchA_Router.ppp++;
//        coreRouter.ppp++ <--> branchB_Router.ppp++;
//        coreRouter.ppp++ <--> branchC_Router.ppp++;
//
//        // Branch A Connections
//        branchA_Router.ppp++ <--> switchStackA.ppp++;
//        switchStackA.ppp++ <--> workstationA1.ppp;
//        switchStackA.ppp++ <--> workstationA2.ppp;
//        switchStackA.ppp++ <--> workstationA3.ppp;
//        switchStackA.ppp++ <--> terminalA1.ppp;
//        switchStackA.ppp++ <--> terminalA2.ppp;
//        switchStackA.ppp++ <--> terminalA3.ppp;
//        switchStackA.ppp++ <--> printServerA1.ppp;
//        switchStackA.ppp++ <--> printServerA2.ppp;
//
//        // Branch B Connections
//        branchB_Router.ppp++ <--> switchStackB.ppp++;
//        switchStackB.ppp++ <--> workstationB1.ppp;
//        switchStackB.ppp++ <--> workstationB2.ppp;
//        switchStackB.ppp++ <--> workstationB3.ppp;
//        switchStackB.ppp++ <--> terminalB1.ppp;
//        switchStackB.ppp++ <--> terminalB2.ppp;
//        switchStackB.ppp++ <--> terminalB3.ppp;
//        switchStackB.ppp++ <--> printServerB1.ppp;
//        switchStackB.ppp++ <--> printServerB2.ppp;
//
//        // Branch C Connections
//        branchC_Router.ppp++ <--> switchStackC.ppp++;
//        switchStackC.ppp++ <--> workstationC1.ppp;
//        switchStackC.ppp++ <--> workstationC2.ppp;
//        switchStackC.ppp++ <--> workstationC3.ppp;
//        switchStackC.ppp++ <--> terminalC1.ppp;
//        switchStackC.ppp++ <--> terminalC2.ppp;
//        switchStackC.ppp++ <--> terminalC3.ppp;
//        switchStackC.ppp++ <--> printServerC1.ppp;
//        switchStackC.ppp++ <--> printServerC2.ppp;
//}


package simulations;

import zpractive.Modules.Router;
import zpractive.Modules.Switch;
import zpractive.Modules.PC;
import zpractive.Modules.DHCP;
import zpractive.Modules.DNSServer;
import zpractive.Modules.MailServer;
import zpractive.Modules.WebServer;
import zpractive.Modules.DatabaseServer;

network BankingNetwork {
    submodules:
        // HQ Core Infrastructure
        coreRouter: Router {
            address = 1;
            @display("p=700,300");
        }

        // HQ Servers (NOW COMPOUND MODULES - TCP inside!)
        dhcpPool: DHCP {
            address = 10;
            @display("p=500,150");
        }

        dnsServer: DNSServer {
            address = 11;
            @display("p=650,150");
        }

        mailServer: MailServer {
            address = 12;
            @display("p=800,150");
        }

        webFarm: WebServer {
            address = 13;
            @display("p=950,150");
        }

        // DATABASE CLUSTER (NOW COMPOUND MODULES - TCP inside!)
        dbPrimary: DatabaseServer {
            address = 20;
            role = "primary";
            @display("p=500,450;i=device/server,gold");
        }

        dbSecondary: DatabaseServer {
            address = 21;
            role = "secondary";
            @display("p=650,450;i=device/server,cyan");
        }

        dbBackup: DatabaseServer {
            address = 22;
            role = "backup";
            @display("p=800,450;i=device/server,grey");
        }

        // Branch A Infrastructure
        branchA_Router: Router {
            address = 100;
            @display("p=200,300");
        }

        switchStackA: Switch {
            address = 101;
            @display("p=200,450");
        }

        // Branch A Devices (NOW COMPOUND MODULES - TCP inside!)
        workstationA1: PC { address = 111; vlanId = 10; @display("p=50,600"); }
        workstationA2: PC { address = 112; vlanId = 10; @display("p=120,600"); }
        workstationA3: PC { address = 113; vlanId = 10; @display("p=190,600"); }
        terminalA1: PC { address = 114; vlanId = 10; @display("p=260,600"); }
        terminalA2: PC { address = 115; vlanId = 10; @display("p=330,600"); }
        terminalA3: PC { address = 116; vlanId = 10; @display("p=400,600"); }
        printServerA1: PC { address = 117; vlanId = 10; @display("p=50,670"); }
        printServerA2: PC { address = 118; vlanId = 10; @display("p=120,670"); }

        // Branch B Infrastructure
        branchB_Router: Router {
            address = 200;
            @display("p=700,600");
        }

        switchStackB: Switch {
            address = 210;
            @display("p=700,750");
        }

        // Branch B Devices (NOW COMPOUND MODULES - TCP inside!)
        workstationB1: PC { address = 211; vlanId = 20; @display("p=550,900"); }
        workstationB2: PC { address = 212; vlanId = 20; @display("p=620,900"); }
        workstationB3: PC { address = 213; vlanId = 20; @display("p=690,900"); }
        terminalB1: PC { address = 214; vlanId = 20; @display("p=760,900"); }
        terminalB2: PC { address = 215; vlanId = 20; @display("p=830,900"); }
        terminalB3: PC { address = 216; vlanId = 20; @display("p=900,900"); }
        printServerB1: PC { address = 217; vlanId = 20; @display("p=550,970"); }
        printServerB2: PC { address = 218; vlanId = 20; @display("p=620,970"); }

        // Branch C Infrastructure
        branchC_Router: Router {
            address = 300;
            @display("p=1200,300");
        }

        switchStackC: Switch {
            address = 301;
            @display("p=1200,450");
        }

        // Branch C Devices (NOW COMPOUND MODULES - TCP inside!)
        workstationC1: PC { address = 311; vlanId = 30; @display("p=1050,600"); }
        workstationC2: PC { address = 312; vlanId = 30; @display("p=1120,600"); }
        workstationC3: PC { address = 313; vlanId = 30; @display("p=1190,600"); }
        terminalC1: PC { address = 314; vlanId = 30; @display("p=1260,600"); }
        terminalC2: PC { address = 315; vlanId = 30; @display("p=1330,600"); }
        terminalC3: PC { address = 316; vlanId = 30; @display("p=1400,600"); }
        printServerC1: PC { address = 317; vlanId = 30; @display("p=1050,670"); }
        printServerC2: PC { address = 318; vlanId = 30; @display("p=1120,670"); }

    connections:
        // HQ Core to Servers via PPP (TCP is INSIDE the compound modules!)
        coreRouter.ppp++ <--> dhcpPool.ppp;
        coreRouter.ppp++ <--> dnsServer.ppp;
        coreRouter.ppp++ <--> mailServer.ppp;
        coreRouter.ppp++ <--> webFarm.ppp;

        // HQ Core to Database Cluster via PPP
        coreRouter.ppp++ <--> dbPrimary.ppp;
        coreRouter.ppp++ <--> dbSecondary.ppp;
        coreRouter.ppp++ <--> dbBackup.ppp;

        // Core Router to Branch Routers via PPP
        coreRouter.ppp++ <--> branchA_Router.ppp++;
        coreRouter.ppp++ <--> branchB_Router.ppp++;
        coreRouter.ppp++ <--> branchC_Router.ppp++;

        // Branch A connections
        branchA_Router.ppp++ <--> switchStackA.ppp++;
        switchStackA.ppp++ <--> workstationA1.ppp;
        switchStackA.ppp++ <--> workstationA2.ppp;
        switchStackA.ppp++ <--> workstationA3.ppp;
        switchStackA.ppp++ <--> terminalA1.ppp;
        switchStackA.ppp++ <--> terminalA2.ppp;
        switchStackA.ppp++ <--> terminalA3.ppp;
        switchStackA.ppp++ <--> printServerA1.ppp;
        switchStackA.ppp++ <--> printServerA2.ppp;

        // Branch B connections
        branchB_Router.ppp++ <--> switchStackB.ppp++;
        switchStackB.ppp++ <--> workstationB1.ppp;
        switchStackB.ppp++ <--> workstationB2.ppp;
        switchStackB.ppp++ <--> workstationB3.ppp;
        switchStackB.ppp++ <--> terminalB1.ppp;
        switchStackB.ppp++ <--> terminalB2.ppp;
        switchStackB.ppp++ <--> terminalB3.ppp;
        switchStackB.ppp++ <--> printServerB1.ppp;
        switchStackB.ppp++ <--> printServerB2.ppp;

        // Branch C connections
        branchC_Router.ppp++ <--> switchStackC.ppp++;
        switchStackC.ppp++ <--> workstationC1.ppp;
        switchStackC.ppp++ <--> workstationC2.ppp;
        switchStackC.ppp++ <--> workstationC3.ppp;
        switchStackC.ppp++ <--> terminalC1.ppp;
        switchStackC.ppp++ <--> terminalC2.ppp;
        switchStackC.ppp++ <--> terminalC3.ppp;
        switchStackC.ppp++ <--> printServerC1.ppp;
        switchStackC.ppp++ <--> printServerC2.ppp;
}
