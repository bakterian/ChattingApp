#pragma once
#include <string>
#include "ChatDataAggregator.h"
#include "BaseChatCommand.h"
#include "Configuration.h"
#include <iomanip>

namespace chatManagement
{
	class MessageBatchCommand : public BaseChatCommand
	{
	public:
		explicit MessageBatchCommand(std::map<sysTime, userAndMsgPair> timeMessageMap) noexcept;

		explicit MessageBatchCommand() noexcept;

		friend bool operator==(const MessageBatchCommand& a, const MessageBatchCommand& b)
		{
			return (a.m_TimeMessageMap == b.m_TimeMessageMap);
		}

		friend std::istream& operator>>(std::istream& inputStream, chatManagement::MessageBatchCommand& batchMsgCmd)
		{
			std::string inputStr;

			std::string s;
			std::ostringstream os;
			inputStream >> os.rdbuf();
			inputStr = os.str();

			auto posMsgBatchBegin = inputStr.find(Config::RawDataMsgBatchBegin);
			auto posMsgBatchEnd = inputStr.find(Config::RawDataMsgBatchEnd);

			if ((posMsgBatchBegin == std::string::npos) || (posMsgBatchEnd == std::string::npos))
			{
				throw std::runtime_error("MessageBatchCommand: Problem with the <TAGS>!");
			}

			// remove XML Tags			
			inputStr.replace(posMsgBatchEnd, strlen(Config::RawDataMsgBatchEnd), "");
			inputStr.replace(posMsgBatchBegin, strlen(Config::RawDataMsgBatchBegin), "");

			std::istringstream inputSS(inputStr);

			std::string msgEntry {};
			std::string msgEntryElem {};

			batchMsgCmd.m_TimeMessageMap.clear();

			while (getline(inputSS, msgEntry, m_batchEntriesSeparator))
			{
				std::istringstream msgEntrySS(msgEntry);

				getline(msgEntrySS, msgEntryElem, m_timeMsgDetailsSepartor);
				auto timestamp = getSysTime(msgEntryElem);

				getline(msgEntrySS, msgEntryElem, m_timeMsgDetailsSepartor);
				auto user = msgEntryElem;

				getline(msgEntrySS, msgEntryElem, m_timeMsgDetailsSepartor);
				auto msg = msgEntryElem;

				auto userMsgPair = std::make_pair(user, msg);
				auto timeUserMsgPair = std::make_pair(timestamp, userMsgPair);

				batchMsgCmd.m_TimeMessageMap.insert(timeUserMsgPair);
			}

			//MessageBatchCommand parsedIntroCmd(userId);
			//batchMsgCmd = parsedIntroCmd;

			return inputStream;
		}

		friend std::ostream& operator<<(std::ostream& outputStr, chatManagement::MessageBatchCommand& batchMsgCmd)
		{
			std::string startXmlTag{ Config::RawDataMsgBatchBegin };
			std::string endXmlTag{ Config::RawDataMsgBatchEnd };
			outputStr << startXmlTag;

			for (auto& [t, m] : batchMsgCmd.m_TimeMessageMap)
			{
				auto& [user, msg] = m;
				outputStr << getTimeStr(t) << m_timeMsgDetailsSepartor;
				outputStr << user << m_timeMsgDetailsSepartor;
				outputStr << msg << m_batchEntriesSeparator;
			}
			
			outputStr << endXmlTag;

			return outputStr;
		}

		virtual std::map<sysTime, userAndMsgPair> getChatData() const override;

	private:

		static std::string getTimeStr(const sysTime& timestamp)
		{
			std::time_t timestamp_c = std::chrono::system_clock::to_time_t(timestamp);
			std::tm timestamp_tm{};

			errno_t errorCode = localtime_s(&timestamp_tm , &timestamp_c);
			if (errorCode)
			{
				throw std::runtime_error("MessageBatchCommand localtime_s: Issue when converting time to std::tm");
			}

			char ascTimeBuffer[200] {};

			errorCode = asctime_s(&ascTimeBuffer[0], sizeof(ascTimeBuffer) / sizeof(const char), &timestamp_tm);
			if (errorCode)
			{
				throw std::runtime_error("MessageBatchCommand asctime_s: Issue when converting std::time to chars");
			}

			return std::string{ ascTimeBuffer };
		}

		static sysTime getSysTime(const std::string& timestampStr)
		{
			std::tm timestamp_tm = {};
			std::istringstream ss(timestampStr);
			ss.imbue(std::locale());
			ss >> std::get_time(&timestamp_tm, "%a %b %e %H:%M:%S %Y\n");

			char ascTimeBuffer[200]{};

			auto errorCode = asctime_s(&ascTimeBuffer[0], sizeof(ascTimeBuffer) / sizeof(const char), &timestamp_tm);
			if (errorCode != 0)
			{
				throw std::runtime_error("MessageBatchCommand asctime_s: Issue when converting std::time to chars");
			}

			// Explictily setting the Daylight Saving property "tm_isdst" as the DST information is not persisted in the string timestamp
			// according to cppreference:
			// A negative value of time->tm_isdst causes mktime to attempt to determine if Daylight Saving Time was in effect.
			timestamp_tm.tm_isdst = -1;
			std::time_t timestamp_c = mktime(&timestamp_tm);

			auto timestamp = std::chrono::system_clock::from_time_t(timestamp_c);
			return timestamp;
		}

		std::map<sysTime, userAndMsgPair> m_TimeMessageMap;
		static constexpr char m_timeMsgDetailsSepartor{ ';' };
		static constexpr char m_batchEntriesSeparator{ '|' };
	};
}
