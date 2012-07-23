/*
 * bp-rt-trend-of-delivery-neigh-hello.cc
 *
 *  Created on: 15/04/2012
 *      Author: sergio
 */

#include "bp-rt-trend-of-delivery-neigh-hello.h"
//#include "ieee754.h"

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/* IEEE 754 pack unpack */

uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

NS_LOG_COMPONENT_DEFINE ("NeighHello");

namespace ns3 {
namespace bundleProtocol {

NS_OBJECT_ENSURE_REGISTERED(NeighHello);


NeighHello::NeighHello() {
}

void NeighHello::SetBundleEndpointId(BundleEndpointId eid) {
        m_eid = eid;
}

BundleEndpointId NeighHello::GetBundleEndpointId() const {
        return m_eid;
}

TypeId NeighHello::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::bundleProtocol::NeighHello")
                        .SetParent<Header>()
                        .AddConstructor<NeighHello>();

        return tid;
}

TypeId NeighHello::GetInstanceTypeId(void) const {
        return GetTypeId();
}

void NeighHello::Print(std::ostream &os) const {
        os << "NeighHello: eid = " << m_eid << " position = (" << m_pos.x << ", " << m_pos.y << ")  velocity = (" << m_vel.x << ", " << m_vel.y << ") time stamp = " << m_timeStamp.GetSeconds();
}

uint32_t NeighHello::GetSerializedSize(void) const {
        return m_eid.GetSerializedSize() + 4 + 32 + 8;
}



void NeighHello::Serialize(Buffer::Iterator start) const {
        int length = m_eid.GetSerializedSize();
        Buffer::Iterator i = start;
        i.WriteHtonU32(length);
        uint8_t buf[length];
        m_eid.Serialize(buf);
        i.Write(buf, length);

        i.WriteU64(pack754_64(m_pos.x));
        i.WriteU64(pack754_64(m_pos.y));

        i.WriteU64(pack754_64(m_vel.x));
        i.WriteU64(pack754_64(m_vel.y));

        i.WriteU64(pack754_64(m_timeStamp.GetSeconds()));
}

uint32_t NeighHello::Deserialize(Buffer::Iterator start) {
        Buffer::Iterator i = start;
        uint32_t length = i.ReadNtohU32();
        uint8_t buf[length];
        i.Read(buf, length);
        m_eid = BundleEndpointId::Deserialize(buf);

        m_pos.x = unpack754_64(i.ReadU64());
        m_pos.y = unpack754_64(i.ReadU64());

        m_vel.x = unpack754_64(i.ReadU64());
        m_vel.y = unpack754_64(i.ReadU64());

        m_timeStamp = Seconds(unpack754_64(i.ReadU64()));

        return GetSerializedSize();
}

void NeighHello::setPos(double x, double y) {
        m_pos.x = x;
        m_pos.y = y;
}

void NeighHello::setVel(double x, double y) {
        m_vel.x = x;
        m_vel.y = y;
}

Vector2d NeighHello::getPos() const {
        return m_pos;
}

Vector2d NeighHello::getVel() const{
        return m_vel;
}

void NeighHello::setTimeStamp(Time timeStamp) {
        m_timeStamp = timeStamp;
}

Time NeighHello::getTimeStamp() const {
        return m_timeStamp;
}

} /* namespace bundleProtocol */
} /* namespace ns3 */
