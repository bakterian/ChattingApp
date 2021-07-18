#include "pch.h"
#include "IntroductionCommand.h"
#include "Configuration.h"

using namespace Config;

namespace chatManagement
{
	IntroductionCommand::IntroductionCommand(std::string userId) :
		BaseChatCommand(),
		m_userId(userId)
	{

	}

	IntroductionCommand::IntroductionCommand() :
		BaseChatCommand(),
		m_userId{}
	{

	}

	std::map<sysTime, userAndMsgPair> IntroductionCommand::getChatData() const
	{
		auto timestamp = std::chrono::system_clock::now();

		std::string emptyMsg{};
		std::pair<std::string,std::string> userMsgPair = std::make_pair(m_userId, emptyMsg);

		auto newMsgEntry = std::make_pair( timestamp, userMsgPair);

		return std::map<sysTime, userAndMsgPair> {newMsgEntry};
	}

} // chatManagement
