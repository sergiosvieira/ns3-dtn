/*
 * aid-neighbor.h
 *
 *  Created on: 08/11/2011
 *      Author: sergio
 */

#ifndef AIDNEIGHBOR_H_
#define AIDNEIGHBOR_H_

#include "ns3/simulator.h"
#include "ns3/timer.h"
#include "ns3/ipv4-address.h"
#include "ns3/wifi-mac-header.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "ns3/vector2d.h"
#include "ns3/nstime.h"

namespace ns3 {

class AIDNeighbor {
public:
	AIDNeighbor();
	  /// Return expire time for neighbor node with address addr, if exists, else return 0.
	  Time GetExpireTime (Ipv4Address addr);
	  /// Check that node with address addr  is neighbor
	  bool IsNeighbor (Ipv4Address addr);
	  /// Update expire time for entry with address addr, if it exists, else add new entry
	  void Update (Ipv4Address addr, Vector2d position, Vector2d velocity, Time expire);
	  /// Remove all expired entries
	  void Purge ();
	  /// Schedule m_ntimer.
	  void ScheduleTimer ();
	  /// Remove all entries
	  void Clear () { m_table.clear (); }
	  /// Add entry
	  //void Add(Ipv4Address addr, Mac48Address mac, Time expire_time);
	  void Add(Ipv4Address addr, Vector2d position, Vector2d velocity, Time expire_time);
	  /// Print entries
	  void Print(Address address);
private:
	struct Neighbor {
		Ipv4Address m_neighborAddress;
		//Mac48Address m_hardwareAddress;
		Time m_expireTime;
		Vector2d m_pos;
		Vector2d m_vel;
		bool close;

		Neighbor(Ipv4Address ip, Vector2d p, Vector2d v, Time t) : m_neighborAddress(ip), m_expireTime(t), close(false) {
			m_pos.x = p.x; m_pos.y = p.y;
			m_vel.x = v.x; m_vel.y = p.y;
		}

	};
	std::vector<Neighbor> m_table;
};

}
#endif /* AIDNEIGHBOR_H_ */
