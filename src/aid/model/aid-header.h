/*
 * aid-header.h
 *
 *  Created on: 31/10/2011
 *      Author: sergio
 */

#ifndef AIDHEADER_H_
#define AIDHEADER_H_

#include "ns3/header.h"
#include "ns3/ptr.h"
#include "ns3/vector2d.h"

namespace ns3 {
class AIDHeader: public Header {
public:
	static TypeId GetTypeId (void);

	enum {
		HELLO = 0, UPDATE = 1
	};

	AIDHeader();
	virtual ~AIDHeader();
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator i) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);
	virtual void Print(std::ostream &os) const;
	virtual TypeId GetInstanceTypeId (void) const;
	void SetType(uint8_t type);
	uint8_t GetType();
	void setPos(double x, double y);
	void setVel(double x, double y);
	Vector2d getPos();
	Vector2d getVel();
private:
	uint8_t m_type;
	Vector2d m_pos; // current position
	Vector2d m_vel; // velocity
};
}
#endif /* AIDHEADER_H_ */
