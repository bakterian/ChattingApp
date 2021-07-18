#pragma once
#include <string>
#include "NonCopyableMovable.h"
#include "ISocketDataListener.h"
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <vector>
#include <condition_variable>

namespace connectionLogic
{
	enum class SocketConType
	{
		Undetermined,
		Client,
		Server
	};

	class SocketConnector : private utils::NonCopyableMovable
	{
	public:
		SocketConnector();

		~SocketConnector();

		bool init();

		bool start();

		void stop();

		void sendOnSocket(std::string& text, const std::vector<SOCKADDR_IN>& recepients);

		void sendOnSocketToAll(std::string& text);

		bool isConnectionAlive() const;

		void subscribeForNotifications(ISocketDataListener* listener);

		SocketConType getConnectionType() const;

	private:

		inline bool initWinsock();

		inline bool createSocket();

		inline bool attachSocketToPort();

		void continuesListen();

		void continuesSend();

		void cleanUp();

		bool doHandshake();

		bool doHandshakeSendRecv();

		bool isHandshake(std::string potencialHandshake);

		bool isHandshakeAck(std::string potencialHandshakeAck);

		int m_socketServDescriptor;
		int m_clientSocket;
		std::thread* m_pReceiveThread;
		std::thread* m_pSenderThread;
		std::atomic_bool m_ListenError;
		std::atomic_bool m_SendError;
		std::atomic_int  m_wsaLastError;
		SOCKADDR_IN m_otherSocketAddress;
		SOCKADDR_IN m_commonServerAddress;
		ISocketDataListener* m_pSocketDataListener;
		SocketConType		 m_socketType;
		std::mutex				m_sendingMutex;
		std::condition_variable m_sendingCondtion;
		std::mutex				m_clientAddrMutex;
		std::vector<SOCKADDR_IN> m_clientAddrCollection;
	};

}
