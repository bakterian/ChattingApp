
#include "pch.h"
#include "ChatDataAggregator.h"


namespace chatManagement
{
	ChatDataAggregator::ChatDataAggregator() : 
		m_socketDataParser{},
		m_listenersCollection{},
		m_clientsSocketVsUserIdMap{},
		m_msgCollection{},
		m_localNick{"Unknown"}
	{
	}

	bool ChatDataAggregator::subscribeForLiveDataNotifications(chatManagement::ILiveDataListener* listener)
	{
		auto listenerAdded = false;

		if (std::find(m_listenersCollection.begin(), m_listenersCollection.end(), listener) == m_listenersCollection.end()) 
		{
			m_listenersCollection.push_back(listener);
			listenerAdded = true;
		}

		return listenerAdded;
	}

	void ChatDataAggregator::removeAllListeners()
	{
		m_listenersCollection.clear();
	}

	std::map<sysTime, userAndMsgPair> ChatDataAggregator::getChatEntries() const
	{
		return m_msgCollection;
	}

	// parses raw data, stores, notifies subscribed listeners
	bool ChatDataAggregator::processRawData(const SOCKADDR_IN& socketAddr, const char* data, int amountOfData)
	{
		bool isValidData = false;

		// TODO use dedicated deserilizers of the objects 
		    // 1) get to know which command it is a enum class can be used getCmdType(data, amountOfData)
		    // 2) depedning on type use a different parser and create cmd objects from the input
			

		if (auto userAndMsgPairOpt = m_socketDataParser.parseSocketData(data, amountOfData))
		{
			auto userAndMsg = userAndMsgPairOpt.value();
			auto [userId, msg] = userAndMsg;

			auto timestamp = std::chrono::system_clock::now();

			// user unkown add it to the clientCollection
			if(m_clientsSocketVsUserIdMap.find(userId) == m_clientsSocketVsUserIdMap.end())
			{
				auto newEntry = std::pair<std::string, SOCKADDR_IN>{ userId, socketAddr };
				m_clientsSocketVsUserIdMap.insert(newEntry);
			}

			insertNotifyIncomingMessage(timestamp, userAndMsg);

			isValidData = true;
		}

		return isValidData;
	}

	void ChatDataAggregator::updateUserIdsMap(const SOCKADDR_IN& socketAddr, const std::string& userId)
	{
		// user unkown add it to the clientCollection
		if (m_clientsSocketVsUserIdMap.find(userId) == m_clientsSocketVsUserIdMap.end())
		{
			auto newEntry = std::pair<std::string, SOCKADDR_IN>{ userId, socketAddr };
			m_clientsSocketVsUserIdMap.insert(newEntry);
		}
	}

	void ChatDataAggregator::processDataMul(const std::map<sysTime, userAndMsgPair>& rMsgEntires)
	{
		for (auto& [timestamp, userAndMsg] : rMsgEntires)
		{
			insertNotifyIncomingMessage(timestamp, userAndMsg);
		}
	}

	void ChatDataAggregator::processData(const sysTime& timestamp,const userAndMsgPair& userAndMsg)
	{
		insertNotifyIncomingMessage(timestamp, userAndMsg);
	}

	void ChatDataAggregator::processUserNameChange(const std::string& newNickname,const bool isLocalUser)
	{
		//TODO update internal strucutres, possibly force a update of the whole chat window

		if (isLocalUser)
		{
			m_localNick = newNickname;
			notifyLocalNicknameChange(newNickname);
		}
	}

	// informs if the message collection contains any entries
	bool ChatDataAggregator::isMsgCollectionDirty() const
	{
		return (m_msgCollection.size() > 0);
	}

	void ChatDataAggregator::initlizeChat()
	{
		notifyLocalNicknameChange(m_localNick);

		sendOutCompleteMsgHistory(m_msgCollection);
	}

	std::string ChatDataAggregator::getLocalNickname() const
	{
		return m_localNick;
	}

	void ChatDataAggregator::insertNotifyIncomingMessage(const sysTime& timestamp, const userAndMsgPair& userAndMsg)
	{
		auto newMsgEntry = std::pair<sysTime, userAndMsgPair>{ timestamp, userAndMsg };
		m_msgCollection.insert(newMsgEntry);

		// notify the listeners
		std::for_each(m_listenersCollection.cbegin(), m_listenersCollection.cend(),
			[&timestamp, &userAndMsg](auto listener) { listener->processIncomingMessage(timestamp, userAndMsg); });
	}

	void ChatDataAggregator::sendOutCompleteMsgHistory(const std::map<sysTime, userAndMsgPair>& msgCollection)
	{
		std::for_each(m_listenersCollection.cbegin(), m_listenersCollection.cend(),
			[&msgCollection](auto listener) { listener->fillChatWithStoredData(msgCollection); });
	}

	void ChatDataAggregator::notifyLocalNicknameChange(const std::string& newNickname)
	{
		// notify the listeners
		std::for_each(m_listenersCollection.cbegin(), m_listenersCollection.cend(),
			[&newNickname](auto listener) { listener->processUserNameChanged(newNickname); });
	}
}
