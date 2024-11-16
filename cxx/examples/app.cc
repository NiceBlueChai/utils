#include "utils/utils.h"

#include <chrono>
#include <iostream>
#include <thread>
using namespace std;
int main(int argc, char *argv[]) {

  cout << boolalpha
       << "first is runing: " << nbc::utils::is_running("my_app.yid") << endl;
  if (nbc::utils::is_running("my_app.id")) {
    cout << "already running 程序已经在运行了." << endl;
    exit(0);
  }
  this_thread::sleep_for(chrono::seconds(100));
  return 0;
}
