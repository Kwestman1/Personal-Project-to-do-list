#include "File.h"

// --------------------- TIME FUNCTIONS --------------------- //

void File::set_time() {
    TimePoint timestamp = Clock::now();
    std::time_t t = Clock::to_time_t(timestamp);
    // Adjust for the time zone offset (-8: PST, -5: EST)
    constexpr int timezone_offset_seconds = -8 * 3600;
    t += timezone_offset_seconds;
    // update timestamps
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
    cout << "Deleted list entry " << position << "\n";
}

void File::move_to_beginning(uint32_t position) {
    for (uint32_t i = position; i > 0; i--) {
        std::swap(master_list[i], master_list[i - 1]);
    }
    cout << "Moved list entry " << position << " to the beginning. \n";
}

void File::move_to_end(uint32_t position) {
    for (uint32_t i = position; i < master_list.size() - 1; i++) {
        std::swap(master_list[i], master_list[i + 1]);
    }
    cout << "Moved list entry " << position << " to the end.\n";
}
