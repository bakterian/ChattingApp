#pragma once
#include <string>
#include <stdexcept>
#include <istream>
#include "Configuration.h"
#include <map>
#include "ILiveDataListener.h"

namespace chatManagement
{
	enum class ChatCmdType
	{
		Introduction,
		Message,
		BatchMessage
	};

	class BaseChatCommand
	{
	public:
		explicit BaseChatCommand();

		virtual std::map<sysTime, userAndMsgPair> getChatData() const = 0;
	};
}
