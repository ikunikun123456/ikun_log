#include <ikun_log/log.h>

#include <iostream>

int main() {
  using namespace ikun::log;
  std::cout << "begin log print" << std::endl;
  for (int i = 0; i < 10; i++) {
    std::string msg = "hello";
    Info(msg);

    // Trace("hello world!");
    // Debug("hello world!");
    Info("hello world!");
    // std::cout << er.category().name() << " " << er.message() << std::endl;
    Warn("hello world!");
    Error("hello world!");
    // Register::GetInstance().GetDefaultLog().SetLoglevel(Loglevel::TRACE);
    Trace("hello world!");
    Debug("hello world!");
    Info("hello world!");
    Warn("hello world!");
    Error("hello world!");
  }
  std::cout << "end log print" << std::endl;
  return 0;
}