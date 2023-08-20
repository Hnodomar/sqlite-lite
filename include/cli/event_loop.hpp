#pragma once

#include <poll.h>

#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include <cassert>
#include <iostream>
#include <cstring>

#include "lib/exception.hpp"



namespace cli {

using fd_cb_t = std::function<bool(const int32_t)>;

class event_loop_t
{
public:

  event_loop_t(const int timeout);
  
  void add_read_fd(const int32_t fd, fd_cb_t fd_cb);
  
  void add_write_fd(const int32_t fd, fd_cb_t fd_cb);

  void remove_read_fd(const int32_t fd);

  void remove_write_fd(const int32_t fd);

  void poll();

private:

  enum class fd_mode_t : uint16_t
  {
    READ,
    WRITE
  };

  template<fd_mode_t fd_mode>
  constexpr auto mode_details()
    -> std::unordered_map<int32_t, fd_cb_t>&
  {
    if constexpr (fd_mode == fd_mode_t::READ) {
      return _read_callbacks;
    }

    return _write_callbacks;
  }

  template<fd_mode_t fd_mode>
  void add_impl(const int32_t fd, fd_cb_t fd_cb)
  {
    auto& cbs = mode_details<fd_mode>();

    const auto& [_, success] = cbs.emplace(
      fd,
      std::move(fd_cb)
    );

    if (!success) {
      RUNTIME_THROW(
        lib::error_code_t::INVALID_ARG,
        "Tried adding fd %i to event-loop cb handler, but it "
        "already existed",
        fd
      );
    }

    auto it = std::find_if(
      _fds.begin(),
      _fds.end(),
      [&](auto& pfd){return pfd.fd == fd;}
    );

    if (it != _fds.end()) {
      it->events |= static_cast<int>(fd_mode);
      return;
    }

    _fds.push_back({fd, static_cast<int>(fd_mode), 0});
  }

  template<fd_mode_t fd_mode>
  void remove_impl(const int32_t fd)
  {
    auto& cbs = mode_details<fd_mode>();

    auto it = cbs.find(fd);
    
    if (it == cbs.end()) {
      RUNTIME_THROW(
        lib::error_code_t::INVALID_ARG,
        "Tried removing fd %i from event-loop cb handler, "
        "but it didn't exist",
        fd
      );
    }
    
    _fds.erase(
      std::find_if(
        _fds.begin(),
        _fds.end(),
        [&](auto& pfd){return pfd.fd == fd;}
      )
    );
  }

  template<fd_mode_t fd_mode>
  bool exec_impl(const int32_t fd)
  {
    auto& cbs = mode_details<fd_mode>();

    auto it = cbs.find(fd);
    assert(it != cbs.end());

    return it->second(fd);
  }


  std::vector<pollfd>                  _fds;
  std::unordered_map<int32_t, fd_cb_t> _read_callbacks; 
  std::unordered_map<int32_t, fd_cb_t> _write_callbacks;
  const int32_t                        _timeout;

};

}

