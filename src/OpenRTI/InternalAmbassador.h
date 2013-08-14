/* -*-c++-*- OpenRTI - Copyright (C) 2009-2013 Mathias Froehlich
 *
 * This file is part of OpenRTI.
 *
 * OpenRTI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * OpenRTI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenRTI.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OpenRTI_InternalAmbassador_h
#define OpenRTI_InternalAmbassador_h

#include "AbstractConnect.h"
#include "Message.h"
#include "StringUtils.h"

namespace OpenRTI {

class InternalTimeManagement;
class Federate;

class OPENRTI_API InternalAmbassador {
public:
  InternalAmbassador();
  virtual ~InternalAmbassador();

  /// Handle the connection to the ServerNodes.
  /// FIXME in the long term, avoid the abstime here.
  /// Just resond with a ConnectRespnose message and wait for that in the ambassador.
  bool isConnected() const;
  void connect(const StringMap& parameterMap, const Clock& abstime);
  void connect(const StringStringListMap& parameterMap, const Clock& abstime);
  void disconnect();

  /// Send a message
  void send(const SharedPtr<const AbstractMessage>& message);

  /// Recieve a message and put it into the internal message processing,
  /// Returns true if there is an other pending message.
  bool receiveAndDispatchInternalMessage(const Clock& abstime);
  template<typename F>
  bool receiveAndDispatch(const Clock& abstime, F& functor)
  { return _receiveAndDispatch(abstime, FunctorMessageDispatcher<F>(functor)); }
  template<typename F>
  bool receiveAndDispatch(const Clock& abstime, const F& functor)
  { return _receiveAndDispatch(abstime, ConstFunctorMessageDispatcher<F>(functor)); }
  bool _receiveAndDispatch(const Clock& abstime, const AbstractMessageDispatcher& dispatcher);
  //// FIXME rethink this all?!

  /// Default internal message processing method
  void acceptInternalMessage(const AbstractMessage& message);
  /// Internal message processing methods
  void acceptInternalMessage(const ConnectionLostMessage& message);
  void acceptInternalMessage(const EnumerateFederationExecutionsRequestMessage& message);
  void acceptInternalMessage(const EnumerateFederationExecutionsResponseMessage& message);
  virtual void acceptInternalMessage(const InsertFederationExecutionMessage& message) = 0;
  void acceptInternalMessage(const ShutdownFederationExecutionMessage& message);
  void acceptInternalMessage(const EraseFederationExecutionMessage& message);
  void acceptInternalMessage(const ReleaseFederationHandleMessage& message);
  void acceptInternalMessage(const JoinFederationExecutionRequestMessage& message);
  void acceptInternalMessage(const JoinFederationExecutionResponseMessage& message);
  void acceptInternalMessage(const ResignFederationExecutionRequestMessage& message);
  void acceptInternalMessage(const JoinFederateNotifyMessage& message);
  void acceptInternalMessage(const ResignFederateNotifyMessage& message);
  void acceptInternalMessage(const RegisterFederationSynchronizationPointResponseMessage& message);
  void acceptInternalMessage(const AnnounceSynchronizationPointMessage& message);
  void acceptInternalMessage(const SynchronizationPointAchievedMessage& message);
  void acceptInternalMessage(const FederationSynchronizedMessage& message);
  void acceptInternalMessage(const EnableTimeRegulationRequestMessage& message);
  void acceptInternalMessage(const EnableTimeRegulationResponseMessage& message);
  void acceptInternalMessage(const DisableTimeRegulationRequestMessage& message);
  void acceptInternalMessage(const CommitLowerBoundTimeStampMessage& message);
  void acceptInternalMessage(const InsertRegionMessage& message);
  void acceptInternalMessage(const CommitRegionMessage& message);
  void acceptInternalMessage(const EraseRegionMessage& message);
  void acceptInternalMessage(const ChangeInteractionClassPublicationMessage& message);
  void acceptInternalMessage(const ChangeObjectClassPublicationMessage& message);
  void acceptInternalMessage(const ChangeInteractionClassSubscriptionMessage& message);
  void acceptInternalMessage(const ChangeObjectClassSubscriptionMessage& message);
  void acceptInternalMessage(const RegistrationForObjectClassMessage& message);
  void acceptInternalMessage(const TurnInteractionsOnMessage& message);
  void acceptInternalMessage(const InteractionMessage& message);
  void acceptInternalMessage(const TimeStampedInteractionMessage& message);
  void acceptInternalMessage(const ObjectInstanceHandlesRequestMessage& message);
  void acceptInternalMessage(const ObjectInstanceHandlesResponseMessage& message);
  void acceptInternalMessage(const ReleaseMultipleObjectInstanceNameHandlePairsMessage& message);
  void acceptInternalMessage(const ReserveObjectInstanceNameRequestMessage& message);
  void acceptInternalMessage(const ReserveObjectInstanceNameResponseMessage& message);
  void acceptInternalMessage(const ReserveMultipleObjectInstanceNameRequestMessage& message);
  void acceptInternalMessage(const ReserveMultipleObjectInstanceNameResponseMessage& message);
  void acceptInternalMessage(const InsertObjectInstanceMessage& message);
  void acceptInternalMessage(const DeleteObjectInstanceMessage& message);
  void acceptInternalMessage(const TimeStampedDeleteObjectInstanceMessage& message);
  void acceptInternalMessage(const AttributeUpdateMessage& message);
  void acceptInternalMessage(const TimeStampedAttributeUpdateMessage& message);
  void acceptInternalMessage(const RequestAttributeUpdateMessage& message);
  void acceptInternalMessage(const RequestClassAttributeUpdateMessage& message);


  std::pair<CreateFederationExecutionResponseType, std::string>
  dispatchWaitCreateFederationExecutionResponse(const Clock& abstime);
  DestroyFederationExecutionResponseType
  dispatchWaitDestroyFederationExecutionResponse(const Clock& abstime);

  JoinFederationExecutionResponseType dispatchWaitJoinFederationExecutionResponse(const Clock& abstime);
  bool dispatchWaitEraseFederationExecutionResponse(const Clock& abstime);

  /// Tries to reserve the object instance name in the federate and returns
  /// an object handle for this reserved name if successful.
  ObjectInstanceHandle dispatchWaitReserveObjectInstanceName(const Clock& abstime, const std::string& objectInstanceName);


  /// Get access to the federate
  virtual Federate* getFederate() = 0;

  /// To factor out the management stuff and make this adaptable to the implementation
  virtual InternalTimeManagement* getTimeManagement() = 0;


  ///////////////////////////////////////////////////////////////////
  // processing of callback messages - this is what the ambassador user sees
  typedef std::list<SharedPtr<const AbstractMessage> > MessageList2;
  void queueCallback(SharedPtr<const AbstractMessage> message)
  { _callbackMessageList.push_back(message); }
  void queueCallback(const AbstractMessage& message)
  { _callbackMessageList.push_back(&message); }
  // Append the whole list to the callback list /// FIXME throw out; needs to go into a timestamp message queue
  void queueCallbacks(MessageList2& messageList)
  { _callbackMessageList.splice(_callbackMessageList.end(), messageList); }
  void queueReceiveOrderCallback(const AbstractMessage& message);
  void queueTimeStampedMessage(const VariableLengthData& timeStamp, const AbstractMessage& message);
  void flushReceiveOrderMessages()
  { queueCallbacks(_receiveOrderMessages); }

  bool _dispatchCallbackMessage(AbstractMessageDispatcher& messageDispatcher)
  {
    if (_callbackMessageList.empty())
      return false;
    _callbackMessageList.front()->dispatch(messageDispatcher);
    _callbackMessageList.pop_front();
    return true;
  }
  bool _callbackMessageAvailable() const
  { return !_callbackMessageList.empty(); }

private:
  InternalAmbassador(const InternalAmbassador&);
  InternalAmbassador& operator=(const InternalAmbassador&);

  class _InternalMessageDispatchFunctor;
  class _CreateFederationExecutionFunctor;
  class _JoinFederationExecutionFunctor;
  class _DestroyFederationExecutionFunctor;
  class _EraseFederationExecutionFunctor;
  class _ReserveObjectInstanceNameFunctor;

  // The connect to the ServerNodes
  SharedPtr<AbstractConnect> _connect;

  // List for receive order messages already queued for callback
  MessageList2 _callbackMessageList;

  // FIXME organize queues differently should vanish
  MessageList2 _receiveOrderMessages;
};

} // namespace OpenRTI

#endif