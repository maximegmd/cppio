#include "iocp.hpp"
#include "basic_overlapped.hpp"

namespace cppio::win32
{
    iocp::iocp()
    {
        m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    }

    iocp::~iocp()
    {
        CloseHandle(m_handle);
    }
    
    void iocp::run(const std::chrono::nanoseconds& wait_time) noexcept
    {
        DWORD bytes_transferred = 0;
        ULONG_PTR completion_key = 0;
        basic_overlapped* p_overlapped = nullptr;

        SetLastError(ERROR_SUCCESS);

        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(wait_time).count();

        auto result = GetQueuedCompletionStatus(m_handle, &bytes_transferred, &completion_key, (OVERLAPPED**)&p_overlapped, delay);

        if (result == FALSE && !p_overlapped)
            return;

        if (p_overlapped->type == basic_overlapped::Type::kTcpSocket)
        {
            p_overlapped->success = result == FALSE ? false : true;
            p_overlapped->task->wake();
            p_overlapped->bytes_transferred = bytes_transferred;
        }
    }

    bool iocp::add(handle_t handle) noexcept
    {
        return CreateIoCompletionPort((HANDLE)handle, m_handle, 0, 0) != INVALID_HANDLE_VALUE;
    }

    void iocp::notify_one() noexcept
    {
        PostQueuedCompletionStatus(m_handle, 0, 0, nullptr);
    }
}