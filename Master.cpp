#include "Master.h"

// --------------------- PROCESSING FUNCTIONS --------------------- //

bool MasterFiles::process_name(string &name) {
    uint32_t option;
    File file;
    Input menu;
    int fileNumber = -1; // indicator that not special case

    // find list in hash map
    while (get_num_dupes(name) == -1) {
      size_t openParen = name.find_last_of('(');
      size_t closeParen = name.find_last_of(')');
      // Special case, enters file name with (x)
      if (openParen != string::npos && closeParen != string::npos && openParen < closeParen) {
          string numStr = name.substr(openParen + 1, closeParen - openParen - 1);
          try {
              fileNumber = stoi(numStr);
          } catch (exception &e) {
              cout << "Error in finding unique file number, Returning to Main Menu.\n";
              return true;  // If parsing fails, return
          }
          break; // exit loop
      }
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
        cout << "Enter filename: ";
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
              break; // It's a filename, break out of loop
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

    list_found(name, fileNumber);
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
    getline(cin, input); 

    istringstream iss(input);
    vector<string> keywords{istream_iterator<string>{iss}, istream_iterator<string>{}};

    if (input.empty() || keywords.empty()) {
        cout << "No keywords entered. Please try again.\n";
        return;
    }

    std::unordered_map<uint32_t, int> file_scores; // Store cumulative scores
    std::unordered_map<uint32_t, std::vector<std::pair<int, std::string>>> content_matches; // Store matched content

    for (const auto &key : keywords) {
        std::unordered_set<uint32_t> file_indices;
        std::unordered_set<uint32_t> content_indices;

        search_with_wildcards(key, file_indices, 'F'); // Search filenames
        search_with_wildcards(key, content_indices, 'C'); // Search content

        // Increase score for filename matches
        for (const auto &index : file_indices) {
            file_scores[index] += 10;
        }

        // Process content matches and increase score
        for (const auto &index : content_indices) {
            file_scores[index] += 15;

            // Store matched content
            for (const auto &pair : k_search) {
                if (pair.first[0] != 'C') continue;

                if (std::find(pair.second.begin(), pair.second.end(), index) != pair.second.end()) {
                    std::smatch match;
                    if (std::regex_search(pair.first, match, std::regex(key, std::regex::icase))) {
                        content_matches[index].emplace_back(15, pair.first.substr(2));
                    }
                }
            }
        }
    }

    // Sort results by score, with tie-breaking logic
    std::vector<std::pair<uint32_t, int>> sorted_results(file_scores.begin(), file_scores.end());
    std::sort(sorted_results.begin(), sorted_results.end(), 
              [&](const std::pair<uint32_t, int> &a, const std::pair<uint32_t, int> &b) { 
                  if (a.second != b.second) return a.second > b.second;

                  const File &fileA = master_files[a.first];
                  const File &fileB = master_files[b.first];

                  if (fileA.favorite != fileB.favorite) return fileA.favorite > fileB.favorite;
                  if (fileA.comp_timestamp != fileB.comp_timestamp) return fileA.comp_timestamp > fileB.comp_timestamp;
                  return fileA.print_file_name < fileB.print_file_name;
              });

    // Print results
    if (sorted_results.empty()) {
        cout << "No files match your search criteria.\n";
    } else {
        cout << "Search results for keywords, ranked best results to worst:\n";

        for (const auto &[index, score] : sorted_results) {
            if (index >= master_files.size()) {
                cerr << "  Error: Index " << index << " is out of bounds!\n";
                continue;
            }

            cout << "  Filename number #" << index << ". File: " << master_files[index].print_file_name 
                << ", Timestamp: " << master_files[index].print_timestamp
                << (master_files[index].favorite ? " \u2B50" : "") << "\n";

            if (content_matches.find(index) != content_matches.end()) {
                cout << "     Matching Keywords:\n";
                for (const auto &[_, keyword] : content_matches[index]) {
                    cout << "      -" << keyword << "\n";
                }
            }
            cout << "\n";
        }
    }
}

void MasterFiles::search_with_wildcards(const std::string &pattern, std::unordered_set<uint32_t> &matching_indices, const char prefix) {
    std::string regex_pattern = ".*" + std::regex_replace(pattern, std::regex(R"(\*)"), ".*") + ".*";
    std::regex re(regex_pattern, std::regex::icase);

    for (const auto &pair : k_search) {
        if (prefix != pair.first[0]) { continue; }

        std::string key_to_match = pair.first;
        if (prefix == 'F') {
            key_to_match += ".txt";
        }

        if (std::regex_search(key_to_match, re)) {
          // std::cout << "Matched: " << key_to_match << " ,inserting indices: ";
            for (uint32_t idx : pair.second) {
              //std::cout << idx << " ";
              matching_indices.insert(idx);
            }
            // cout << "\n";
        }
    }
}


void MasterFiles::search_by_date() {
    int year = -1, month = 0, day = 0;
    int current_year = getCurrentYear();
    std::string input;

    while (true) {
        std::cout << "Enter year (YYYY): ";
        // cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

void MasterFiles::list_found(const string &name, int32_t fileNumber){
  uint32_t idx = 0;
  Input in;
  auto iter_fn = k_search.find("F:" + name);
  cout << "List Found!\n";
  if (fileNumber != -1) { // special case
    auto it = find_if(master_files.begin(), master_files.end(),
                      [&name](const File& f) { return f.print_file_name == name; });
    idx = distance(master_files.begin(), it);
  }
  else if (iter_fn->second.size() > 1) {
    cout << "More than one List name exists\n";
    cout << "Select from numbered list below: \n";
    for (uint32_t i = 0; i < iter_fn->second.size(); i++) {
      std::cout << i << ". Filename: " << master_files[iter_fn->second[i]].print_file_name << ", last edited: " 
                    << master_files[iter_fn->second[i]].print_timestamp 
                    << (master_files[iter_fn->second[i]].favorite ? " \u2B50" : "") << "\n";
    }
    idx = in.get_menu_option(0, iter_fn->second.size());
    idx = iter_fn->second[idx];
  }
  else if (iter_fn->second.size() == 1) {
    idx = iter_fn->second[0];
  }
  else {
    cout << "Programming Error: EMPTY value, master files indexing error";
    exit(1);
  }
  cout << "Now editing file: " << master_files[idx].print_file_name << "\n";
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
