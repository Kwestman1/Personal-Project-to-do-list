#ifndef FILE_H
#define FILE_H

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
using namespace std;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

// --------------------- FILE CLASS --------------------- //

struct File {
    int32_t file_idx{ -1 }; // where the file is in the master file
    uint32_t comp_timestamp{ 0 }; 
    bool favorite{ false };
    string print_timestamp{ " " };
    string file_name{ " " };
    vector<string> master_list; // contains all file entries for that list
    
    void print_list() {
        for (uint32_t i = 0; i < master_list.size(); i++) {
            cout << i << ". " << master_list[i] << "\n";
        }
    }
    void add_file_contents() {
        std::ifstream fin(file_name);
        if (!fin) {
            std::cerr << "Error opening the file." << std::endl;
            exit(1);
        }
        string line;
        while (std::getline(fin, line)) {
            master_list.push_back(line); 
        }
        fin.close();
    }
    uint32_t get_size() {
        return master_list.size();
    }
    void set_time();
    string format_time(time_t t) const;
    void append(uint32_t position);
    void delete_el(uint32_t position);
    void move_to_beginning(uint32_t position);
    void move_to_end(uint32_t position);

    File() {} // ctor
};

#endif // FILE_H