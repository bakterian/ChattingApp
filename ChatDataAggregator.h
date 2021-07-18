#pragma once
#include "ILiveDataListener.h"
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <map>
#include "SocketDataParser.h"
#include "NonCopyableMovable.h"

namespace chatManagement
{
	class ChatDataAggregator : private utils::NonCopyableMovable
	{
	public:

		ChatDataAggregator();

		bool subscribeForLiveDataNotifications(chatManagement::ILiveDataListener* listener);

		void removeAllListeners();

		// provide a collection of <message_sys_time, user_name, message> entities
		std::map<sysTime, userAndMsgPair> getChatEntries() const;

		// parses raw data, stores, notifies subscribed listeners
		bool processRawData(const SOCKADDR_IN& socketAddr, const char* data, int amountOfData);

		// extends the m_clientsSocketVsUserIdMap in case a new client was connected (only relevant for server)
		void updateUserIdsMap(const SOCKADDR_IN& socketAddr, const std::string& userId);

		// stores, notifies subscribed listeners
		void processDataMul(const std::map<sysTime, userAndMsgPair>& rMsgEntires);

		// stores, notifies subscribed listeners
		void processData(const sysTime& timestamp, const userAndMsgPair& userAndMsg);

		// stores names changes and propagates the info
		void processUserNameChange(const std::string& newNickname, const bool isLocalUser);

		// informs if the message collection contains any entries
		bool isMsgCollectionDirty() const;

		// send the stored chat data to subscribed components
		void initlizeChat();

		std::string getLocalNickname() const;
	private:

		// stores, notifies subscribed listeners about new incoming data
		void insertNotifyIncomingMessage(const sysTime& timestamp, const userAndMsgPair& userAndMsg);

		// sends out a notification with the complete message collection
		void sendOutCompleteMsgHistory(const std::map<sysTime, userAndMsgPair>& msgCollection);

		// notifies about a nickname of the current chat user instance
		void notifyLocalNicknameChange(const std::string& newNickname);

		// component which can decrept the raw data
		SocketDataParser m_socketDataParser;

		// collection of listeners
		std::vector<chatManagement::ILiveDataListener*> m_listenersCollection;

		// collection in form: [key - userID, value - socket]
		std::unordered_map<std::string, SOCKADDR_IN> m_clientsSocketVsUserIdMap;

		// collection in form: [key: sysTime, value: userId, Msg]
		std::map<sysTime, userAndMsgPair> m_msgCollection;

		// local users nickname
		std::string m_localNick;
	};
}
