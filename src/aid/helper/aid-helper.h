/*
 * aidhelper.h
 *
 *  Created on: 19/10/2011
 *      Author: sergiosvieira
 */

#ifndef AIDHELPER_H_
#define AIDHELPER_H_

#include "ns3/ipv4-address.h"
#include "ns3/object-factory.h"
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/attribute.h"

namespace ns3 {

class AidHelper {
public:
	AidHelper(Ipv4Address remote, uint16_t port, bool sending, uint16_t transmission_range);
	virtual ~AidHelper();
	ApplicationContainer Install(NodeContainer nodes) const;
	ApplicationContainer Install(Ptr<Node> node) const;
	ApplicationContainer Install(std::string nodeName) const;
	void SetAttribute(std::string name, const AttributeValue &value);
private:
	Ptr<Application> InstallPriv(Ptr<Node> node) const;
	ObjectFactory m_factory;
};

} /* namespace ns3 */
#endif /* AIDHELPER_H_ */
