#include "pch.h"
#include "MessageCommand.h"

namespace chatManagement
{
	MessageCommand::MessageCommand(std::string userId, std::string message) :
		BaseChatCommand(),
		m_userId(userId),
		m_message(message)
	{

	}

	MessageCommand::MessageCommand() :
		BaseChatCommand(),
		m_userId{},
		m_message{}
	{

	}

	std::map<sysTime, userAndMsgPair> MessageCommand::getChatData() const
	{
		auto timestamp = std::chrono::system_clock::now();

		std::pair<std::string, std::string> userMsgPair = std::make_pair(m_userId, m_message);

		auto newMsgEntry = std::make_pair(timestamp, userMsgPair);

		return std::map<sysTime, userAndMsgPair> {newMsgEntry};
	}

} // namespace chatManagement
