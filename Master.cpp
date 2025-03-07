#include "Master.h"

// --------------------- PROCESSING FUNCTIONS --------------------- //

bool MasterFiles::process_name(string &name) {
    uint32_t option;
    File file;
    Input menu;
    // find list in hash map
    while (get_num_dupes(name) == -1) {
      // if doesn't exist, print new options
      menu.print_doesnt_exist(name);
      option = menu.get_menu_option(1, 5);

      if (option == 1) {
        do {
          cout << "Enter list name (automatically a txt file): ";
          cin >> name;
        } while (!menu.is_valid_name(name));
      }
      else if (option == 2) {
        cout << "Creating new list!\n";
        new_list(name);
        return true;
      }
      else if (option == 3) {
        if (get_files().empty()) {
            cout << "No files yet! Returning back to main menu!\n";
            return false;
        }
        print_filenames(0, -1);
        cout << "Enter filename or number: ";
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        while (std::getline(cin, name)) {
          if (all_of(name.begin(), name.end(), ::isdigit)) { // Check if input is a number
              try {
                  uint32_t num = stoul(name); // Convert to unsigned integer
                  // Validate range
                  if (num >= 0 && num < get_files().size()) {
                      name = get_name(num);
                      break; // Valid number, exit loop
                  } else {
                      cout << "Invalid number, please enter a number between 0 and " << get_files().size() - 1 << ": ";
                  }
              } catch (const exception &e) {
                  cout << "Invalid input. Please enter a valid number or filename: ";
              }
          } else {
              break; // It's a filename, exit loop
          }
      }
      }
      else if (option == 4) {
        if (get_files().empty()) {
            cout << "No files yet! Returning back to main menu!\n";
            return false;
        }
        menu.print_search_options();
        uint32_t new_opt = menu.get_menu_option(1, 2);
        if (new_opt == 1) {
          search_by_date();
        }
        else {
          do_key_search();
        }
      }
      else if (option == 5) {
        return true;
      }
    }

    list_found(name);
    return false;
}

void MasterFiles::new_list(string &name) {
  File file;
  while (get_num_dupes(name) != -1) {
    string user_input;
    cout << "The name \"" << name << "\" is already in use.\n";
    cout << "Enter a new list name (or press Enter to auto-generate a unique name): ";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(cin, user_input);

    if (!user_input.empty()) {
      name = user_input;
    }
    else {
      // Auto-generate a unique name by appending a number
      int32_t dupe_count = get_num_dupes(name);
      name += "(" + to_string(dupe_count) + ")";
      cout << "Auto-generated new name: " << name << "\n";
    }
  }
  // Create and add the file with the finalized unique name
  file.file_name = name;
  cout << "Creating file " << file.file_name << "\n";
  add_file(file);
}

void MasterFiles::add_file(File &file) {
  // Create and open a file
  string f_name = file.file_name + ".txt";
  std::ofstream fin(f_name);
  // check file accordingly
  if (fin.fail()) {
    std::cerr << "Error creating the file." << std::endl;
    return;
  }
  fin.close();

  // insert file into sorted master_file
  auto it = std::lower_bound(master_files.begin(), master_files.end(), file, Sorter());
  uint32_t insertPos = std::distance(master_files.begin(), it);
  master_files.insert(it, std::move(file));
  master_files[insertPos].file_idx = insertPos;
  // add file to hash map
  auto iter_fn = k_search.find("F:" +  master_files[insertPos].file_name);
  if (iter_fn == k_search.end()) {
    k_search["F:" +  master_files[insertPos].file_name].push_back(insertPos);
  }
  else {
    iter_fn->second.push_back(insertPos);
  }
  // Update the indices in the hash map so they match the master_files indices
  update_indices(insertPos);
  cout << "New List successfully created!\n\n";
  process_commands(insertPos);
}

void MasterFiles::process_commands(uint32_t master_idx) {
    std::ifstream fin;
    File &file = master_files[master_idx];
    fin.open(file.file_name + ".txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Could not open file " << file.file_name << ". Please check the file name and try again.\n";
        return;
    }
    file.set_time();

    Input menu;
    menu.print_cmd_options(file.favorite);

    char cmd = ' ';
    uint32_t pos = 0;
    char confirm = ' ';

    do {
        std::cout << "% ";
        std::cin >> cmd;

        if (cmd == 'p' || cmd == 'a' || cmd == 'd' || cmd == 'b' || cmd == 'e') {
            if (file.get_size() == 0 && cmd != 'a') {
              menu.print_empty_message();
              menu.print_cmd_options(file.favorite);
              continue;
            }
            file.print_list();
            if (cmd != 'p' && file.get_size() != 0 ) {
              std::cout << "Select the position you want to edit: ";
              if (cmd == 'd' || cmd == 'b' || cmd == 'e') {
                pos = menu.get_menu_option(0, file.get_size() - 1);
              }
              else {
                pos = menu.get_menu_option(0, file.get_size());
              }
            }
        }

        if (cmd == 'a') {  // Add entry
          string phrase;
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          std::cout << "Enter Phrase: ";
          std::getline(std::cin, phrase);

          while (!menu.is_valid_name(phrase)) {
              std::cout << "Enter Phrase: ";
              std::getline(std::cin, phrase);
          }

          file.master_list.insert(file.master_list.begin() + pos, phrase);
          add_phrase(phrase, master_idx, "C:");
          file.update_file();  // Save changes to file immediately
          std::cout << "Phrase processed, enter next command\n";

      } else if (cmd == 'd') {  // Delete entry
          file.delete_el(pos);
          file.update_file();  // Update file after deleting an entry

      } else if (cmd == 'b') {  // Move to beginning
          file.move_to_beginning(pos);
          file.update_file();  // Update file after moving an entry

      } else if (cmd == 'e') {  // Move to end
          file.move_to_end(pos);
          file.update_file();  // Update file after moving an entry

      } else if (cmd == 'c') {  // Clear list
          std::cout << "Please type 'y' to confirm that you want to clear this list: ";
          std::cin >> confirm;
          if (confirm == 'y') {
              for (const auto &phrase : file.master_list) {
                  delete_phrase("C:" + phrase, master_idx);
              }
              file.master_list.clear();
              file.update_file();  // Save changes after clearing
              std::cout << "List cleared\n";
          }
      } else if (cmd == 'x') {  // Delete list
          std::cout << "Please type 'y' to confirm that you want to delete this list: ";
          std::cin >> confirm;
          if (confirm == 'y') {
            for (const auto &phrase : file.master_list) {
              delete_phrase("C:" + phrase, master_idx);
            }
            file.master_list.clear();
            delete_file(file.file_name + ".txt");
            delete_phrase("F:" + file.file_name, master_idx);
            master_files.erase(master_files.begin() + master_idx);
            // Update indices AFTER deleting an entry
            update_indices(master_idx);
            std::cout << "List deleted\n";
          }
          else {
            cout << "Not deleting, enter next command\n";
          }
        } else if (cmd == 's') {  // Star/Unstar
            file.favorite = !file.favorite;
            std::cout << (file.favorite ? "Starred!\n" : "Removed star\n");
        } else if (cmd == 'r'){
            menu.print_cmd_options(file.favorite);
        } else if (cmd != 'p' && cmd != 'q'){
            std::cerr << "Error: command doesn't exist\n";
        }
    } while (cmd != 'q');

    fin.close();
}

// --------------------- SEARCH FUNCTIONS --------------------- //

void MasterFiles::do_key_search() {
  string key;
  cout << "Enter keyword(s): ";
  cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore leftover newline
  getline(cin, key);

  unordered_set<uint32_t> common_file_indices;
  unordered_set<uint32_t> common_content_indices;

  search_with_wildcards(key, common_file_indices, "F:");
  search_with_wildcards(key, common_content_indices, "C:");

  // Debugging
  /*
  cout << "File indices: ";
  for (const auto &idx : common_file_indices) {cout << idx << " ";}
  cout << "\n";
  cout << "Content indices: ";
  for (const auto &idx : common_content_indices) {cout << idx << " ";}
  cout << "\n";
  */

  // combine files into 1
  for (const auto &idx : common_content_indices) {
    common_file_indices.insert(idx);
  }

  if (!common_file_indices.empty()) {
    cout << "Search results:\n";
    for (const auto &index : common_file_indices) {
      // debugging (shouldn't print)
      if (index >= master_files.size()) {
        cerr << "Error: Index " << index << " is out of bounds!\n";
        continue;
      }
      cout << index << ". File: " << master_files[index].file_name << ", Timestamp: " << master_files[index].print_timestamp
          << ", Starred: " << (master_files[index].favorite ? "Yes" : "No") << "\n";
    }
  }
  else {
    cout << "No files contain all specified keywords in either filenames or content.\n";
  }
}

void MasterFiles::search_with_wildcards(const string &pattern, std::unordered_set<uint32_t> &matching_indices, const std::string &prefix) {
    string regex_pattern = std::regex_replace(pattern, std::regex(R"(\*)"), ".*");
    std::regex re(prefix + regex_pattern, std::regex::icase);
    std::cout << "Regex pattern: " << regex_pattern << "\n";

    for (const auto &pair : k_search) {
        std::string key_to_match = pair.first;
        // If prefix is "F:", assume all files have a .txt extension
        if (prefix == "F:") {
            key_to_match += ".txt";
        }
        std::cout << "Checking against: " << key_to_match << "\n";

        if (std::regex_match(key_to_match, re)) {
            std::cout << "Matched: " << key_to_match << ", inserting indices: ";
            for (uint32_t idx : pair.second) {
                std::cout << idx << " ";
            }
            std::cout << "\n";

            matching_indices.insert(pair.second.begin(), pair.second.end());
        }
    }
}


void MasterFiles::search_by_date() {
    int year = -1;
    int month = -1;
    int day = -1;
    int current_year = getCurrentYear();
    while (true) {
        std::cout << "Enter year (YYYY): ";
        std::cin >> year;

        // Check if input is valid
        if (std::cin.fail() || year < 1900 || year > current_year) {
            std::cout << "Invalid year. Please enter a valid year (1900 - " << current_year << ").\n";
            std::cin.clear();
        } else {
            break; // Valid year entered
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
    }

    while (true) {
        std::cout << "Enter month (MM, or press Enter to default to all months): ";
        std::cin >> month;
        if (month == -1) { // nothing entered
          month = 0;
          break;
        }

        if (std::cin.fail() || month < 1 || month > 12) {
            std::cout << "Invalid month. Enter a number between 1 and 12, or press Enter to default to all months.\n";
            std::cin.clear();
        } else {
            break; // Valid month entered
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    while (true) {
        std::cout << "Enter day (DD, or press Enter to default to all days): ";
        std::cin >> day;
        if (day == -1) { // nothing entered
            day = 0;
            break;
        }

        int max_days = get_days_in_month(year, month);
        if (std::cin.fail() || day < 1 || day > max_days) {
            std::cout << "Invalid day. The month you selected has " << max_days << " days.\n";
            std::cin.clear();
        } else {
            break; // Valid day entered
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    uint32_t lower_bound = 0, upper_bound = 0;
    if (day > 0) {
        // Search for a specific day
        lower_bound = create_timestamp(year, month, day);
        upper_bound = create_timestamp(year, month, day + 1) - 1;
    } else if (month > 0) {
        // Search for a specific month
        lower_bound = create_timestamp(year, month, 1);
        if (month == 12) {
            upper_bound = create_timestamp(year + 1, 1, 1) - 1; // Next year
        } else {
            upper_bound = create_timestamp(year, month + 1, 1) - 1;
        }
    } else {
        // Search for an entire year
        lower_bound = create_timestamp(year, 1, 1);
        upper_bound = create_timestamp(year + 1, 1, 1) - 1;
    }

    std::vector<File> results;
    std::copy_if(master_files.begin(), master_files.end(), std::back_inserter(results),
                 [=](const File &file) {
                     return file.comp_timestamp >= lower_bound && file.comp_timestamp <= upper_bound;
                 });

    // **Print the results**
    if (results.empty()) {
        std::cout << "No files found in the given date range.\n";
    } else {
        std::cout << "Files in the range:\n";
        for (const auto &file : results) {
            std::cout << (file.favorite ? "[Starred] " : "") << file.print_timestamp << " (" << file.file_name << ")\n";
        }
    }
}

// --------------------- HELPERS --------------------- //

void MasterFiles::list_found(const string &name){
  auto iter_fn = k_search.find("F:" + name);
  uint32_t idx = 0;
  Input in;
  cout << "List Found!\n";
  if (iter_fn->second.size() > 1) {
    cout << "More than one List name exists\n";
    cout << "Select numbered list below: \n";
    print_filenames(iter_fn->second[0], iter_fn->second.size());
    in.get_menu_option(0, iter_fn->second.size());
  }
  else if (iter_fn->second.size() == 1) {
    idx = iter_fn->second[0];
  }
  else {
    cout << "Programming Error: EMPTY value, master files indexing error";
    exit(1);
  }
  process_commands(idx);
}

void MasterFiles::add_phrase(const string& phrase, uint32_t idx, const string& prefix) {
  std::istringstream iss(phrase);
  string word;
  while (iss >> word) {
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    string key = prefix + word;
    auto iter = k_search.find(key);
    if (iter == k_search.end()) {
      k_search[key].push_back(idx);
    }
    else {
      if (std::find(iter->second.begin(), iter->second.end(), idx) == iter->second.end()) {
        iter->second.push_back(idx); // Avoid duplicate idx entries
      }
    }
  }
}

void MasterFiles::delete_phrase(const string& phrase, uint32_t idx) {
  string temp = "";
  // Locate the phrase parts in the hash map
  for (uint32_t i = 0; i < phrase.size(); i++) {
    if (phrase[i] == ' ') {
      temp = "";
    }
    else {
      temp += phrase[i];
      auto it = k_search.find(phrase);
      // If the phrase is not found, there's nothing to delete
      if (it != k_search.end()) {
        // Remove the idx from the vector of indices
        auto& indices = it->second;
        indices.erase(std::remove(indices.begin(), indices.end(), idx), indices.end());
        // If the vector is empty, remove the key-value pair from the hash map
        if (indices.empty()) {
          k_search.erase(it);
        }
      }
    }
  }
}

uint64_t MasterFiles::create_timestamp(uint32_t year, uint32_t month, uint32_t day) {
    if (month < 1 || month > 12 || day < 1 || day > get_days_in_month(year, month)) {
        return 0; // Invalid date
    }

    // Construct the timestamp
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    // Adjust to UTC
    #ifdef _WIN32
    // Windows version of timegm()
    time_t utc_time = _mkgmtime(&tm);
    #else
    time_t utc_time = timegm(&tm);
    #endif

    return static_cast<uint64_t>(utc_time);
}
