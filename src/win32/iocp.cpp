#include "iocp.h"
#include "../async_tcp_socket.h"
#include "basic_overlapped.h"

namespace win32
{
	iocp::iocp()
	{
		m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 100);
	}

	iocp::~iocp()
	{
		CloseHandle(m_handle);
	}
	
	void iocp::run(const std::chrono::nanoseconds& wait_time)
	{
		DWORD bytes_transferred = 0;
		ULONG_PTR completion_key = 0;
		basic_overlapped* p_overlapped = nullptr;

		SetLastError(ERROR_SUCCESS);

		auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(wait_time).count();

		auto result = GetQueuedCompletionStatus(m_handle, &bytes_transferred, &completion_key, (OVERLAPPED**)&p_overlapped, delay);

		if (!p_overlapped || !result)
			return;

		if (p_overlapped->type == basic_overlapped::Type::kTcpSocket)
		{
			p_overlapped->success = true;
			p_overlapped->task->wake();
		}
	}

	void iocp::notify_one()
	{
		PostQueuedCompletionStatus(m_handle, 0, 0, nullptr);
	}
	void* iocp::get_handle() const
	{
		return m_handle;
	}
}