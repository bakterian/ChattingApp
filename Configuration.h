#pragma once

namespace Config
{
	constexpr int ConnectionPort = 8123;

	constexpr int IncomingDataBufSize = 2048;

	constexpr int OutgoingDataBufSize = 1024;

	constexpr int HandshakeDataBufSize = 16;

	constexpr int HandshakeAckDataBufSize = 16;

	static constexpr char* InitHandshake = "<ELO320>";

	static constexpr char* InitHandshakeAck = "</ELO320>";

	static constexpr char* ChatMsgAck = "OK";

	constexpr const char* RawDataUserIdSectionBegin = "<USER>";

	constexpr const char* RawDataUserIdSectionEnd = "</USER>";

	constexpr const char* RawDataMsgSectionBegin = "<MSG>";

	constexpr const char* RawDataMsgSectionEnd = "</MSG>";

	constexpr const char* RawDataMsgBatchBegin = "<BATCH>";

	constexpr const char* RawDataMsgBatchEnd = "</BATCH>";

	constexpr const char* IntroStr = "Howdy!";

} // namespace Config

