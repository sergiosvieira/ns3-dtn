/*
 * aidhelper.cpp
 *
 *  Created on: 19/10/2011
 *      Author: sergiosvieira
 */

#include "aid-helper.h"
#include "ns3/aid.h"
#include "ns3/names.h"
#include "ns3/integer.h"
#include "ns3/boolean.h"

namespace ns3 {

AidHelper::AidHelper(Ipv4Address remote, uint16_t port, bool sending, uint16_t transmission_range) {
	m_factory.SetTypeId("ns3::Aid");
	m_factory.Set("Remote", Ipv4AddressValue(remote));
	m_factory.Set("Port", IntegerValue(port));
	m_factory.Set("Sending", BooleanValue(sending));
	m_factory.Set("TRange", IntegerValue(transmission_range));
}

AidHelper::~AidHelper() {
}

void AidHelper::SetAttribute(std::string name, const AttributeValue &value) {
	m_factory.Set(name, value);
}

ApplicationContainer AidHelper::Install(Ptr<Node> node) const {
	return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer AidHelper::Install(std::string nodeName) const {
	Ptr <Node> node = Names::Find<Node>(nodeName);
	return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer AidHelper::Install(NodeContainer c) const {
	ApplicationContainer apps;
	for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
		apps.Add(InstallPriv(*i));
	}

	return apps;
}

Ptr<Application> AidHelper::InstallPriv(Ptr<Node> node) const {
	Ptr<Aid> app = m_factory.Create<Aid> ();
	node->AddApplication(app);

	return app;
}

}
