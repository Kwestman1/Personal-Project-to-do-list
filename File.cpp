#include "File.h"

// --------------------- TIME FUNCTIONS --------------------- //

void File::set_time() {
    using Clock = std::chrono::system_clock;
    auto timestamp = Clock::now();
    std::time_t t = Clock::to_time_t(timestamp);  // Get UTC time

    #ifdef _WIN32
        std::tm utc_tm;
        gmtime_s(&utc_tm, &t); // Get UTC time safely on Windows
    #else
        std::tm utc_tm;
        gmtime_r(&t, &utc_tm); // Get UTC time safely on POSIX systems
    #endif

    comp_timestamp = static_cast<uint32_t>(t);
    print_timestamp = format_time(t);  
}

string File::format_time(time_t t) const {
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// --------------------- COMMAND FUNCTIONS --------------------- //

void File::delete_el(uint32_t position) {
    uint32_t size = static_cast<uint32_t>(master_list.size() - 1);
    for (uint32_t i = position; i < size; i++) {
        master_list[i] = master_list[i + 1];
    }
    master_list.resize(size);
    cout << "Deleted list entry " << position << ". Enter next command\n";
}

void File::move_to_beginning(uint32_t position) {
    for (uint32_t i = position; i > 0; i--) {
        std::swap(master_list[i], master_list[i - 1]);
    }
    cout << "Moved list entry " << position << " to the beginning. Enter next command\n";
}

void File::move_to_end(uint32_t position) {
    for (uint32_t i = position; i < master_list.size() - 1; i++) {
        std::swap(master_list[i], master_list[i + 1]);
    }
    cout << "Moved list entry " << position << " to the end. Enter next command\n";
}

void File::update_file() {
    std::ofstream fout(print_file_name + ".txt");  // Open file for writing (overwrite mode)
    if (!fout) {
        std::cerr << "Error: Could not open file " << print_file_name << " for writing.\n";
        return;
    }

    for (const auto &phrase : master_list) {
        fout << phrase << '\n';  // Write each phrase to the file
    }

    fout.close();
}
