/*
 * aid.cc
 *
 *  Created on: 18/10/2011
 *      Author: sergio
 */

#include "aid.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Aid);

TypeId Aid::GetTypeId(void) {
	static TypeId
			tid = TypeId("ns3::Aid")
				.SetParent<Application>()
				.AddConstructor<Aid>()
				.AddAttribute("Remote",
							"The address of the machine we want to send.",
							Ipv4AddressValue(),
							MakeIpv4AddressAccessor(&Aid::m_remote),
							MakeIpv4AddressChecker())
				.AddAttribute("Port",
							"The port of the application we want to send.",
							IntegerValue(0),
							MakeIntegerAccessor(&Aid::m_port),
							MakeIntegerChecker<uint16_t>())
				.AddAttribute("Sending",
							"start sending messages.",
							BooleanValue(false),
							MakeBooleanAccessor(&Aid::m_sending),
							MakeBooleanChecker())
				.AddAttribute("TRange",
							"Transmission range.",
							IntegerValue(250),
							MakeIntegerAccessor(&Aid::m_transmission_range),
							MakeIntegerChecker<uint16_t>())
				.AddAttribute("Interval",
							"Wait  interval  seconds between sending each packet.",
							TimeValue(Seconds(1)),
							MakeTimeAccessor(&Aid::m_interval),
							MakeTimeChecker());
	return tid;
}

Aid::Aid() : m_port(1978), m_status(-1), m_interval(Seconds(1)), m_socket(0), m_sending(false), m_hello_timer((Timer::CANCEL_ON_DESTROY)) {
}

void Aid::StopApplication(void) {
	m_next.Cancel();
	m_socket->Close();
}

uint32_t Aid::GetApplicationId(void) const {
	Ptr<Node> node = GetNode();
	for (uint32_t i = 0; i < node->GetNApplications(); ++i) {
		if (node->GetApplication(i) == this) {
			return i;
		}
	};
	NS_ASSERT_MSG (false, "forgot to add application to node");
	return 0; // quiet compiler
}

void Aid::StartApplication(void) {
	Ptr<SocketFactory> udp = GetNode()->GetObject<SocketFactory> (
			UdpSocketFactory::GetTypeId());

	m_socket = udp->CreateSocket();
	m_socket->Bind(InetSocketAddress(m_port));
	m_socket->SetRecvCallback(MakeCallback(&Aid::Receive, this));
	m_socket->SetAllowBroadcast(true);

	StartSendMessages();
}

void Aid::StartSendMessages(void) {
	m_hello_timer.SetFunction(&Aid::Send, this);
	m_hello_timer.Schedule(MilliSeconds(UniformVariable().GetInteger(0, 100)));
}

void Aid::DoDispose(void) {
	m_socket = 0;
	for (std::map<Ptr<Socket> , Ipv4InterfaceAddress>::iterator iter =
			m_socketAddresses.begin(); iter != m_socketAddresses.end(); iter++) {
		iter->first->Close();
	}
	m_socketAddresses.clear();
	//Ipv4RoutingProtocol::DoDispose ();
	Application::DoDispose();
}

void Aid::Send() {

	Ptr<Node> node = GetNode();
	//Ptr<Object> object = node;
	Ptr<MobilityModel> model = node->GetObject<MobilityModel> ();
	std::cout << "[Node " << node->GetId() << "] Sending\n";

	Ptr<Packet> p = Create<Packet> ();
	AIDHeader h;

	h.setPos(model->GetPosition().x, model->GetPosition().y);
	h.setVel(model->GetVelocity().x, model->GetVelocity().y);

	h.SetType(AIDHeader::HELLO);
	p->AddHeader(h);

	Ipv4Address destination = Ipv4Address("255.255.255.255");
	m_socket->SendTo(p, m_port, InetSocketAddress(destination, m_port));

	//m_next = Simulator::Schedule(m_interval, &Aid::Send, this);
	//m_htimer.SetFunction(&RoutingProtocol::HelloTimerExpire, this);
	//m_htimer.Schedule(MilliSeconds(UniformVariable().GetInteger(0, 100)));

	m_hello_timer.Cancel();
	Time t = Time(0.01 * MilliSeconds(UniformVariable().GetInteger(0, 100)));
	m_hello_timer.Schedule(m_interval - t);
}

void Aid::Receive(Ptr<Socket> socket) {
	while (socket->GetRxAvailable() > 0) {
		Ptr<Node> node = GetNode();
		// to get local position and velocity
		Ptr<MobilityModel> model = node->GetObject<MobilityModel> ();
		Vector2d local_pos(model->GetPosition());
		Vector2d local_vel(model->GetVelocity());

		std::cout << "[Node " << node->GetId() << "] receiving packet...\n";

		Address from;
		Ptr<Packet> p = socket->RecvFrom(from);

		Ipv4Address addr = InetSocketAddress::ConvertFrom(from).GetIpv4();

		AIDHeader aidh;
		p->RemoveHeader(aidh);

		if (aidh.GetType() == AIDHeader::HELLO) {
			Time t = LinkLifeTime::getExpirationTime(local_pos, local_vel,
					aidh.getPos(), aidh.getVel(), m_transmission_range);
			if (m_table.IsNeighbor(addr)) {
				m_table.Update(addr, aidh.getPos(), aidh.getVel(), t);
			} else {
				m_table.Add(addr, aidh.getPos(), aidh.getVel(), t);
			}
            Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
            Ipv4InterfaceAddress ipv4_int_addr = ipv4->GetAddress (1, 0);
            Ipv4Address ip_addr = ipv4_int_addr.GetLocal ();

			m_table.Print(ip_addr);
		} else if (aidh.GetType() == AIDHeader::UPDATE) {
			std::cout << "\t UPDATE\n";
		}

	}

}

}
