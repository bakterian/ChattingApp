#pragma once
#include <vector>
#include <optional>
#include "NonCopyableMovable.h"

namespace chatManagement
{
	class ChatSessionManager : private utils::NonCopyableMovable
	{
	public:
		// CTOR
		ChatSessionManager();

		// handles client connection and informs if this a new client (returing true)
		bool clientConnected(const SOCKADDR_IN& socketAddr);

		// prepares a collection of the msg recpients (clients which are not the server and the client communicating)
		std::optional<std::vector<SOCKADDR_IN>> getClientsForMsgForward(const SOCKADDR_IN& receivedFromAddr);

		bool areThereMutlipleClients() const;

	private:
		std::vector<SOCKADDR_IN> m_ClientsCollection;
	};
} // namespace chatManagement
