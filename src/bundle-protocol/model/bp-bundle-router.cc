/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <algorithm>
#include <sstream>

#include "ns3/log.h"
#include "ns3/uinteger.h"

#include "bp-neighbourhood-detection-agent.h"
#include "bp-bundle-router.h"
#include "bp-link-manager.h"
#include "ns3/trace-source-accessor.h"
#include "bp-rt-prophet-hello.h"

#include "bp-link.h"
#include "bp-contact.h"


namespace ns3 {
namespace bundleProtocol {

NS_LOG_COMPONENT_DEFINE ("BundleRouter");
NS_OBJECT_ENSURE_REGISTERED (BundleRouter);

TypeId
BundleRouter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::bundleProtocol::BundleRouter")
    .SetParent<Object> ()
    .AddAttribute ("BufferSize",
                   "Sets the size of the routers buffer",
                   UintegerValue (5000000),
                   MakeUintegerAccessor (&BundleRouter::m_maxBytes),
                   MakeUintegerChecker<uint32_t> ())
    .AddTraceSource ("Delete", "A data bundle have been deleted",
                     MakeTraceSourceAccessor (&BundleRouter::m_dataDeleteLogger))
    .AddTraceSource ("BundlesLeft", "Returns the bundles left in the message queue, when the router is closed",
                     MakeTraceSourceAccessor (&BundleRouter::m_bundlesLeftLogger))
    .AddTraceSource ("CancelledBundle", "A bundle has been cancelled",
                     MakeTraceSourceAccessor (&BundleRouter::m_cancelLogger))
     ;
  return tid;
}

BundleRouter::BundleRouter ()
  : m_nBytes (0),
    m_nBundles (0),
    m_isSending (false),
    m_bundleList (),
    m_routerSpecificList (),
    m_forwardLog (),
    m_linkManager (),
    m_node (),
    m_nda (),
    m_eid ()

{

      m_count_received_replicate_bundles = 0;
      m_count_received_bundles = 0;
      m_time_r = 0;

}

  
BundleRouter::~BundleRouter ()
{}

void
BundleRouter::DoDispose ()
{
   m_bundlesLeftLogger (m_node->GetId (),m_bundleList);
  m_bundleList.clear ();
  m_routerSpecificList.clear ();
  m_forwardLog.ClearLog ();
  m_linkManager = 0;  
  m_node = 0;
  m_nda = 0;
  m_sendCb = MakeNullCallback<void,Ptr<Link>,Ptr<Bundle> > ();
  m_cancelCb = MakeNullCallback<void, GlobalBundleIdentifier, BundleEndpointId> ();
  Object::DoDispose ();
}

void
BundleRouter::SetBundleSendCallback (Callback<void, Ptr<Link>, Ptr<Bundle> > sendCb)
{
  m_sendCb = sendCb;
}

void
BundleRouter::SetCancelTransmisisonCallback (Callback<void, GlobalBundleIdentifier, BundleEndpointId> cancelCb)
{
  m_cancelCb = cancelCb;
}

void
BundleRouter::SetLinkManager (Ptr<LinkManager> contactManager)
{
  m_linkManager = contactManager;
}

Ptr<LinkManager>
BundleRouter::GetLinkManager () const
{
  return m_linkManager;
}

void
BundleRouter::SetNeighbourhoodDetectionAgent (Ptr<NeighbourhoodDetectionAgent> nda)
{
  m_nda = nda;
}

Ptr<NeighbourhoodDetectionAgent>
BundleRouter::GetNeighbourhoodDetectionAgent () const
{
  return m_nda;
}

void
BundleRouter::Init ()
{
  NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  Callback<void, Ptr<Packet>, Address > tmp = MakeCallback (&LinkManager::DiscoveredLink, m_linkManager);
  m_nda->SetDiscoveredLinkCallback (tmp);

  m_nda->SetSendHelloCallback(MakeCallback(&BundleRouter::DoSendHello, this));
  m_nda->SetHandleHelloCallback(MakeCallback(&BundleRouter::DoHandleHello, this));

  m_linkManager->SetLinkAvailableCallback (MakeCallback (&BundleRouter::LinkDiscovered, this));
  m_linkManager->SetClosedLinkCallback (MakeCallback (&BundleRouter::LinkClosed, this));
  m_linkManager->SetCreateLinkCallback (MakeCallback (&BundleRouter::CreateLink, this));
  DoInit ();
}

void
BundleRouter::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ptr<Node>
BundleRouter::GetNode () const
{
  return m_node;
}

void
BundleRouter::SetBundleEndpointId (const BundleEndpointId& eid)
{
  m_eid = eid;
}

BundleEndpointId
BundleRouter::GetBundleEndpointId () const
{
  return m_eid;
}

bool
BundleRouter::IsEmpty ()
{
  return m_bundleList.empty ();
}

bool
BundleRouter::HasBundle (GlobalBundleIdentifier gbid)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");

        /* procura nas lista de agregados do nó um que tenha a identificação gbid */
        if (find_if(m_bundleList.begin(), m_bundleList.end(), MatchingGbid(gbid))
                        != m_bundleList.end()) {
                NS_LOG_DEBUG(" -> true");
                return true;
        } else {
                NS_LOG_DEBUG(" -> false");
                return HasRouterSpecificBundle(gbid);
        }
}

bool
BundleRouter::InsertBundle (Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  RemoveExpiredBundles ();
  m_forwardLog.RemoveExpiredEntries ();
  if (MakeRoomForBundle (bundle))
    {
      m_nBytes += bundle->GetSize ();
      m_nBundles++;
      DoInsert (bundle);
      return true;
    }
  return false;
}

void
BundleRouter::DeleteBundle (const GlobalBundleIdentifier& gbid, bool drop)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  Ptr<Bundle> bundle = GetBundle (gbid);
  //  cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ") " << "BundleRouter::DeleteBundle " << "(" << bundle->GetPayload ()->GetSize () << " : " << bundle->GetCreationTimestampSequence ()  << ")" << " drop = " << boolalpha << drop << endl;
  if (DoDelete (gbid,drop))
    {
      //NS_LOG_DEBUG ("size = " << bundle->GetPayload ()->GetSize () << endl << *bundle);
      m_dataDeleteLogger (bundle, drop);
      m_nBytes -= bundle->GetSize ();
      --m_nBundles;
    }
}

bool
BundleRouter::DoDelete (const GlobalBundleIdentifier& gbid, bool drop)
{
  //BundleList::iterator iter = remove_if (m_bundleList.begin (), m_bundleList.end (), MatchingGbid (gbid));
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  BundleList::iterator iter = find_if (m_bundleList.begin (), m_bundleList.end (), MatchingGbid (gbid));
  if (iter != m_bundleList.end ())
    {
      /*
      if (m_node->GetId () == 49)
        {
          cout << Simulator::Now ().GetSeconds () << " (" << m_node->GetId () << ")" << "CancelTransmission anropas av DoDelete" << endl;
        }
      */
      CancelTransmission (gbid); // FIXME hur ska jag göra med detta?
      m_bundleList.erase (iter);
      return true;
    }
  else 
    {
      return false;
    }
}

Ptr<Bundle>
BundleRouter::GetBundle (const GlobalBundleIdentifier& gbid)
{
  //NS_LOG_DEBUG ("(" << m_node->GetId () << ") " << "BundleRouter::GetBundle");
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  BundleList::iterator iter = find_if (m_bundleList.begin (), m_bundleList.end (), MatchingGbid (gbid));

  if (iter != m_bundleList.end ())
    {
      return *iter;
    }
  else
    {
      return GetRouterSpecificBundle (gbid);
    }
}

void
BundleRouter::LinkDiscovered (Ptr<Link> link)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoLinkDiscovered (link);
}

void
BundleRouter::LinkClosed (Ptr<Link> link)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoLinkClosed (link);
}

bool
BundleRouter::AcceptBundle (Ptr<Bundle> bundle, bool fromApplication)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoAcceptBundle (bundle, fromApplication);
}

bool
BundleRouter::AcceptCustody (Ptr<Bundle> bundle, CustodySignalReason& reason)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoAcceptCustody (bundle, reason);
}

bool
BundleRouter::DoAcceptCustody (Ptr<Bundle> bundle, CustodySignalReason& reason)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  reason = CUSTODY_NO_ADDITIONAL_INFORMATION;
  return false;
}

bool
BundleRouter::CanDeleteBundle (const GlobalBundleIdentifier& gbid)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoCanDeleteBundle (gbid);
}

bool
BundleRouter::IsForwardingContradicted (Ptr<Bundle> bundle, CustodySignalReason& reason)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoIsForwardingContradicted (bundle, reason);
}

bool
BundleRouter::IsForwardingFailure (const CustodySignalReason& reason)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoIsForwardingFailure (reason);
}

void
BundleRouter::CancelTransmission (const GlobalBundleIdentifier& gbid)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  Ptr<Bundle> bundle = GetBundle (gbid);
  if (bundle != 0)
    {  
      m_cancelLogger (bundle);
      DoCancelTransmission (bundle, 0);
      m_cancelCb (bundle, BundleEndpointId::GetAnyBundleEndpointId ());
    }
}

void
BundleRouter::CancelTransmission (Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  if (bundle == 0)
    {
      cout << "Doh!" << endl;
    }
  
  m_cancelLogger (bundle);
  DoCancelTransmission (bundle, 0);
  m_cancelCb (bundle, BundleEndpointId::GetAnyBundleEndpointId ());
}

void
BundleRouter::CancelTransmission (const GlobalBundleIdentifier& gbid, const BundleEndpointId& eid)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  Ptr<Bundle> bundle = GetBundle (gbid);
  Ptr<Link> link = m_linkManager->FindLink (link);
  if (bundle != 0 && link != 0)
    {  
      m_cancelLogger (bundle);
      DoCancelTransmission (bundle, link);
      m_cancelCb (bundle, eid);
    }
}

void
BundleRouter::CancelTransmission (Ptr<Bundle> bundle, Ptr<Link> link)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  if (bundle == 0)
    {
      cout << "Doh!" << endl;
    }
  
  m_cancelLogger (bundle);
  DoCancelTransmission (bundle, link);
  m_cancelCb (bundle, link->GetRemoteEndpointId ());
}

void
BundleRouter::TransmissionCancelled (const Address& address, const GlobalBundleIdentifier& gbid)
{
  debug << Simulator::Now ().GetSeconds () << " BundleRouter::TransmissionCancelled" << endl; // bundle " << bundle->GetPayload ()->GetSize () << " : " << bundle->GetCreationTimestampSequence ()  << ")" << endl;
  m_isSending = false;
  DoTransmissionCancelled (address, gbid);
}

uint8_t
BundleRouter::CalculateReplicationFactor (const BundlePriority& priority) const
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoCalculateReplicationFactor (priority);
}

void
BundleRouter::BundleSent (const Address& address, const GlobalBundleIdentifier& gbid, bool finalDelivery)
{
  debug << Simulator::Now ().GetSeconds () << " BundleRouter::BundleSent" << endl;
  m_isSending = false;
  DoBundleSent (address, gbid, finalDelivery);
}

void
BundleRouter::BundleTransmissionFailed (const Address& address, const GlobalBundleIdentifier& gbid)
{
  debug << Simulator::Now ().GetSeconds () << " BundleRouter::BundleTransmissionFailed" << endl; //for bundle " << bundle->GetPayload ()->GetSize () << " : " << bundle->GetCreationTimestampSequence ()  << ") to node (" << link->GetRemoteEndpointId ().GetId () << ")" << endl;
  m_isSending = false;
  DoBundleTransmissionFailed (address, gbid);
}

void
BundleRouter::HandleCustodyAcceptance (Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoHandleCustodyAcceptance (bundle);
}

void
BundleRouter::HandleCustodyRelease (Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoHandleCustodyRelease (bundle);
}

void
BundleRouter::HandleCustodyTransferFailure (const CustodySignal& signal, bool timeout)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoHandleCustodyTransferFailure (signal, timeout);
}

bool
BundleRouter::IsRouterSpecific (Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoIsRouterSpecific (bundle);
}

bool
BundleRouter::IsRouterSpecific (const BlockType& block)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoIsRouterSpecific (block);
}

void
BundleRouter::BundleReceived (Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoBundleReceived (bundle);
}

void
BundleRouter::BundleDelivered (Ptr<Bundle> bundle, bool fromAck)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  DoBundleDelivered (bundle, fromAck);
}

void
BundleRouter::SendBundle (Ptr<Link> link, Ptr<Bundle> bundle)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  stringstream ss;
  stringstream payload;
  payload << bundle->GetPayload ()->GetSize ();
  ss << "(" << m_node->GetId () << ") " << "BundleRouter::SendBundle " << "\t(" << m_node->GetId () << ")" << " >>>>>>>>>>>>>" << "(" << bundle->GetPayload ()->GetSize () << " : " << bundle->GetCreationTimestampSequence ()  << ")";
  for (uint32_t i = 0; i < (13 - payload.str ().size ()); ++i)
    {
      ss << ">";
    }
  ss << " (" << link->GetRemoteEndpointId ().GetSsp () << ")";
  if (!IsRouterSpecific (bundle))
    {
      NS_LOG_DEBUG (ss.str ());
      //cout << Simulator::Now ().GetSeconds () << " " << ss.str () << endl;
      //debug <<  Simulator::Now ().GetSeconds () << " " << ss.str () << endl;
    }
 
  debug <<  Simulator::Now ().GetSeconds () << " " << ss.str () << endl;
  m_isSending = true;
  Ptr<Bundle> send = DoSendBundle (link, bundle);
  Simulator::ScheduleNow (&BundleRouter::NotifySend, this, link, send);
}

void
BundleRouter::NotifySend (Ptr<Link> link, Ptr<Bundle> bundle)
{
  if (!m_sendCb.IsNull ()) {
          NS_LOG_DEBUG("(" << m_node->GetId() << ")");
    m_sendCb (link, bundle);
  }
}

bool
BundleRouter::IsSending () const
{
  return m_isSending;
}

void
BundleRouter::RemoveExpiredBundles ()
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  BundleList tmp;
  for (BundleList::iterator it = m_bundleList.begin(); it != m_bundleList.end(); ++it)
    {
      Ptr<Bundle> bundle = *it;
      if (TimeExpired (bundle)) 
        {
          tmp.push_back (bundle);
        }
    }
      
  for (BundleList::iterator iter = tmp.begin (); iter != tmp.end (); ++iter)
    {
          NS_LOG_DEBUG("--> Expired!!!");
      DeleteBundle (*iter, true);
    }
}

bool
BundleRouter::TimeExpired (Ptr<Bundle> bundle) const
{
  PrimaryBundleHeader header = bundle->GetPrimaryHeader ();
  Time lifetime = header.GetLifetime ();
  Time creationTime = header.GetCreationTimestamp ().GetTime ();
  return Simulator::Now () > lifetime + creationTime; 
}

uint32_t
BundleRouter::GetNBundles (void) const
{
  return m_nBundles;
}

uint32_t
BundleRouter::GetNBytes (void) const
{
  return m_nBytes;
}

uint32_t
BundleRouter::GetFreeBytes  (void) const
{
  return m_maxBytes - m_nBytes;
}

Ptr<Link>
BundleRouter::CreateLink (BundleEndpointId eid, Address address)
{
        NS_LOG_DEBUG("(" << m_node->GetId() << ")");
  return DoCreateLink (eid, address);
}

LinkBundleList
BundleRouter::GetAllDeliverableBundles ()
{
  Links links = m_linkManager->GetConnectedLinks ();
  LinkBundleList result;
  for (Links::iterator iter = links.begin (); iter != links.end (); ++iter)
    {
      LinkBundleList linkBundleList = GetAllBundlesForLink (*iter);
      result.insert (result.end (), linkBundleList.begin (), linkBundleList.end ());
    }
  return result;
}

LinkBundleList
BundleRouter:: GetAllBundlesForLink (Ptr<Link> link)
{
  LinkBundleList linkBundleList;
  for (BundleList::iterator iter = m_bundleList.begin (); iter != m_bundleList.end (); ++iter)
    {
      if ((*iter)->HasRetentionConstraint (RC_FORWARDING_PENDING))
        {
          if (link->GetRemoteEndpointId () == (*iter)->GetDestinationEndpoint ())
            {
              linkBundleList.push_back (LinkBundle (link, *iter));
            }
        }
    }
  return linkBundleList;
}

LinkBundleList
BundleRouter:: GetAllBundlesToAllLinks ()
{
  Links links = m_linkManager->GetConnectedLinks ();
  LinkBundleList linkBundleList;

  for (Links::iterator iter = links.begin (); iter != links.end (); ++iter)
    {
      for (BundleList::iterator it = m_bundleList.begin (); it != m_bundleList.end (); ++it)
        {
          linkBundleList.push_back (LinkBundle (*iter, *it));
        }
    }
  return linkBundleList;
}


bool
BundleRouter::DoCanDeleteBundle (const GlobalBundleIdentifier& gbid)
{
  return true;
}

bool
BundleRouter::DoIsForwardingContradicted (Ptr<Bundle> bundle, CustodySignalReason& reason)
{
  reason = CUSTODY_NO_ADDITIONAL_INFORMATION;
  return false;
}

bool
BundleRouter::DoIsForwardingFailure (const CustodySignalReason& reason)
{
  return false;
}

void
BundleRouter::DoHandleCustodyAcceptance (Ptr<Bundle> bundle)
{
}

void
BundleRouter::DoHandleCustodyRelease (Ptr<Bundle> bundle)
{ 
  bundle->RemoveRetentionConstraint (RC_CUSTODY_ACCEPTED);
}

void
BundleRouter::DoHandleCustodyTransferFailure (const CustodySignal& signal, bool timeout)
{}




/*Variaveis Globais Para Coletar Dados*/
int global_bundle_receveid = 0;
CountCopy count;
int m_count_expired_bundles = 0;
double copy_e = 0;

/*Joao*/
void BundleRouter::SetBundleReceived(std::string protocol,Ptr<Bundle> bundle)
{
  std::string protocol_r;
  std::string protocol_t;
  std::string protocol_efi2;

  /*Contabilize a Cópia Para Esse Bundle*/
  count.cc[bundle->GetGlobalId()]++;

  if(bundle->GetDestinationEndpoint().GetId() == m_node->GetId())
  {
      /*Arquivo de Numero de Bundles Que Chegaram Ao destino*/
      m_count_received_bundles++;
      NS_LOG_DEBUG("("<<m_node->GetId()<<") - #RB " << m_count_received_bundles);
      fstream file (protocol.c_str(),fstream::out);
      file<< m_count_received_bundles;
      file.flush();
      std::cout.flush();
      file.close();

      /*Arquivo da Soma do Atraso dos Bundles*/

      protocol_t = protocol + ".t";
      m_time_r = Simulator::Now().GetSeconds() - bundle->GetCreationTimestampTime();
      fstream file2 (protocol_t.c_str(),fstream::out | fstream::app);
      file2<< m_time_r<<"\n";
      file2.flush();
      std::cout.flush();
      file2.close();


      /*Arquivo da Metrica Eficiencia*/

      protocol_efi2 = protocol + ".efi2";
      fstream file3 (protocol_efi2.c_str(),fstream::out);

      copy_e +=  1.0 / count.cc[bundle->GetGlobalId()];

      file3<< copy_e<<"\n";
      file3.flush();
      std::cout.flush();
      file3.close();

   }








   /*Arquivo de Número de Cóṕias de Bundles*/
   protocol_r  = protocol + ".r";


   global_bundle_receveid++;
   fstream file3 (protocol_r.c_str(),fstream::out);
   file3<< global_bundle_receveid ;
   file3.flush();
   std::cout.flush();
   file3.close();

}



void BundleRouter::SetBundleExpired(std::string protocol)
{
        m_count_expired_bundles++;
        fstream file(protocol.c_str(),fstream::out);
        file<<m_count_expired_bundles;
        file.flush();
        std::cout.flush();
        file.close();
}
/*Joao*/

}} // namespace bundleProtocol, ns3
