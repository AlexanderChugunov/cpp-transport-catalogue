
#include<iostream>
#include <fstream>
#include <cassert>
#include "json_reader.h"


int main() {    
    //std::ifstream in("input.json");
     setlocale(LC_ALL, "ru_RU.UTF-8");
    ProcessJsonRequests(std::cin, std::cout);
}
