#include <iostream>
#include <string>
using namespace std;

void print_options();
void print_search_options();
void print_help();
int32_t get_menu_option(int32_t min, int32_t max);
bool is_valid_name(const string& name);

class Input {
    public:
        void print_options() {
            cout << "=== Menu Options ===\n";
            cout << "1. How to use Application: enter '1'\n";
            cout << "2. Edit existing list: enter '2'\n";
            cout << "3. Create new list (automatically txt files): enter '3'\n";
            cout << "4. Preform a Search on the Master list: enter '4'\n";
            cout << "5. Quit: enter '5'\n";
        }
        void print_search_options() {
            cout << "=== Search Options ===\n";
            cout << "1. Timestamp search (search by year, month, date): enter '1'\n";
            cout << "2. Keyword search (filename and contents): enter '2'\n";
        }
        void print_help() {
            cout << "\nThis application will prompt you with '%' symbol, indicating a option to be choosen from the above options list\n";
            cout << "The symbol will be prompted after each command, proccessed and repeated until you type '4' or 'quit'\n";
            cout << "Your list names are sorted by starred, recent timestamps then alphabetically\n";
            cout << "Your list will be saved when you quit the application\n\n";
            cout << "When modifiying list contents there are no undo's, when selected to delete/clear list, you will be prompted with a confirmation. \n";
            cout << "Thank you! Please email me at kyleighwestman@gmail.com for any issues/suggestions!\n\n";
        }
        void print_cmd_options() {
            cout << "=== File Command Options === \n";
            cout << "Type: 'a' to add an entry anywhere in list (list contents will be printed)\n";
            cout << "Type: 'd' to delete an entry anywhere in list (list contents will be printed)\n";
            cout << "Type: 'b' to move an entry to the beginning of the list (list contents will be printed)\n";
            cout << "Type: 'e' to move an entry to the end of the list (list contents will be printed)\n";
            cout << "Type: 'c' to clear the entire lists contents\n";
            cout << "Type: 'x' to delete the entire list\n";
            cout << "Type: 'p' to print the entire lists contents\n";
            cout << "Type: 'r' to see this menu again\n";
            cout << "Type: 'q' to quit and save list\n";
        }
        void print_doesnt_exist(string &name) {
            cout << "Couldn't find file in database. Select new option to proceed:\n";
            cout << "1. Retype filename: enter '1'\n";
            cout << "2. Create new list named '" << name << "': enter '2' \n";
            cout << "3. Print Master List of all existing file names: enter '3'\n";
            cout << "4. Preform a Search on the Master list: enter '4' \n";
            cout << "5. Quit: enter '5' \n";
        }
        int32_t get_menu_option(uint32_t min, uint32_t max) {
            int32_t option;
            while (true) {
                cout << "%";
                cin >> option;
                if(cin.fail() || option < min || option > max) {
                    // cin.clear();
                    // cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "Error: Please enter a number between " << min << " and " << max << ".\n";
                }
                else {
                    // cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    return option;
                }
            }
        }

        bool is_valid_name(const string& name) {
            if(name.empty()) {
                cerr << "Error: Name cannot be empty.\n";
                return false;
            }
            return true;
        }
};