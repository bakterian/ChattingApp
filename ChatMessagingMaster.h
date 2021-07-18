#pragma once
#include <string>
#include "ILiveDataListener.h"
#include <map>
#include <optional>
#include "BaseChatCommand.h"

namespace chatManagement
{
	class ChatMessagingMaster
	{
	public:
		std::string getChatIntroMsgStr(const std::string& userId);
		std::string getChatMsgStr(const userAndMsgPair& userAndMsg);
		std::string getChatBatchMsgStr(std::map<sysTime, userAndMsgPair>& msgCollection);

		std::optional< std::map<sysTime, userAndMsgPair>> processRawData(const char* data, int amountOfData);

	private:
		std::optional<ChatCmdType> determineCmdType(const char* data, int amountOfData);
		std::map<sysTime, userAndMsgPair> extractChatData(ChatCmdType cmdType, const char* data, int amountOfData);

	};

} // namespace chatManagement

