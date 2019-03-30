#include <iostream>
#include <cstdint>
#include <experimental/filesystem>
#include <string>

using namespace std;

int main()
{
 
    string s = "abc";
    const char* a = s.c_str();
    if (a[3] == '\0') {
        cout<<"Working expectedly1\n";
    }
    if (s[3] == '\0')
    {
        cout<<"Working expectedly2";
    }
    


}