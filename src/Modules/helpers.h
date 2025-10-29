#ifndef HELPERS_H_
#define HELPERS_H_

#include <omnetpp.h>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>

// ==================== TCP MESSAGE TYPES ====================
#define TCP_SYN         100
#define TCP_SYNACK      101
#define TCP_ACK         102
#define TCP_DATA        103
#define TCP_FIN         104
#define TCP_RST         105
#define TCP_TIMEOUT     106

// TCP Flags (for combination like SYN+ACK)
#define TCP_FLAG_SYN    0x01
#define TCP_FLAG_ACK    0x02
#define TCP_FLAG_FIN    0x04
#define TCP_FLAG_RST    0x08
#define TCP_FLAG_PSH    0x10

// TCP Ports (Well-known ports)
#define TCP_PORT_HTTP       80
#define TCP_PORT_HTTPS      443
#define TCP_PORT_SMTP       25
#define TCP_PORT_POP3       110
#define TCP_PORT_IMAP       143
#define TCP_PORT_MYSQL      3306
#define TCP_PORT_DNS        53
#define TCP_PORT_FTP        21

// TCP Helper Macros
#define TCP_SEQ(msg)        (msg->par("seq").longValue())
#define TCP_ACK_NUM(msg)    (msg->par("ack").longValue())
#define TCP_FLAGS(msg)      (msg->par("flags").longValue())
#define TCP_WINDOW(msg)     (msg->par("window").longValue())
#define TCP_SRC_PORT(msg)   (msg->par("srcPort").longValue())
#define TCP_DEST_PORT(msg)  (msg->par("destPort").longValue())


// Additional helper macros for encryption
#define OFFERED_IP(msg) (msg->par("offeredIP").longValue())
#define REQUESTED_NAME(msg) (msg->par("requestedName").stringValue())
#define ACCOUNT_ID(msg) (msg->par("accountId").longValue())
#define AMOUNT(msg) (msg->par("amount").longValue())
#define QUERY_TYPE(msg) (msg->par("queryType").stringValue())
#define PAYLOAD(msg) (msg->par("payload").stringValue())

using namespace omnetpp;

// Protocol message types
enum {
    // DHCP Protocol
    DHCP_DISCOVER = 30,
    DHCP_OFFER = 31,
    DHCP_REQUEST = 32,
    DHCP_ACK = 33,

    // DNS Protocol
    DNS_QUERY = 40,
    DNS_RESPONSE = 41,

    // HTTP Protocol
    HTTP_GET = 50,
    HTTP_POST = 51,
    HTTP_RESPONSE = 52,

    // Email Protocol
    SMTP_SEND = 60,
    SMTP_ACK = 61,
    POP3_FETCH = 62,
    POP3_RESPONSE = 63,

    // DATABASE Protocol (NEW)
    DB_QUERY_READ = 80,      // SELECT queries
    DB_QUERY_WRITE = 81,     // INSERT/UPDATE queries
    DB_RESPONSE_SUCCESS = 82,
    DB_RESPONSE_ERROR = 83,
    DB_SYNC = 84,            // Primary → Secondary sync

    // General
    PING = 70,
    PONG = 71,
    DATA_MSG = 90
};

// Message creator
static cMessage* mk(const char* n, int k, long s, long d) {
    auto *m = new cMessage(n, k);
    m->addPar("src") = s;
    m->addPar("dst") = d;
    m->addPar("srcIP") = (long)0;
    m->addPar("dstIP") = (long)0;
    m->addPar("vlanId") = (long)0;
    m->addPar("offeredIP") = (long)0;
    m->addPar("requestedName") = "";
    m->addPar("payload") = "";
    m->addPar("queryType") = "";       // NEW: "READ" or "WRITE"
    m->addPar("accountId") = (long)0;  // NEW: for DB queries
    m->addPar("amount") = (long)0;     // NEW: for transactions
    return m;
}

// Parameter getters
static inline long SRC(cMessage* m) {
    return m->par("src").longValue();
}

static inline long DST(cMessage* m) {
    return m->par("dst").longValue();
}

static inline long SRCIP(cMessage* m) {
    return m->hasPar("srcIP") ? m->par("srcIP").longValue() : 0;
}

static inline long DSTIP(cMessage* m) {
    return m->hasPar("dstIP") ? m->par("dstIP").longValue() : 0;
}

static inline int VLAN(cMessage* m) {
    return m->hasPar("vlanId") ? m->par("vlanId").longValue() : 0;
}


// IP converter
static std::string ipToStr(long ip) {
    if (ip == 0) return "0.0.0.0";

    char buf[32];
    sprintf(buf, "%ld.%ld.%ld.%ld",
        (ip >> 24) & 0xFF,
        (ip >> 16) & 0xFF,
        (ip >> 8) & 0xFF,
        ip & 0xFF);
    return std::string(buf);
}

// Create IP from octets
static inline long makeIP(int a, int b, int c, int d) {
    return ((long)a << 24) | ((long)b << 16) | ((long)c << 8) | (long)d;
}

// Get subnet
static inline long getSubnet(long ip) {
    return ip & 0xFFFFFF00;
}

// Create TCP segment
static inline cMessage* MK_TCP(const char *name, int kind, int src, int dst,
                               long seq, long ack, int flags, int srcPort, int destPort) {
    cMessage *msg = new cMessage(name, kind);
    msg->addPar("src").setLongValue(src);
    msg->addPar("dst").setLongValue(dst);
    msg->addPar("seq").setLongValue(seq);
    msg->addPar("ack").setLongValue(ack);
    msg->addPar("flags").setLongValue(flags);
    msg->addPar("srcPort").setLongValue(srcPort);
    msg->addPar("destPort").setLongValue(destPort);
    msg->addPar("window").setLongValue(65535);  // Default window size
    return msg;
}

// ==================== XOR ENCRYPTION MODULE ====================

#define XOR_KEY 0xAB  // Encryption key

// XOR Encrypt function
inline std::string xorEncrypt(const std::string& data, unsigned char key = XOR_KEY) {
    std::string result = data;
    for (size_t i = 0; i < result.length(); i++) {
        result[i] ^= key;
    }
    return result;
}

// XOR Decrypt function (same as encrypt due to XOR property)
inline std::string xorDecrypt(const std::string& data, unsigned char key = XOR_KEY) {
    return xorEncrypt(data, key);
}

// Convert string to hex for display
inline std::string toHex(const std::string& data, size_t maxLen = 16) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    size_t len = (data.length() < maxLen) ? data.length() : maxLen;
    for (size_t i = 0; i < len; i++) {
        ss << std::setw(2) << (int)(unsigned char)data[i];
        if (i < len - 1) ss << " ";
    }
    if (data.length() > maxLen) ss << "...";
    return ss.str();
}

#endif /* HELPERS_H_ */
