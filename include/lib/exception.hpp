#pragma once

#include <exception>
#include <cstdio>
#include <array>
#include <string>

namespace lib {

  enum class error_code_t
  {
    IO_ERROR,
    OVERFLOW,
    UNDERFLOW,
    INVALID_ARG,
    UNEXPECTED_BEHAVIOUR
  };

  static inline std::string to_str(const error_code_t& ec)
  {
    switch (ec) {
      case error_code_t::IO_ERROR:
        return "[IO ERROR]: ";
      case error_code_t::OVERFLOW:
        return "[OVERFLOW]: ";
      case error_code_t::UNDERFLOW:
        return "[UNDERFLOW]: ";
      case error_code_t::INVALID_ARG:
        return "[INVALID_ARG]: ";
      case error_code_t::UNEXPECTED_BEHAVIOUR:
        return "[UNEXPECTED BEHAVIOUR]: ";
      default:
        return "[NO STR FOR EC]: ";
    }
  }

  struct error_t
  {
    static constexpr size_t ERROR_MSG_LEN = 256;

    template<typename... Args>
    static error_t make(const std::string fmt,
                        Args&&         ...args)
    {
      error_t error;

      std::snprintf(
        error.error_msg.data(),
        error.error_msg.size() - 2,
        fmt.c_str(),
        args...
      );
      
      error.error_msg[ERROR_MSG_LEN] = '\0';
      return error;
    }

    std::array<char, ERROR_MSG_LEN> error_msg;
  };

  struct error_t;
  
  class exception_t : public std::exception
  {
  public:
   
    exception_t(const error_t& error)
      : _error(std::move(error))
    {}

    error_t error() const
    {
      return _error;
    }

    const char* what() const noexcept override
    {
      return _error.error_msg.data();
    }

  private:

    error_t _error;
  };

  template<typename ...THROW_ARGS>
  void RUNTIME_THROW(const error_code_t& ec,
                     const std::string   fmt,
                     THROW_ARGS&&...     args)
  {
    std::string fmt_prepend(to_str(ec));
    fmt_prepend += fmt;

    throw exception_t(
      error_t::make(fmt_prepend, args...)
    );
  }
}



