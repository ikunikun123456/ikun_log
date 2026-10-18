#include <ikun_log/log.h>

#include <iostream>

int main() {
  using namespace ikun::log;
  std::cout << "begin async log print" << std::endl;
  for (int i = 0; i < 10000; i++) {
    // volatile int j = 0;
    // for (; j < 1000;) {
    //   int temp = j;
    //   j = temp + 1;
    // }
    // Trace("hello world!");
    // Debug("hello world!");
    AsyncInfo("hello world!");
    // std::cout << er.category().name() << " " << er.message() << std::endl;
    AsyncWarn("hello world!");
    AsyncError("hello world!");
    // Register::GetInstance().GetDefaultLog().SetLoglevel(Loglevel::TRACE);
    AsyncTrace("hello world!");
    AsyncDebug("hello world!");
    AsyncInfo("hello world!");
    AsyncWarn("hello world!");
    AsyncError("hello world!");
    // for (; j < 2000;) {
    //   int temp = j;
    //   j = temp + 1;
    // }
  }
  std::cout << "end async log print" << std::endl;
  return 0;
}