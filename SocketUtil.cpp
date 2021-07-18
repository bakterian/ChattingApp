#include "pch.h"
#include "SocketUtil.h"

bool operator==(const SOCKADDR_IN& a, const SOCKADDR_IN& b)
{
	return ((a.sin_addr.S_un.S_addr == b.sin_addr.S_un.S_addr)
		&& (a.sin_family == b.sin_family)
		&& (a.sin_port == b.sin_port));
}
