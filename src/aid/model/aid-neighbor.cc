/*
 * aid-neighbor.cc
 *
 *  Created on: 08/11/2011
 *      Author: sergio
 */

#include "aid-neighbor.h"

namespace ns3 {

using namespace std;

AIDNeighbor::AIDNeighbor() {
}

/// Return expire time for neighbor node with address addr, if exists, else return 0.
Time AIDNeighbor::GetExpireTime(Ipv4Address addr) {
	Purge();
	for (std::vector<Neighbor>::const_iterator i = m_table.begin(); i
			!= m_table.end(); ++i) {
		if (i->m_neighborAddress == addr)
			return (i->m_expireTime - Simulator::Now());
	}
	return Seconds(0);
}

/// Check that node with address addr  is neighbor
bool AIDNeighbor::IsNeighbor(Ipv4Address addr) {
	Purge();
	for (std::vector<Neighbor>::const_iterator i = m_table.begin(); i
			!= m_table.end(); ++i) {
		if (i->m_neighborAddress == addr)
			return true;
	}
	return false;
}

/// Update expire time for entry with address addr, if it exists, else add new entry
void AIDNeighbor::Update(Ipv4Address addr, Vector2d position, Vector2d velocity, Time expire) {
	for (std::vector<Neighbor>::iterator i = m_table.begin(); i
			!= m_table.end(); ++i)
		if (i->m_neighborAddress == addr) {
			i->m_expireTime = std::max(expire + Simulator::Now(),
					i->m_expireTime);
			/*
			if (i->m_hardwareAddress == Mac48Address())
				i->m_hardwareAddress = LookupMacAddress(i->m_neighborAddress);
			*/
			return;
		}

	//NS_LOG_LOGIC("Open link to " << addr);
	//Neighbor neighbor(addr, LookupMacAddress(addr), expire + Simulator::Now());
	//m_nb.push_back(neighbor);
	Purge();
}

/// Remove all expired entries
void AIDNeighbor::Purge() {

	if (m_table.empty())
		return;
}

/// Schedule m_ntimer.
void AIDNeighbor::ScheduleTimer() {

}

void AIDNeighbor::Add(Ipv4Address addr, Vector2d position, Vector2d velocity, Time expire) {
	m_table.push_back(Neighbor(addr, position, velocity, expire));
}


void AIDNeighbor::Print(Address address) {
	cout << "\n" << address << "\n";
	cout << setfill(' ') << "ADDR      "
			<< "\t" << setw(16) << "POSITION"
			<< "\t" << setw(16) << "VELOCITY"
			<< "\t" << setw(23) << "EXPIRATION TIME\n";
	cout << setfill('-') << setw(90) << "\n";
	for (std::vector<Neighbor>::iterator i = m_table.begin(); i != m_table.end(); ++i) {
		cout << setfill(' ') << i->m_neighborAddress << "\t" << setw(16) << i->m_pos.toString()
				<< "\t" << setw(16) << i->m_vel.toString()
				<< "\t" << setw(8) << i->m_expireTime << "\n";
	}
}

}
