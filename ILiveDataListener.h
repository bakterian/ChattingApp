#pragma once

#include <chrono>
#include <map>

using sysTime = std::chrono::time_point<std::chrono::system_clock>;
using userAndMsgPair = std::pair<std::string, std::string>;

namespace chatManagement
{
	class ILiveDataListener
	{
	public:
		virtual void processIncomingMessage(const sysTime& timestamp, const userAndMsgPair& userAndMsg) = 0;

		virtual void fillChatWithStoredData(const std::map<sysTime, userAndMsgPair>& m_msgCollection) = 0;

		virtual void processUserNameChanged(const std::string& newUserNickname) = 0;
	};
}