#include "cli/event_loop.hpp"

using namespace cli;

event_loop_t::event_loop_t(const int timeout)
  : _timeout(timeout)
{}

void event_loop_t::add_read_fd(const int32_t               fd,
                               fd_cb_t                read_cb)
{
  add_impl<fd_mode_t::READ>(fd, std::move(read_cb));
}

void event_loop_t::add_write_fd(const int32_t fd,
                                fd_cb_t       write_cb)
{
  add_impl<fd_mode_t::WRITE>(fd, std::move(write_cb));
}

void event_loop_t::remove_read_fd(const int32_t fd)
{
  remove_impl<fd_mode_t::READ>(fd);
}

void event_loop_t::remove_write_fd(const int32_t fd)
{
  remove_impl<fd_mode_t::WRITE>(fd);
}

void event_loop_t::poll()
{
  const auto r = ::poll(_fds.data(), _fds.size(), 1); 

  if (r < 0) {
    std::cerr << strerror(errno);
    assert(false);
  }

  for (const auto& pfd : _fds) {
    
    const bool read_ready = pfd.events & static_cast<uint16_t>(fd_mode_t::READ);
    const bool write_ready = pfd.events & static_cast<uint16_t>(fd_mode_t::WRITE);

    //assert(read_ready || write_ready);

    if (read_ready && !exec_impl<fd_mode_t::READ>(pfd.fd)) {
      RUNTIME_THROW(
        lib::error_code_t::IO_ERROR,
        "Read callback for fd %i failed",
        pfd.fd
      );
    }
    
    if (write_ready && !exec_impl<fd_mode_t::WRITE>(pfd.fd)) {
      RUNTIME_THROW(
        lib::error_code_t::IO_ERROR,
        "Write callback for fd %i failed",
        pfd.fd
      );
    }
  }
}

