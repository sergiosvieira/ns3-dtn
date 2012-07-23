/*
 * aid-header.cc
 *
 *  Created on: 31/10/2011
 *      Author: sergio
 */

#include "aid-header.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (AIDHeader);

AIDHeader::AIDHeader() :
	m_type(0) {

}

AIDHeader::~AIDHeader() {
}

TypeId AIDHeader::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AIDHeader").SetParent<Header> (). AddConstructor<
					AIDHeader> ();
	return tid;
}

TypeId AIDHeader::GetInstanceTypeId(void) const {
	return GetTypeId();
}

uint32_t AIDHeader::GetSerializedSize(void) const {
	return 40;
}

void AIDHeader::Serialize(Buffer::Iterator start) const {
	start.WriteU8(m_type);

	/*
	start.WriteU64(pack754_64(m_pos.x));
	start.WriteU64(pack754_64(m_pos.y));

	start.WriteU64(pack754_64(m_vel.x));
	start.WriteU64(pack754_64(m_vel.y));
	*/
}

uint32_t AIDHeader::Deserialize(Buffer::Iterator start) {
	m_type = start.ReadU8();

	/*
	m_pos.x = unpack754_64(start.ReadU64());
	m_pos.y = unpack754_64(start.ReadU64());

	m_vel.x = unpack754_64(start.ReadU64());
	m_vel.y = unpack754_64(start.ReadU64());
	*/
	return 40;
}

void AIDHeader::Print(std::ostream &os) const {
	os << "type=" << (uint32_t) m_type;
}

void AIDHeader::SetType(uint8_t type) {
	m_type = type;
}

uint8_t AIDHeader::GetType() {
	return m_type;
}

void AIDHeader::setPos(double x, double y) {
	m_pos.x = x;
	m_pos.y = y;
}

void AIDHeader::setVel(double x, double y) {
	m_vel.x = x;
	m_vel.y = y;
}

Vector2d AIDHeader::getPos() {
	return m_pos;
}

Vector2d AIDHeader::getVel() {
	return m_vel;
}

}
