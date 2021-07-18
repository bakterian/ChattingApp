#include "pch.h"
#include "ChatMessagingMaster.h"
#include "MessageCommand.h"
#include "IntroductionCommand.h"
#include "MessageBatchCommand.h"

namespace chatManagement
{
	std::string ChatMessagingMaster::getChatIntroMsgStr(const std::string& userId)
	{
		IntroductionCommand chatCmd(userId);
		std::stringstream cmdSstream{};
		cmdSstream << chatCmd;
		return cmdSstream.str();
	}

	std::string ChatMessagingMaster::getChatMsgStr(const userAndMsgPair& userAndMsg)
	{
		auto [userId, userMessage] = userAndMsg;
		MessageCommand chatCmd(userId, userMessage);
		std::stringstream cmdSstream{};
		cmdSstream << chatCmd;
		
		return cmdSstream.str();
	}

	std::string ChatMessagingMaster::getChatBatchMsgStr(std::map<sysTime, userAndMsgPair>& msgCollection)
	{
		MessageBatchCommand chatCmd(msgCollection);
		std::stringstream cmdSstream{};
		cmdSstream << chatCmd;
		return cmdSstream.str();
	}

	std::optional<std::map<sysTime, userAndMsgPair>> ChatMessagingMaster::processRawData(const char* data, int amountOfData)
	{
		std::optional<std::map<sysTime, userAndMsgPair>> res = std::nullopt;

		if (auto cmdTypeOpt = determineCmdType(data, amountOfData))
		{
			res = extractChatData(cmdTypeOpt.value(), data, amountOfData);
		}

		return res;
	}

	std::optional<ChatCmdType> ChatMessagingMaster::determineCmdType(const char* data, int amountOfData)
	{
		std::string rawData(data, amountOfData);

		std::optional<ChatCmdType> res = std::nullopt;

		auto posUserSectionBegin = rawData.find(Config::RawDataUserIdSectionBegin);
		auto posUserSectionEnd = rawData.find(Config::RawDataUserIdSectionEnd);

		auto posMsgSectionBegin = rawData.find(Config::RawDataMsgSectionBegin);
		auto posMsgSectionEnd = rawData.find(Config::RawDataMsgSectionEnd);

		auto posMsgBatchSectionBegin = rawData.find(Config::RawDataMsgBatchBegin);
		auto posMsgBaychSectionEnd = rawData.find(Config::RawDataMsgBatchEnd);

		auto posIntroCmd = rawData.find(Config::IntroStr);

		if ((posUserSectionBegin == std::string::npos) &&
			(posUserSectionEnd == std::string::npos) &&
			(posMsgSectionBegin == std::string::npos) &&
			(posMsgSectionEnd == std::string::npos) &&
			(posMsgBatchSectionBegin != std::string::npos) &&
			(posMsgBaychSectionEnd != std::string::npos) &&
			(posIntroCmd == std::string::npos))
		{
			res = ChatCmdType::BatchMessage;
		}

		else if ((posUserSectionBegin != std::string::npos) &&
			(posUserSectionEnd != std::string::npos) &&
			(posMsgSectionBegin != std::string::npos) &&
			(posMsgSectionEnd != std::string::npos) &&
			(posMsgBatchSectionBegin == std::string::npos) &&
			(posMsgBaychSectionEnd == std::string::npos) &&
			(posIntroCmd == std::string::npos))
		{
			res = ChatCmdType::Message;
		}

		else if ((posUserSectionBegin != std::string::npos) &&
			(posUserSectionEnd != std::string::npos) &&
			(posMsgSectionBegin == std::string::npos) &&
			(posMsgSectionEnd == std::string::npos) &&
			(posMsgBatchSectionBegin == std::string::npos) &&
			(posMsgBaychSectionEnd == std::string::npos) &&
			(posIntroCmd != std::string::npos))
		{
			res = ChatCmdType::Introduction;
		}
		
		return res;	
	}

	std::map<sysTime, userAndMsgPair> ChatMessagingMaster::extractChatData(ChatCmdType cmdType, const char* data, int amountOfData)
	{
		std::string rawData(data, amountOfData);
		std::stringstream rawSstream{ rawData };
		std::map<sysTime, userAndMsgPair> res{};

		if (cmdType == ChatCmdType::Introduction)
		{
			IntroductionCommand cmd{};
			rawSstream >> cmd;
			res = cmd.getChatData();
		}

		else if (cmdType == ChatCmdType::Message)
		{
			MessageCommand cmd{};
			rawSstream >> cmd;
			res = cmd.getChatData();
		}

		else if (cmdType == ChatCmdType::BatchMessage)
		{
			MessageBatchCommand cmd{};
			rawSstream >> cmd;
			res = cmd.getChatData();
		}
		else
		{
			throw std::runtime_error("This cmd type is not yet supported.");
		}

		return res;
	}

}
