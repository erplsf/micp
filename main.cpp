#include <atomic>
#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

// TODO: switch from read/write to recv/sendv
// TODO: use poll instead of select
// TODO: use atomic<bool> to stop the read server

using json = nlohmann::json;

using namespace std;

const char *SOCKET_PATH = "/tmp/mpv.socket";

void full_write(int sock, string data) {
  auto wrote = 0;
  auto rem = data.size();
  while (rem > 0) {
    auto chunk = write(sock, data.c_str() + wrote, rem);
    if (chunk == -1) {
      perror("error writing to a socket: ");
      exit(1);
    }
    wrote += chunk;
    rem -= chunk;
  }
}

void serv_read(int sock, atomic<bool> &run) {
  // TODO: implement
  cout << "started read loop\n";
  pollfd pfd{.fd = sock, .events = POLLIN};
  while (run.load()) {
    auto ready = poll(&pfd, 1, 1000);
    if (ready == -1) {
      perror("poll error: ");
      exit(1);
    } else if (ready == 1) {
      cout << "got data on socket to read!\n";
      const auto bufflen = 200;
      char buff[bufflen + 1];
      auto chunk = read(sock, buff, bufflen);

      cout << "read:\n";
      cout << buff;
    } else if (ready == 0) {
      cout << "timeout!\n";
    }
  }
}

int main(int argc, char *argv[]) {
  int sock = socket(AF_LOCAL, SOCK_STREAM,
                    0); // 0 is required for AF_LOCAL/AF_UNIX sockets
  if (sock == -1) {
    perror("couldn't create a socket: ");
    return -1;
  }

  sockaddr_un address;
  if (strlen(SOCKET_PATH) < sizeof(address.sun_path)) {
    strcpy(address.sun_path, SOCKET_PATH);
  } else {
    throw runtime_error("socket path is longer than max size!");
  }

  if (connect(sock, reinterpret_cast<sockaddr *>(&address), sizeof(address)) !=
      0) {
    perror("coudn't connect to a socket: ");
    return -1;
  };

  atomic<bool> run = true;

  thread t(serv_read, sock, ref(run));

  json command = {{"command", {"get_property", "playback-time"}}};
  string str = command.dump() + "\n";

  sleep(2);

  full_write(sock, str);
  cout << "finished write\n";

  // const size_t bufflen = 200;
  // char buff[bufflen + 1];
  // auto n = read(sock, buff, bufflen);

  // cout << string(buff);

  // while (true) {
  //   sleep(1000);
  // }

  sleep(2);

  run.store(false);

  t.join();

  return 0;
}
