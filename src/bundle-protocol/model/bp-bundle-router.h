/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#ifndef BP_BUNDLE_ROUTER_H
#define BP_BUNDLE_ROUTER_H

#include <deque>
#include <fstream>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/simulator.h"
#include "ns3/callback.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/traced-callback.h"

#include "bp-bundle.h"
#include "bp-bundle-endpoint-id.h"
#include "bp-global-bundle-identifier.h"
#include "bp-custody-signal.h"
#include "bp-forwarding-log.h"

using namespace std;

namespace ns3 {
namespace bundleProtocol {

class LinkManager;
class NeighbourhoodDetectionAgent;
class Link;
class Contact;

typedef deque<Ptr<Bundle> > BundleList;

struct CountCopy{
      double cc[300];
      CountCopy()
      {
        for (int i = 0; i < 300; i++)
          {
            cc[i] = 0;
          }
      }
};


struct LinkBundle {
        Ptr<Link> m_link;
        Ptr<Bundle> m_bundle;

        LinkBundle() :
                m_link(0), m_bundle(0) {
        }

        LinkBundle(Ptr<Link> link, Ptr<Bundle> bundle) :
                m_link(link), m_bundle(bundle) {
        }

        Ptr<Link> GetLink() const {
                return m_link;
        }

        Ptr<Bundle> GetBundle() const {
                return m_bundle;
        }

        bool IsNull() {
                return (m_link == 0) || (m_bundle == 0);
        }
};

typedef deque<LinkBundle> LinkBundleList;

/**
 * \ingroup bundleRouter
 *
 * \brief A abstract class defining the API and implementing some general
 * functionality to be used by implementing classes.
 *
 *
 */

class BundleRouter: public Object {
public:
        static TypeId GetTypeId(void);

        BundleRouter();
        virtual ~BundleRouter();

        void SetBundleSendCallback(Callback<void, Ptr<Link> , Ptr<Bundle> > sendCb);
        void SetCancelTransmisisonCallback(Callback<void, GlobalBundleIdentifier,
                        BundleEndpointId> cancelCb);

        void Init();

        void SetNeighbourhoodDetectionAgent(Ptr<NeighbourhoodDetectionAgent> nda);
        Ptr<NeighbourhoodDetectionAgent> GetNeighbourhoodDetectionAgent() const;

        void SetLinkManager(Ptr<LinkManager> contactManager);
        Ptr<LinkManager> GetLinkManager() const;

        void SetNode(Ptr<Node> node);
        Ptr<Node> GetNode() const;

        void SetBundleEndpointId(const BundleEndpointId& eid);
        BundleEndpointId GetBundleEndpointId() const;

        // Bundle buffer management functions
        bool IsEmpty();
        bool HasBundle(GlobalBundleIdentifier gbid);
        bool InsertBundle(Ptr<Bundle> bundle);
        void DeleteBundle(const GlobalBundleIdentifier& gbid, bool drop);
        Ptr<Bundle> GetBundle(const GlobalBundleIdentifier& gbid);

        void LinkDiscovered(Ptr<Link> link);
        void LinkClosed(Ptr<Link> link);

        bool AcceptBundle(Ptr<Bundle> bundle, bool fromApplication = false);
        bool AcceptCustody(Ptr<Bundle> bundle, CustodySignalReason& reason);
        bool CanDeleteBundle(const GlobalBundleIdentifier& gbid);
        bool IsForwardingContradicted(Ptr<Bundle> bundle,
                        CustodySignalReason& reason);
        bool IsForwardingFailure(const CustodySignalReason& reason);

        void CancelTransmission(const GlobalBundleIdentifier& gbid);
        void CancelTransmission(const GlobalBundleIdentifier& gbid,
                        const BundleEndpointId& eid);
        void TransmissionCancelled(const Address& address,
                        const GlobalBundleIdentifier& gbid);
        uint8_t CalculateReplicationFactor(const BundlePriority& priority) const;
        void BundleSent(const Address& address, const GlobalBundleIdentifier& gbid,
                        bool finalDelivery);
        void BundleTransmissionFailed(const Address&,
                        const GlobalBundleIdentifier& gbid);
        void BundleReceived(Ptr<Bundle> bundle);
        void BundleDelivered(Ptr<Bundle> bundle, bool fromAck);
        bool IsSending() const;

        void HandleCustodyAcceptance(Ptr<Bundle> bundle);
        void HandleCustodyRelease(Ptr<Bundle> bundle);
        void HandleCustodyTransferFailure(const CustodySignal& signal, bool timeout);

        bool IsRouterSpecific(Ptr<Bundle> bundle);
        bool IsRouterSpecific(const BlockType& block);

        virtual void ReceiveRouterSpecific(Ptr<Bundle> bundle) = 0;
        virtual void SentRouterSpecific(Ptr<Link> link, const GlobalBundleIdentifier& gbid) = 0;

        uint32_t GetNBundles(void) const;
        uint32_t GetNBytes(void) const;
        uint32_t GetFreeBytes(void) const;

        Ptr<Link> CreateLink(BundleEndpointId eid, Address address);

        bool HasForwardLogEntry(ForwardLogEntry entry) const;
        bool HaForwardLogsEntry(Ptr<Bundle> bundle, BundleEndpointId forwardedTo) const;
        bool HasForwardLogEntry(GlobalBundleIdentifier gbid,
                        BundleEndpointId forwardedTo, Time ttl) const;

        ForwardLogEntries GetAllForwardLogEntries() const;
        ForwardLogEntries GetForwardLogEntries(BundleEndpointId forwardedTo) const;
        ForwardLogEntries GetForwardLogEntries(GlobalBundleIdentifier gbid) const;

        virtual void RemoveExpiredBundles();
        /* sergiosvieira */
        virtual void DoSendHello(Ptr<Socket> socket, BundleEndpointId eid) {};
        virtual void DoHandleHello(Ptr<Socket> socket) {};

        /* sergiosvieira */
private:
        void NotifySend(Ptr<Link> link, Ptr<Bundle> bundle);

protected:
        ofstream debug; // FIXME Remove this
        virtual void DoDispose();
        /*Joao*/
        uint32_t m_count_received_bundles;
        uint32_t m_count_received_replicate_bundles;
        uint64_t m_time_r;
        void SetBundleReceived(std::string protocol,Ptr<Bundle> bundle);
        void SetBundleExpired(std::string protocol);
        /*Joao*/
        /* pure virtual methods */

        virtual void DoInit() = 0;
        virtual void SendRouterSpecific(Ptr<Link> link, Ptr<Bundle> bundle) = 0;
        virtual void AddRouterSpecificBundle(Ptr<Bundle> bundle) = 0;
        virtual void RemoveRouterSpecificBundle(const GlobalBundleIdentifier& gbid,
                        uint8_t reason) = 0;
        virtual bool HasRouterSpecificBundle(const GlobalBundleIdentifier& gbid) = 0;
        virtual Ptr<Bundle> GetRouterSpecificBundle(
                        const GlobalBundleIdentifier& gbid) = 0;
        virtual bool DoIsRouterSpecific(Ptr<Bundle> bundle) = 0;
        virtual bool DoIsRouterSpecific(const BlockType& block) = 0;
        virtual void DoBundleReceived(Ptr<Bundle> bundle) = 0;
        virtual void DoBundleDelivered(Ptr<Bundle> bundle, bool fromAck) = 0;
        virtual Ptr<Bundle> DoSendBundle(Ptr<Link> link, Ptr<Bundle> bundle) = 0;
        virtual void DoLinkDiscovered(Ptr<Link> link) = 0;
        virtual void DoLinkClosed(Ptr<Link> link) = 0;
        virtual bool DoAcceptBundle(Ptr<Bundle> bundle, bool fromApplication) = 0;
        virtual bool DoAcceptCustody(Ptr<Bundle> bundle,
                        CustodySignalReason& reason);
        virtual bool DoCanDeleteBundle(const GlobalBundleIdentifier& gbid);
        virtual void DoInsert(Ptr<Bundle> bundle) = 0;
        virtual bool MakeRoomForBundle(Ptr<Bundle> bundle) = 0;
        virtual void DoCancelTransmission(Ptr<Bundle> bundle, Ptr<Link> link) = 0;
        virtual void DoTransmissionCancelled(const Address &,
                        const GlobalBundleIdentifier& gbid) = 0;
        virtual uint8_t
                        DoCalculateReplicationFactor(const BundlePriority& priority) const = 0;
        virtual void DoBundleSent(const Address& address,
                        const GlobalBundleIdentifier& gbid, bool finalDelivery) = 0;
        virtual void DoBundleTransmissionFailed(const Address& address,
                        const GlobalBundleIdentifier& gbid) = 0;
        virtual Ptr<Link> DoCreateLink(const BundleEndpointId& eid,
                        const Address& address) = 0;

        /* end of pure virtual methods */

        virtual void SendBundle(Ptr<Link> link, Ptr<Bundle> bundle);
        void CancelTransmission(Ptr<Bundle> bundle);
        void CancelTransmission(Ptr<Bundle> bundle, Ptr<Link> link);

        virtual bool DoDelete(const GlobalBundleIdentifier& gbid, bool drop);
        virtual bool DoIsForwardingContradicted(Ptr<Bundle> bundle,
                        CustodySignalReason& reason);
        virtual bool DoIsForwardingFailure(const CustodySignalReason& reason);

        virtual void DoHandleCustodyAcceptance(Ptr<Bundle> bundle);
        virtual void DoHandleCustodyRelease(Ptr<Bundle> bundle);
        virtual void DoHandleCustodyTransferFailure(const CustodySignal& signal,
                        bool timeout);
        virtual bool TimeExpired(Ptr<Bundle> bundle) const;

        virtual LinkBundleList GetAllDeliverableBundles();
        virtual LinkBundleList GetAllBundlesForLink(Ptr<Link> link);
        virtual LinkBundleList GetAllBundlesToAllLinks();

        struct MatchingGbid: public unary_function<Ptr<Bundle> , bool> {
                GlobalBundleIdentifier m_gbid;

                MatchingGbid(GlobalBundleIdentifier gbid) :
                        m_gbid(gbid) {
                }

                bool operator ()(Ptr<Bundle> bundle) {
                        return m_gbid == bundle->GetBundleId();
                }
        };

protected:
        uint32_t m_maxBytes;
        uint32_t m_nBytes;
        uint32_t m_nBundles;
        bool m_isSending;

        BundleList m_bundleList;
        BundleList m_routerSpecificList;
        ForwardLog m_forwardLog;
        Ptr<LinkManager> m_linkManager;
        Ptr<Node> m_node;
        Ptr<NeighbourhoodDetectionAgent> m_nda;
        BundleEndpointId m_eid;

        Callback<void, Ptr<Link> , Ptr<Bundle> > m_sendCb;
        Callback<void, GlobalBundleIdentifier, BundleEndpointId> m_cancelCb;
        TracedCallback<Ptr<const Bundle> , bool> m_dataDeleteLogger;
        TracedCallback<Ptr<const Bundle> > m_cancelLogger;
        TracedCallback<uint32_t, BundleList> m_bundlesLeftLogger;
};

}
} // namespace bundleProtocol, ns3

#endif /* BP_BUNDLE_ROUTER_H */
