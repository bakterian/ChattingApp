#pragma once

#include <string>
#include <winsock.h>

namespace connectionLogic
{
	class ISocketDataListener
	{
	public:
		// All of those functions called from Socket Server/Client Thread Context.
		// Therefore special care must be taken inside of the functions when accessing common thread resources.

		virtual void OnClientConnected(const SOCKADDR_IN& socketAddr) = 0;

		virtual void OnMessageReceived(const SOCKADDR_IN& socketAddr, const char* data, int amountOfData) = 0;
	};
}