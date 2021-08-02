/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/abort.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "queue.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Queue");

NS_OBJECT_ENSURE_REGISTERED (QueueBase);
NS_OBJECT_TEMPLATE_CLASS_DEFINE (Queue,Packet);
NS_OBJECT_TEMPLATE_CLASS_DEFINE (Queue,QueueDiscItem);

TypeId
QueueBase::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QueueBase")
    .SetParent<Object> ()
    .SetGroupName ("Network")
    .AddAttribute ("MaxSize",
                   "The max queue size",
                   QueueSizeValue (QueueSize ("100p")),
                   MakeQueueSizeAccessor (&QueueBase::SetMaxSize,
                                          &QueueBase::GetMaxSize),
                   MakeQueueSizeChecker ())
    .AddTraceSource ("PacketsInQueue",
                     "Number of packets currently stored in the queue",
                     MakeTraceSourceAccessor (&QueueBase::m_nPackets),
                     "ns3::TracedValueCallback::Uint32")
    .AddTraceSource ("BytesInQueue",
                     "Number of bytes currently stored in the queue",
                     MakeTraceSourceAccessor (&QueueBase::m_nBytes),
                     "ns3::TracedValueCallback::Uint32")
    .AddAttribute ("Target",
                   "The CoDel algorithm target queue delay",
                   StringValue ("5ms"),
                   MakeTimeAccessor (&QueueBase::m_target),
                   MakeTimeChecker ())
  ;
  return tid;
}

QueueBase::QueueBase () :
  m_nBytes (0),
  m_nTotalReceivedBytes (0),
  m_nPackets (0),
  m_nTotalReceivedPackets (0),
  m_nTotalDroppedBytes (0),
  m_nTotalDroppedBytesBeforeEnqueue (0),
  m_nTotalDroppedBytesAfterDequeue (0),
  m_nTotalDroppedPackets (0),
  m_nTotalDroppedPacketsBeforeEnqueue (0),
  m_nTotalDroppedPacketsAfterDequeue (0)
{
  NS_LOG_FUNCTION (this);
}

QueueBase::~QueueBase ()
{
  NS_LOG_FUNCTION (this);
}

void
QueueBase::AppendItemTypeIfNotPresent (std::string& typeId, const std::string& itemType)
{
  if (typeId.back () != '>')
    {
      typeId.append ("<" + itemType + ">");
    }
}

bool
QueueBase::IsEmpty (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << (m_nPackets.Get () == 0));
  return m_nPackets.Get () == 0;
}

uint32_t
QueueBase::GetNPackets (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nPackets);
  return m_nPackets;
}

uint32_t
QueueBase::GetNBytes (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (" returns " << m_nBytes);
 return m_nBytes;
  
 //return flag;
}




QueueSize
QueueBase::GetCurrentSize (void) const
{
  NS_LOG_FUNCTION (this);

  if (m_maxSize.GetUnit () == QueueSizeUnit::PACKETS)
    {
      return QueueSize (QueueSizeUnit::PACKETS, m_nPackets);
    }
  if (m_maxSize.GetUnit () == QueueSizeUnit::BYTES)
    {
      return QueueSize (QueueSizeUnit::BYTES, m_nBytes);
    }
  NS_ABORT_MSG ("Unknown queue size unit");
}

uint32_t
QueueBase::GetTotalReceivedBytes (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalReceivedBytes);
  return m_nTotalReceivedBytes;
}

uint32_t
QueueBase::GetTotalReceivedPackets (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalReceivedPackets);
  return m_nTotalReceivedPackets;
}

uint32_t
QueueBase:: GetTotalDroppedBytes (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalDroppedBytes);
  return m_nTotalDroppedBytes;
}

uint32_t
QueueBase:: GetTotalDroppedBytesBeforeEnqueue (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalDroppedBytesBeforeEnqueue);
  return m_nTotalDroppedBytesBeforeEnqueue;
}

uint32_t
QueueBase:: GetTotalDroppedBytesAfterDequeue (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalDroppedBytesAfterDequeue);
  return m_nTotalDroppedBytesAfterDequeue;
}

uint32_t
QueueBase::GetTotalDroppedPackets (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalDroppedPackets);
  return m_nTotalDroppedPackets;
}

uint32_t
QueueBase::GetTotalDroppedPacketsBeforeEnqueue (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalDroppedPacketsBeforeEnqueue);
  return m_nTotalDroppedPacketsBeforeEnqueue;
}

uint32_t
QueueBase::GetTotalDroppedPacketsAfterDequeue (void) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("returns " << m_nTotalDroppedPacketsAfterDequeue);
  return m_nTotalDroppedPacketsAfterDequeue;
}

void
QueueBase::ResetStatistics (void)
{
  NS_LOG_FUNCTION (this);
  m_nTotalReceivedBytes = 0;
  m_nTotalReceivedPackets = 0;
  m_nTotalDroppedBytes = 0;
  m_nTotalDroppedBytesBeforeEnqueue = 0;
  m_nTotalDroppedBytesAfterDequeue = 0;
  m_nTotalDroppedPackets = 0;
  m_nTotalDroppedPacketsBeforeEnqueue = 0;
  m_nTotalDroppedPacketsAfterDequeue = 0;
}

void
QueueBase::SetMaxSize (QueueSize size)
{
  NS_LOG_FUNCTION (this << size);

  // do nothing if the size is null
  if (!size.GetValue ())
    {
      return;
    }

  m_maxSize = size;

  NS_ABORT_MSG_IF (size < GetCurrentSize (),
                   "The new maximum queue size cannot be less than the current size");
}

QueueSize
QueueBase::GetMaxSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_maxSize;
}

CoDelTimestampTag::CoDelTimestampTag ()
  : m_creationTime (Simulator::Now ().GetTimeStep ())
{
}

TypeId
CoDelTimestampTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CoDelTimestampTag::GetSerializedSize (void) const
{
  return 8;
}
void
CoDelTimestampTag::Serialize (TagBuffer i) const
{
  i.WriteU64 (m_creationTime);
}
void
CoDelTimestampTag::Deserialize (TagBuffer i)
{
  m_creationTime = i.ReadU64 ();
}
void
CoDelTimestampTag::Print (std::ostream &os) const
{
  os << "CreationTime=" << m_creationTime;
}
Time
CoDelTimestampTag::GetTxTime (void) const
{
  return TimeStep (m_creationTime);
}

TypeId 
MyTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyTag")
    .SetParent<Tag> ()
    .AddConstructor<MyTag> ()
    .AddAttribute ("SimpleValue",
                   "A simple value",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&MyTag::GetSimpleValue),
                   MakeUintegerChecker<uint8_t> ())
  ;
  return tid;
}
TypeId 
MyTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
MyTag::GetSerializedSize (void) const
{
  return 1;
}
void 
MyTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_simpleValue);
}
void 
MyTag::Deserialize (TagBuffer i)
{
  m_simpleValue = i.ReadU8 ();
}
void 
MyTag::Print (std::ostream &os) const
{
  os << "v=" << (uint8_t)m_simpleValue;
}
void 
MyTag::SetSimpleValue (uint8_t value)
{

 b= value;
std::cout << " MyTag::SetSimpleValue value = " << b << "\n";
  m_simpleValue = value;
}
int
MyTag::GetSimpleValue (void) const
{
  return b;
}


} // namespace ns3
