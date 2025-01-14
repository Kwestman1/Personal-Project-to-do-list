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
    string file_name;
    int file_idx; // where the file is in the master file
    bool favorite;
    string print_timestamp;
    uint32_t comp_timestamp; 
    vector<string> master_list; // contains all file entries for that list
    
    void set_time();
    string format_time(time_t t) const;
    void add_file_contents();
    // Proccess command functions:
    void print_list();
    void append(uint32_t position);
    void delete_el(uint32_t position);
    void move_to_beginning(uint32_t position);
    void move_to_end(uint32_t position);

    File() : file_name{" "}, file_idx{-1}, favorite{false}, print_timestamp{" "}, comp_timestamp{0} {}
};

void File::set_time() {
    TimePoint timestamp = Clock::now();
    std::time_t t = Clock::to_time_t(timestamp);
    // Adjust for the time zone offset (-8: PST, -5: EST)
    constexpr int timezone_offset_seconds = -8 * 3600;
    t += timezone_offset_seconds;
    // update timestamps
    comp_timestamp = static_cast<uint64_t>(t);
    print_timestamp = format_time(t);
}

string File::format_time(time_t t) const {
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// ----- Proccess command functions ----- //

void File::print_list() {
    for (uint32_t i = 0; i < master_list.size(); i++) {
        cout << i << ". " << master_list[i] << "\n";
    }
}

