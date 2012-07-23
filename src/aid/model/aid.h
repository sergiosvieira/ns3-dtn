/*
 * aid.h
 *
 *  Created on: 18/10/2011
 *      Author: sergio
 */

#ifndef AID_H_
#define AID_H_

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/integer.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/socket-factory.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/inet-socket-address.h"
#include "ns3/simulator.h"
#include "ns3/aid-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/ethernet-header.h"
#include "ns3/packet.h"
#include "ns3/mobility-model.h"
#include "ns3/aid-neighbor.h"
#include "ns3/link-life-time.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/random-variable.h"
#include "ns3/packet-socket-address.h"
#include <map>
#include <stdint.h>

namespace ns3 {

class Aid: public Application {
public:
	static TypeId GetTypeId(void);
	Aid();
	virtual void StartApplication(void);
	virtual void StopApplication(void);
	virtual void DoDispose(void);
	virtual void StartSendMessages(void);
	uint32_t GetApplicationId(void) const;
	Ipv4Address m_remote;
	uint16_t m_port;
	int m_status;
	Time m_interval;
	uint16_t m_transmission_range;
	Ptr<Socket> m_socket;
	std::map< Ptr<Socket>, Ipv4InterfaceAddress > m_socketAddresses;
	bool m_sending;
	EventId m_next;
private:
	Timer m_hello_timer;
	AIDNeighbor m_table;
	void Receive(Ptr<Socket> socket);
	void Send();
	void Forward(Ptr<Socket> socket);
};

}

#endif /* AID_H_ */
