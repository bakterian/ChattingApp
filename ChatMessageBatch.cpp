#include "pch.h"
#include "ChatMessageBatch.h"

namespace chatManagement
{
	ChatMessageBatch::ChatMessageBatch(std::map<sysTime, userAndMsgPair>& rTimeMessageMap) noexcept :
		m_rTimeMessageMap(rTimeMessageMap)
	{

	}

	std::string ChatMessageBatch::getMsgBatchCmd()
	{
		std::string batch;

		for (const auto& [time, msg] : m_rTimeMessageMap)
		{
			
		}

		return batch;
	}
}