#include <iostream>
#include <vector>
#include<thread>
#include<string>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <chrono>
#include <functional>
using namespace std; // bad practice

static void getStatus(pid_t pid) {
  char procpath[100];
  char buf[100];
  int pfd;
  char Name[100];
  FILE * proc;

  sprintf(procpath, "/proc/%d/status", pid);
  proc = fopen(procpath, "r");
  if (proc) {
    fgets(buf, 256, proc);
    sscanf(buf, "Name:\t%s", Name);   // c-style
  }
  cout << Name << endl;
}

void iterateProcesses(void) {
  struct dirent* dirent;         // c-style
  DIR* dir;
  int pid;

  if (!(dir = opendir("/proc"))) {
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  while (dirent = readdir(dir)) {
    if (isdigit( * dirent -> d_name)) {
      pid = atoi(dirent -> d_name);
      getStatus(pid);
    }
  }
  closedir(dir);
}

void sigHandler(int sig) {
  printf("you hit Ctrl-C\n");
  exit(0);
}

void run(std:: function < void(void) > f, int duration) {
  std::thread([f, duration]() {
    while (true) {
      f();
      auto ms = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
      std::this_thread::sleep_until(ms);
    }
  }).detach();
}

int main(int argc, char * argv[]) {
  signal(SIGINT, sigHandler);
  run(iterateProcesses, 1000);
  while (true);

  return 0;
}
