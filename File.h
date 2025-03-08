#pragma once

#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <unordered_set> 
#include <cstdio>
#include <queue>
using namespace std;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

// --------------------- FILE CLASS --------------------- //

struct File {
    uint32_t comp_timestamp{ 0 };
    bool favorite{ false };
    string print_timestamp{ " " };
    string print_file_name{ " " };
    string hash_file_name{ " " };
    vector<string> master_list; // Contains all file entries

    inline void print_list() {
        for (uint32_t i = 0; i < master_list.size(); i++) {
            cout << i << ". " << master_list[i] << "\n";
        }
    }

    inline uint32_t get_size() {
        if (master_list.empty()) {return 0;}
        return master_list.size();
    }

    // Functions that should be implemented in File.cpp
    void set_time();
    string format_time(time_t t) const;
    void append(uint32_t position); 
    string delete_el(uint32_t position); // return the deleted phrase so we can remove from hash_map
    void move_to_beginning(uint32_t position);
    void move_to_end(uint32_t position);
    void update_file(); // write to file

    File() {} // Constructor
};
