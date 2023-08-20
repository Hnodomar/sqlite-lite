#include <iostream>
#include <string>

#include <unistd.h>

#include "cli/event_loop.hpp"

int cli_loop()
{
  cli::event_loop_t ev(10);

  std::string input;
  input.reserve(50);

  int i = 0;
  ev.add_read_fd(
    0,
    [&](const int32_t fd) {
      const auto r = read(fd, input.data(), 50);
      if (r) {
        ++i;
        std::cout << "str read: " << input << std::endl;
      }

      return r;
    }
  );

  while (i < 5) {
    ev.poll();
    std::string x;
    std::cin >> x;
    std::cout << "str: " << x << std::endl;
  }

  return 0;
}

int main()
{
  return cli_loop();
}
