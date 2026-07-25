#include <iostream>
#include <string>
#include <thread>
#include "library.h"
#include "include/IO.h"
#include "include/SharedBuffer.h"
using namespace std;

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.
    SharedBuffer buffer;


    std::thread t1(io::IO::inputThread, std::ref(buffer));
    std::thread t2(io::IO::workerThread, std::ref(buffer));

    t1.join();
    t2.join();
    /*
    string str = "1213313466668797991132235356757454686877996675426886776907532868681113538688469867";
    //string str = "91919191919131";
    cout << "Before" << str << "\n";
    lib::processString(str);
    cout << "After:" << str << endl;
    cout <<"Sum: "<<lib::calculateSum(str) << endl;
    cout <<"ITS: " << lib::checkSum(lib::calculateSum(str));
    */
    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}