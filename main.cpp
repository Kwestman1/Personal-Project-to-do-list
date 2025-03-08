#include "Master.h"

// --------------------- DRIVER --------------------- //
int main() {
    std::ios_base::sync_with_stdio(false);
    cout << "Hello!! Welcome to your personalized To-Do List!\n";

    uint32_t option;
    string name = "";
    MasterFiles m;
    Input menu;

    while (true) {
        menu.print_options();
        option = menu.get_menu_option(1, 6);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clears leftover newline after numeric input
        
        if (option == 6) {
            break;
        }
        if (option == 1) {
            menu.print_help();
        }
        else if (option == 2) {
            do {
                cout << "Enter list name: ";
                getline(cin, name);  // No need for cin.ignore() here
            } while (!menu.is_valid_name(name));

            if (m.process_name(name)) {
                continue;
            }
        }
        else if (option == 3) {
            cout << "Creating new list!\n";
            do {
                cout << "Enter list name: ";
                getline(cin, name);  // No extra cin.ignore() here
            } while (!menu.is_valid_name(name));

            m.new_list(name);
        }
        else if (option == 4) {
            m.print_filenames();
        }
        else if (option == 5) {
            menu.print_search_options();
            uint32_t new_opt = menu.get_menu_option(1, 3);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Ensure clean input buffer for next getline()

            if (new_opt == 1) {
                m.search_by_date();
            }
            else if (new_opt == 2){
                m.do_key_search();
            }
            else {
                continue;
            }
        }
    }

    cout << "Goodbye! Thank you for using my application!\n";
    return 0;
}