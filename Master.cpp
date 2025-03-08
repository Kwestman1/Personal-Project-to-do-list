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
          cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
          getline(cin, name);
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
        print_filenames();
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
  string print_name = name;
  while (get_num_dupes(name) != -1) {
    string user_input;
    cout << "The name \"" << name << "\" is already in use.\n";
    cout << "Enter a new list name (or press Enter to auto-generate a unique name): ";
    std::getline(cin, user_input);

    if (!user_input.empty()) {
      name = user_input;
      print_name = name;
    }
    else {
      // Auto-generate a unique name by appending a number
      int32_t dupe_count = get_num_dupes(name);
      print_name = name + "(" + to_string(dupe_count) + ")";
      cout << "Auto-generated new name: " << print_name << "\n";
      break;
    }
  }
  // Create and add the file with the finalized unique name
  file.print_file_name = print_name;
  file.hash_file_name = name;
  cout << "Creating file " << print_name << "\n";
  add_file(file);
}

void MasterFiles::add_file(File &file) {
  string f_name = file.print_file_name;
  // Create and open the file
  std::ofstream fin(f_name + ".txt");
  // check file accordingly
  if (fin.fail()) {
    std::cerr << "Error creating the file." << std::endl;
    return;
  }
  fin.close();

  uint32_t insertPos = insert_file(file);
  file = master_files[insertPos];
  // add file to hash map
  auto iter_fn = k_search.find("F:" + file.hash_file_name);
  if (iter_fn == k_search.end()) {
    k_search["F:" + file.hash_file_name].push_back(insertPos);
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
    // if (testing) {return;}
    std::ifstream fin;
    File file = master_files[master_idx];
    fin.open(file.print_file_name + ".txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Could not open file " << file.print_file_name << ". Please check the file name and try again.\n";
        return;
    }

    file.set_time();
    // Update priority Step 1: remove File
    master_files.erase(master_files.begin() + master_idx);
    // Step 2: Reinsert in sorted order
    uint32_t new_idx = insert_file(file);
    // Step 3: Update indices in k_search
    master_idx = std::min(master_idx, static_cast<uint32_t>(new_idx));
    update_indices(master_idx);
    file = master_files[master_idx];

    Input menu;
    menu.print_cmd_options(file.favorite);

    char cmd = ' ';
    uint32_t pos = 0;
    char confirm = ' ';

    do {
        std::cout << "% ";
        std::cin >> cmd;
        file = master_files[master_idx];

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

          master_files[master_idx].master_list.insert(master_files[master_idx].master_list.begin() + pos, phrase);
          add_phrase(phrase, master_idx, "C:");
          master_files[master_idx].update_file();  // Save changes to file immediately
          std::cout << "Phrase processed, enter next command\n";
          file = master_files[master_idx];

      } else if (cmd == 'd') {  // Delete entry
          string phrase = file.delete_el(pos);
          file.update_file();  // Update file after deleting an entry
          delete_phrase("C:" + phrase, master_idx);
          master_files[master_idx] = file;

      } else if (cmd == 'b') {  // Move to beginning
          file.move_to_beginning(pos);
          file.update_file();  // Update file after moving an entry
          master_files[master_idx] = file;

      } else if (cmd == 'e') {  // Move to end
          file.move_to_end(pos);
          file.update_file();  // Update file after moving an entry
          master_files[master_idx] = file;

      } else if (cmd == 'c') {  // Clear list
          std::cout << "Please type 'y' to confirm that you want to clear this list: ";
          std::cin >> confirm;
          if (confirm == 'y') {
              for (const auto &phrase : file.master_list) {
                  delete_phrase("C:" + phrase, master_idx);
              }
              file.master_list.clear();
              file.update_file();  // Save changes after clearing
              master_files[master_idx] = file;
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
            delete_file(file.print_file_name + ".txt");
            delete_phrase("F:" + file.hash_file_name, master_idx);
            master_files.erase(master_files.begin() + master_idx);
            // Update indices AFTER deleting an entry
            update_indices(master_idx);
            master_files[master_idx] = file;
            std::cout << "List deleted, Returning to main menu.\n";
            break;
          }
          else {
            cout << "Not deleting, enter next command\n";
          }
        } else if (cmd == 's') {  // Star/Unstar
            file.favorite = !file.favorite;
            std::cout << (file.favorite ? "Starred!, enter next command\n" : "Removed star, enter next command\n");
            // Update priority Step 1: remove File
            master_files.erase(master_files.begin() + master_idx);
            // Step 2: Reinsert in sorted order
            uint32_t new_idx = insert_file(file);
            // Step 3: Update indices in k_search
            master_idx = std::min(master_idx, static_cast<uint32_t>(new_idx));
            update_indices(master_idx);
            file = master_files[master_idx];
        } else if (cmd == 'r'){
            menu.print_cmd_options(file.favorite);
        } else if ((cmd != 'p' && cmd != 'q') || cin.fail()){
            std::cerr << "Error: command doesn't exist\n";
        }
    } while (cmd != 'q');

    fin.close();
}

// --------------------- SEARCH FUNCTIONS --------------------- //

void MasterFiles::do_key_search() {
    string input;
    cout << "Enter keyword(s): ";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore leftover newline
    getline(cin, input);
    
    istringstream iss(input);
    vector<string> keywords{istream_iterator<string>{iss}, istream_iterator<string>{}};
    
    if (keywords.empty()) {
        cout << "No keywords entered. Please try again.\n";
        return;
    }
    
    unordered_set<uint32_t> common_file_indices;
    unordered_set<uint32_t> common_content_indices;
    
    for (const auto &key : keywords) {
        unordered_set<uint32_t> file_indices;
        unordered_set<uint32_t> content_indices;
        
        search_with_wildcards(key, file_indices, 'F');
        search_with_wildcards(key, content_indices, 'C');
        
        if (common_file_indices.empty()) {
            common_file_indices = file_indices;
        } else {
            unordered_set<uint32_t> temp;
            for (const auto &index : common_file_indices) {
                if (file_indices.find(index) != file_indices.end()) {
                    temp.insert(index);
                }
            }
            common_file_indices = temp;
        }
        
        if (common_content_indices.empty()) {
            common_content_indices = content_indices;
        } else {
            unordered_set<uint32_t> temp;
            for (const auto &index : common_content_indices) {
                if (content_indices.find(index) != content_indices.end()) {
                    temp.insert(index);
                }
            }
            common_content_indices = temp;
        }
    }
    
    if (common_file_indices.empty() && common_content_indices.empty()) {
        cout << "No files contain all specified keywords in either filenames or content.\n";
        return;
    }
    
    cout << "Search results for keywords:\n";
    
    if (!common_file_indices.empty()) {
        cout << "  File Name Search results:\n";
        for (const auto &index : common_file_indices) {
            if (index >= master_files.size()) {
                cerr << "  Error: Index " << index << " is out of bounds!\n";
                continue;
            }
            cout << "  " << index << ". File: " << master_files[index].print_file_name 
                 << ", Timestamp: " << master_files[index].print_timestamp
                 << (master_files[index].favorite ? " \u2B50" : "") << "\n";
        }
    }
    
    if (!common_content_indices.empty()) {
        cout << "  File Content Search results:\n";
        for (const auto &index : common_content_indices) {
            if (index >= master_files.size()) {
                cerr << "  Error: Index " << index << " is out of bounds!\n";
                continue;
            }
            
            vector<pair<int, string>> scored_keywords;
            for (const auto &pair : k_search) {
                if (pair.first[0] != 'C') continue;
                std::smatch match;
                for (const auto &key : keywords) {
                    if (std::regex_search(pair.first, match, std::regex(key, std::regex::icase))) {
                        int score = 100 - static_cast<int>(match.str().length()) + (50 - static_cast<int>(match.position()));
                        scored_keywords.emplace_back(score, pair.first.substr(2)); // Remove "C:" prefix
                    }
                }
            }
            
            cout << "  List number #" << index << ". File: " << master_files[index].print_file_name 
                 << ", Timestamp: " << master_files[index].print_timestamp
                 << (master_files[index].favorite ? " \u2B50" : "") << "\n";
            cout << "     Matching Keywords: ";
            for (const auto &[score, keyword] : scored_keywords) {
                cout << keyword << " ";
            }
            cout << "\n";
        }
    }
}

void MasterFiles::search_with_wildcards(const std::string &pattern, std::unordered_set<uint32_t> &matching_indices, const char prefix) {
    std::string regex_pattern = ".*" + std::regex_replace(pattern, std::regex(R"(\*)"), ".*") + ".*";
    std::regex re(regex_pattern, std::regex::icase);
    std::cout << "Regex pattern: " << regex_pattern << "\n";

    // Define a scoring function for better match ranking
    auto score_match = [](const std::string &key, const std::smatch &match) {
        int exact_match = (match.str() == key) ? 1000 : 0; // Perfect match gets the highest score
        int match_length = static_cast<int>(match.str().length());
        int position = static_cast<int>(match.position());
        return exact_match + (100 - match_length) + (50 - position); // Prioritize shorter and earlier matches
    };

    // Priority queue for ranking matches
    using ScoredMatch = std::pair<int, uint32_t>;  // (score, index)
    std::priority_queue<ScoredMatch> pq;

    for (const auto &pair : k_search) {
        if (prefix != pair.first[0]) { continue; }
        
        std::string key_to_match = pair.first;
        if (prefix == 'F') {
            key_to_match += ".txt";
        }
        std::cout << "Checking against: " << key_to_match << "\n";

        std::smatch match;
        if (std::regex_search(key_to_match, match, re)) {
            int score = score_match(key_to_match, match);
            std::cout << "Matched: " << key_to_match << " (Score: " << score << "), inserting indices: ";
            for (uint32_t idx : pair.second) {
                std::cout << idx << " ";
                pq.emplace(score, idx);
            }
            std::cout << "\n";
        }
    }

    // Insert results into the matching_indices set in priority order
    while (!pq.empty()) {
        matching_indices.insert(pq.top().second);
        pq.pop();
    }
}

void MasterFiles::search_by_date() {
    int year = -1, month = 0, day = 0;
    int current_year = getCurrentYear();
    std::string input;

    while (true) {
        std::cout << "Enter year (YYYY): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        std::getline(std::cin, input);

        if (input.empty()) {
            std::cout << "Year is required. Please enter a valid year.\n";
            continue;
        }

        try {
            year = std::stoi(input);
            if (year < 1900 || year > current_year)throw std::out_of_range("Invalid year");
            break;
        } catch (...) {
            std::cout << "Invalid input. Please enter a valid year (1900 - " << current_year << ").\n";
        }
    }

    std::cout << "Enter month (MM, or press Enter to select all months): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            month = std::stoi(input);
            if (month < 1 || month > 12) throw std::out_of_range("Invalid month");
        } catch (...) {
            std::cout << "Invalid month. Defaulting to all months.\n";
            month = 0;
        }
    }

    std::cout << "Enter day (DD, or press Enter to select all days): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            day = std::stoi(input);
            int max_days = get_days_in_month(year, month > 0 ? month : 1);
            if (day < 1 || day > max_days) throw std::out_of_range("Invalid day");
        } catch (...) {
            std::cout << "Invalid day. Defaulting to all days.\n";
            day = 0;
        }
    }

    uint32_t lower_bound = 0, upper_bound = 0;
    if (day > 0) {
        // Search for a specific day
        lower_bound = create_timestamp(year, month, day);
        upper_bound = create_timestamp(year, month, day + 1) - 1;
    } else if (month > 0) {
        // Search for a specific month
        lower_bound = create_timestamp(year, month, 1);
        upper_bound = create_timestamp(year, month == 12 ? 1 + year : month + 1, 1) - 1;
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
            std::cout << file.print_timestamp << " (" << file.print_file_name << ") " << (file.favorite ? " \u2B50" : "") << "\n";
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
    cout << "Select from numbered list below: \n";
    for (uint32_t i = 0; i < iter_fn->second.size(); i++) {
      std::cout << i << ". Filename: " << master_files[iter_fn->second[i]].print_file_name << ", last edited: " 
                    << master_files[iter_fn->second[i]].print_timestamp 
                    << (master_files[iter_fn->second[i]].favorite ? " \u2B50" : "") << "\n";
    }
    idx = in.get_menu_option(0, iter_fn->second.size());
    cout << "Now editing file: " << master_files[idx].print_file_name << "\n";
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
    string key = prefix + phrase; // Store the entire phrase as the key

    auto& indices = k_search[key];
    if (std::find(indices.begin(), indices.end(), idx) == indices.end()) {
        indices.push_back(idx);
    }

    // Track the phrase in reverse_map
    reverse_map[idx].insert(key);
}

void MasterFiles::delete_phrase(const string& phrase, uint32_t idx) {
    auto it = k_search.find(phrase);
    if (it != k_search.end()) {
        auto& indices = it->second;

        // Remove idx from the indices list
        indices.erase(std::remove(indices.begin(), indices.end(), idx), indices.end());

        // If no more indices reference this phrase, remove the phrase entirely
        if (indices.empty()) {
            k_search.erase(it);
        }
    }

    // Remove phrase reference from reverse_map
    auto rev_it = reverse_map.find(idx);
    if (rev_it != reverse_map.end()) {
        rev_it->second.erase(phrase);
        if (rev_it->second.empty()) {
            reverse_map.erase(rev_it);
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
