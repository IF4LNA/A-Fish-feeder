#include <iostream>
using namespace std;

int main(int argc, char const *argv[]){
    string JadwalMakan[2] = {"10:10:00", "10:10:10"};
    cout << (
        JadwalMakan[1] < JadwalMakan[0]&&
        JadwalMakan[0] < JadwalMakan[1]
    ) << endl;
    return 0;
}