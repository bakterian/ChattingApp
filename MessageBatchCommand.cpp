#include "pch.h"
#include "MessageBatchCommand.h"


namespace chatManagement
{
	MessageBatchCommand::MessageBatchCommand(std::map<sysTime, userAndMsgPair> timeMessageMap) noexcept :
		m_TimeMessageMap(timeMessageMap)
	{

	}

	MessageBatchCommand::MessageBatchCommand() noexcept :
		m_TimeMessageMap{}
	{

	}

	std::map<sysTime, userAndMsgPair> MessageBatchCommand::getChatData() const
	{
		return m_TimeMessageMap;
	}
}
