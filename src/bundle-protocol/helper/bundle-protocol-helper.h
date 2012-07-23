/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */#ifndef BUNDLE_PROTOCOL_HELPER_H#define BUNDLE_PROTOCOL_HELPER_H#include "ns3/object-factory.h"#include "ns3/node-container.h"#include "ns3/node.h"#include "ns3/bp-convergence-layer-agent.h"#include "ns3/bp-bundle-protocol-agent.h"#include "ns3/bp-neighbourhood-detection-agent.h"#include "ns3/bp-bundle-router.h"#include "ns3/packet-socket-helper.h"#include "ns3/packet-socket-factory.h"//#include "ns3/bp-orwar-router.h"#include "ns3/bp-bundle-endpoint-id.h"#include "ns3/bp-link-manager.h"#include "ns3/bp-registration-factory.h"#include "ns3/wifi-net-device.h"namespace ns3 {class BundleProtocolHelper{ public:  BundleProtocolHelper ();  void SetConvergenceLayer (std::string tid,                            std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),                            std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),                            std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),                            std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),                            std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),                            std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),                            std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),                            std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());    void SetNeighbourhoodDetectionAgent (std::string tid,                      std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),                      std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),                      std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),                      std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),                      std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),                      std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),                      std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),                      std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());    void SetLinkManager (std::string tid,                          std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),                          std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),                          std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),                          std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),                          std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),                          std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),                          std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),                          std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());    void SetBundleRouter (std::string tid,                        std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),                        std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),                        std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),                        std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),                        std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),                        std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),                        std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),                        std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());  void SetBundleProtocolAgent (std::string tid,                               std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),                               std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),                               std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),                               std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),                               std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),                               std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),                               std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),                               std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());    void Install (NodeContainer container);  void Install (NodeContainer::Iterator begin, NodeContainer::Iterator end);  void Install (Ptr<Node> node);  void Start();private:  ObjectFactory m_claFactory;  ObjectFactory m_neighbourhoodDetectionAgentFactory;  ObjectFactory m_linkManagerFactory;  ObjectFactory m_bundleRouterFactory;  ObjectFactory m_bundleProtocolAgentFactory;  /* sergiosvieira */  Ptr<ns3::bundleProtocol::ConvergenceLayerAgent> cla;  Ptr<ns3::bundleProtocol::BundleProtocolAgent> bpa;  Ptr<ns3::bundleProtocol::NeighbourhoodDetectionAgent> nda;  Ptr<ns3::bundleProtocol::BundleRouter> br;  /* sergiosvieira */};} // namespace ns3#endif /* BUNDLE_PROTOCOL_HELPER_H */