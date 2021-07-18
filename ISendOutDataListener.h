#pragma once
#include <chrono>

using sysTime = std::chrono::time_point<std::chrono::system_clock>;
using userAndMsgPair = std::pair<std::string, std::string>;

namespace chatManagement
{
	class ISendOutDataListener
	{
	public:
		virtual void processOutgoingMessage(const sysTime& timestamp, const userAndMsgPair& userAndMsg) = 0;
	};
}
