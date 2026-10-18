#include <ikun_log/log.h>

#include <iostream>

int main() {
  using namespace ikun::log;
  std::cout << "begin log print" << std::endl;
  for (int i = 0; i < 10; i++) {
    // Trace("hello world!");
    // Debug("hello world!");
    std::string name = "cxk";
    Info("hello world!,my name is {}", name);
    // std::cout << er.category().name() << " " << er.message() << std::endl;
    Warn("hello world!,my name is {}", name);
    Error("hello world!,my name is {}", name);
    // Register::GetInstance().GetDefaultLog().SetLoglevel(Loglevel::TRACE);
    Trace("hello world!,my name is {}", name);
    Debug("hello world!,my name is {}", name);
    Info("hello world!,my name is {}", name);
    Warn("hello world!,my name is {}", name);
    Error("hello world!,my name is {}", name);
  }
  std::cout << "end log print" << std::endl;
  return 0;
}