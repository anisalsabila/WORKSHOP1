#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>  
#include <mysql.h>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <cctype>
#include <regex>
#include <sstream>       
#include <unordered_map>
#include "ordering_system.h"

using namespace std;

// MySQL connection object
MYSQL* conn;

unordered_map<string, string> usersDatabase; 

extern string loggedInCustomerID;
extern int attemptCount;
extern const int maxAttempts;
int attemptCount = 0;          // Initialize attemptCount to 0
const int maxAttempts = 3;     // Set maxAttempts to 3

string loggedInCustomer = ""; 

// Function Prototypes (Optional but recommended for clarity)
void printLine(char ch = '-', int length = 50);
void waitForUser();

// Function Definition for printLine (no default arguments here)
void printLine(char ch, int length) {
    for (int i = 0; i < length; i++) {
        cout << ch;
    }
    cout << endl;
}

// Function to pause output and give the user a chance to continue (paging)
void waitForUser() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

bool isValidPhoneNumber(const std::string& phoneNumber) {
    std::regex phoneRegex("^\\d{11,12}$");

    return std::regex_match(phoneNumber, phoneRegex);
}

bool isValidPassword(const string& password) {
    regex passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
    return regex_match(password, passwordRegex);
}

std::string hashPassword(const std::string& password) {
    unsigned long hash = 5381; // Starting hash value
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;
    }

    std::ostringstream hashHex;
    hashHex << std::hex << std::setw(16) << std::setfill('0') << hash;
    return hashHex.str();
}

bool isValidEmail(const std::string& email) {
    const std::regex emailRegex(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );

    return std::regex_match(email, emailRegex);
}

class db_response {
public:
    static void ConnectionFunction() {
        conn = mysql_init(0);
        if (conn)
            cout << "Database Connected" << endl;
        else
            cout << "Failed To Connect!" << endl;

        conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);
        if (conn)
            cout << "Database Connected To MySql" << endl;
        else
            cout << "Failed To Connect!" << endl;
    }
};

void resetPassword() {
    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\Reset Password\n";
    cout << "\t\t\t\t\t\t\t\t-------------------\n";
    string username, newPassword;
    cout << "\n\n\t\t\t\t\t\t\t\tEnter your username: ";
    cin >> username;
    cout << "Enter new password (minimum length is 8 characters, must contain uppercase, lowercase, numbers, and symbols): ";
    cin >> newPassword;

    while (!isValidPassword(newPassword)) {
        cout << "Password must contain at least 8 characters, one uppercase letter, one lowercase letter, one number, and one symbol! Try again: ";
        cin >> newPassword;
    }

    // Update the password in the database (simplified for demonstration)
    usersDatabase[username] = newPassword;
    cout << "Password reset successfully!\n";
    system("pause");
}

void login() {

    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "MySQL Connection Failed: " << mysql_error(conn) << endl;
        return;
    }

    string Username, Password;
    bool is_valid = false;
    int attempts = 0; // Counter for login attempts
    MYSQL_RES* res = NULL; // Declare the result pointer
    MYSQL_ROW row = NULL;  // Declare the row pointer
    int qstate; // Declare qstate to store the result of mysql_query()

    while (attempts < 3 && !is_valid) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\tLogin\n";
        cout << "\t\t\t\t\t\t\t\t-------------------\n";
        cout << "\n\n\t\t\t\t\t\t\t\tEnter Username: ";
        cin >> Username;

        cout << "\t\t\t\t\t\t\t\tEnter Password: ";
        Password.clear();

        char ch;
        while ((ch = _getch()) != '\r') { // Read until Enter (carriage return)
            if (ch == '\b') { // Handle Backspace
                if (!Password.empty()) {
                    Password.pop_back();
                    cout << "\b \b"; // Erase last character
                }
            }
            else {
                Password += ch;
                cout << '*'; // Mask the character
            }
        }
        cout << endl;

        // Query to check customer credentials first
        string query = "SELECT * FROM customer WHERE CustomerID = '" + Username + "' AND CustPassword = '" + Password + "'";
        const char* q = query.c_str();
        qstate = mysql_query(conn, q); // Execute the query

        if (!qstate) {
            res = mysql_store_result(conn); // Get the result of the query
            if (res && mysql_num_rows(res) > 0) { // Check if the result has rows
                cout << "\t\t\t\t\t\tLogin Successful! Welcome, " << Username << "!\n";
                mysql_free_result(res); // Free the result
                system("pause");
                loggedInCustomerID = Username;  // Store the logged-in customer's ID 
                ShowCustomerMenu(); // Transition to the customer menu
                return;
            }
            else {
                if (res) mysql_free_result(res); // Free result if not NULL
                // Query to check staff credentials if customer login fails
                query = "SELECT * FROM staff WHERE StaffID = '" + Username + "' AND StaffPassword = '" + Password + "'";
                q = query.c_str();
                qstate = mysql_query(conn, q);

                if (!qstate) {
                    res = mysql_store_result(conn);
                    if (res && mysql_num_rows(res) > 0) { // Check if the result has rows
                        cout << "\t\t\t\t\t\t\t\tLogin Successful! Welcome, " << Username << "!\n";
                        mysql_free_result(res); // Free the result
                        system("pause");
                        ShowStaffMenu(); // Transition to the staff menu
                        return;
                    }
                    else {
                        if (res) mysql_free_result(res); // Free result if not NULL
                        attempts++; // Increment the attempts counter
                        cout << "\t\t\t\t\t\t\t\tInvalid Username or Password. You have " << (3 - attempts) << " attempt(s) left.\n";
                        if (attempts < 3) {
                            system("pause");
                        }
                    }
                }
                else {
                    cout << "\t\t\t\t\t\t\t\tError Occurred: " << mysql_error(conn) << "\n";
                    system("pause");
                    Home();
                    return;
                }
            }
        }
        else {
            cout << "\t\t\t\t\t\t\t\tError Occurred: " << mysql_error(conn) << "\n";
            system("pause");
            Home();
            return;
        }
    }

    // If the user exceeds 3 attempts
    string query = "SELECT * FROM staff WHERE StaffID = '" + Username + "'";
    const char* q = query.c_str();
    qstate = mysql_query(conn, q);

    if (!qstate) {
        res = mysql_store_result(conn);
        if (res && mysql_num_rows(res) > 0) { // User is a staff
            cout << "\n\t\t\t\t\t\t\t\tMaximum login attempts exceeded.\n";
            cout << "\t\t\t\t\t\t\t\tPlease contact the admin to reset your login credentials.\n";
        }
        else { // User is a customer
            cout << "\n\t\t\t\t\t\t\t\tMaximum login attempts exceeded.\n";
            cout << "\t\t\t\t\t\t\t\tWould you like to reset your password? (y/n): ";
            char choice;
            cin >> choice;

            if (choice == 'y' || choice == 'Y') {
                resetPassword(); // Call the forget password function if the user chooses to reset their password
            }
            else {
                cout << "\t\t\t\t\t\t\t\tReturning to Home...\n";
            }
        }
        if (res) mysql_free_result(res); // Free result
    }
    else {
        cout << "\t\t\t\t\t\t\t\tError Occurred: " << mysql_error(conn) << "\n";
    }

    system("pause");
    Home(); // Redirect to the home menu
}

void registerAsCustomer() {
    system("cls");

    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        std::string Username, CustName, CustTelNum, Password;
        bool isUsernameValid = false;

        // Loop to ensure the username is valid and not a duplicate
        do {
            system("cls");
            std::cout << "\n\n\n\t\t\t\t\t\t\t\tRegister as customer\n";
            cout << "\t\t\t\t\t\t\t\t------------------------\n";
            std::cout << "\n\n\nEnter username (Please put 'C' in front of your username. This will be your ID): ";
            std::cin >> Username;

            // Check if username starts with 'C' or 'c'
            if (Username[0] != 'C' && Username[0] != 'c') {
                std::cout << "Error: Username must start with 'C' or 'c'. Please try again.\n";
                continue;
            }

            // Normalize username to start with uppercase 'C'
            if (Username[0] == 'c') {
                Username[0] = 'C';
            }

            // Check if the username already exists in the database
            std::string checkQuery = "SELECT COUNT(*) FROM customer WHERE CustomerID = '" + Username + "';";
            if (mysql_query(conn, checkQuery.c_str()) == 0) {
                MYSQL_RES* res = mysql_store_result(conn);
                MYSQL_ROW row = mysql_fetch_row(res);
                int count = std::stoi(row[0]);

                if (count > 0) {
                    std::cout << "Error: Username already exists. Please choose a different one.\n";
                    mysql_free_result(res);
                    continue;
                }
                mysql_free_result(res);
                isUsernameValid = true; // Username is valid and unique
            }
            else {
                std::cerr << "Error checking username: " << mysql_error(conn) << "\n";
                return;
            }
        } while (!isUsernameValid);

        std::cout << "Enter your full name: ";
        std::cin.ignore();
        std::getline(std::cin, CustName);
        std::cout << "Enter your telephone number (60+): ";
        std::cin >> CustTelNum;

        // Validate phone number
        while (!isValidPhoneNumber(CustTelNum)) {
            std::cout << "Invalid phone number. Please enter a valid digit phone number: ";
            std::cin >> CustTelNum;
        }

        std::cout << "Enter your password (minimum length is 8 characters, must contain uppercase, lowercase, numbers, and symbols): ";
        std::cin >> Password;

        while (!isValidPassword(Password)) {
            std::cout << "\nPassword must contain at least 8 characters, one uppercase letter, one lowercase letter, one number, and one symbol! Try again: ";
            std::cin >> Password;
        }

        std::string query = "INSERT INTO `customer` (CustomerID, CustName, CustTelNum, CustPassword) "
            "VALUES ('" + Username + "', '" + CustName + "', '" + CustTelNum + "', '" + Password + "')";
        const char* q = query.c_str();

        if (mysql_query(conn, q) == 0) {
            std::cout << "\nYour registration is complete! Hello " << Username << "!" << std::endl;
        }
        else {
            std::cerr << "Error registering user: " << mysql_error(conn) << "\n";
        }

        mysql_close(conn);
    }
    else {
        std::cerr << "Database connection failed: " << mysql_error(conn) << "\n";
    }
    system("pause");
}

int main() {
    system("cls");
    system("title My Project");
    db_response::ConnectionFunction();
    welcome_Page();
}

void welcome_Page() {
    system("cls");
    system("Color 7D");

    // Header
    std::cout << "\n\n\n\t\t\t\t\t\t\t\t\t W E L C O M E\n";
    std::cout << "\t\t\t\t\t\t\t\t\t      T O\n";
    std::cout << "\t\t\t\t\t\t  ______________\n";
    std::cout << "\t\t\t\t\t\t  |x|x|x|x|x|x|x|\n";
    std::cout << "\t\t\t\t\t\t  |x|x|     |x|x|\n";
    std::cout << "\t\t\t\t\t\t  |x|x|     |x|x|     _________     _________     _________\n";
    std::cout << "\t\t\t\t\t\t  |x|x|x|x|x|x|       |x|x|x|x|     |x|x|x|x|     |x|x|x|x|\n";
    std::cout << "\t\t\t\t\t\t  |x|x|x|x|x|x|       |x|   |x|           |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t\t\t  |x|x|     |x|x|     |x|x|x|x|     |x|x|x|x|     |x|   |x|\n";
    std::cout << "\t\t\t\t\t\t  |x|x|     |x|x|     |x|           |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t\t\t  |x|x|x|x|x|x|x|     |x|x|x|x|     |x|x|x|x|     |x|   |x|\n";
    std::cout << "\n\t\t\t\t\t\t                                                ___\n";
    std::cout << "\t\t\t\t\t\t                                                |x|\n";
    std::cout << "\t\t\t\t\t\t                                                |x|\n";
    std::cout << "\t\t\t\t\t\t               _________    _________     ______|x|\n";
    std::cout << "\t\t\t\t\t\t               |x|x|x|x|    |x|x|x|x|     |x|x|x|x|\n";
    std::cout << "\t\t\t\t\t\t                     |x|    |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t\t\t               |x|x|x|x|    |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t\t\t               |x|   |x|    |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t\t\t               |x|x|x|x|    |x|   |x|     |x|x|x|x|\n";
    std::cout << "\n\t\t\t\t     _______________                                                                   ___\n";
    std::cout << "\t\t\t\t     |x|x|x|x|x|x|x|                                                                   |x|\n";
    std::cout << "\t\t\t\t     |x|x|     |x|x|                                                                   |x|\n";
    std::cout << "\t\t\t\t     |x|x|     |x|x|     _________     _________     _________     _________     ______|x|\n";
    std::cout << "\t\t\t\t     |x|x|x|x|x|x|       |x|x|x|x|     |x|   |x|     |x|x|x|x|     |x|x|x|x|     |x|x|x|x|\n";
    std::cout << "\t\t\t\t     |x|x|x|x|x|x|       |x|   |x|     |x|   |x|     |x|   |x|     |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t     |x|x|     |x|x|     |x|x|x|x|     |x|   |x|     |x|   |x|     |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t     |x|x|     |x|x|     |x|           |x|x|x|x|     |x|   |x|     |x|   |x|     |x|   |x|\n";
    std::cout << "\t\t\t\t     |x|x|x|x|x|x|x|     |x|x|x|x|           |x|     |x|x|x|x|     |x|   |x|     |x|x|x|x|\n";
    std::cout << "\t\t\t\t                                             |x|\n";
    std::cout << "\t\t\t\t                                       |x|   |x|\n";
    std::cout << "\t\t\t\t                                       |x|x|x|x|\n";
    std::cout << "\n\t\t\t\t\t                                     C A F E\n";

    std::cout << "\n\n\t\t\t\t\t\t\t\t\t    Press enter";
    (void)_getch(); // Ignore the return value
    system("cls");
    Home(); 
}

void Home() {
    system("cls");
    int choice;
    while (true) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t   B  E  A  N\n\t\t\t\n\n";
        cout << "\t\t\t\t\t\t                 A  N  D \n\t\t\t\n\n";
        cout << "\t\t\t\t\t\t                           B  E  Y  O  N  D\n\t\t\t\n\n";
        cout << "\t\t\t\t\t\t                                                C  A  F  E     \n\t\t\t\n\n";
        cout << "\n";
        cout << "\t\t\t\t\t\t\t      ----------------------------- " << endl;
        cout << "\t\t\t\t\t\t\t      | 1. Log in                 |\n";
        cout << "\t\t\t\t\t\t\t      | 2. Register               |\n";
        cout << "\t\t\t\t\t\t\t      | 3. Forgot Password        |\n";
        cout << "\t\t\t\t\t\t\t      | 4. Exit                   |\n";
        cout << "\t\t\t\t\t\t\t      ----------------------------- " << endl;
        cout << "\t\t\t\t\t\t\t       Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            login();
            break;
        case 2:
            registerAsCustomer();
            break;
        case 3:
            resetPassword();
            break;
        case 4:
            cout << "\n\t\t\t\t\t\t\t\tExiting the program.\n";
            return;
        default:
            cout << "\n\t\t\t\t\t\t\tInvalid choice! Please try again.\n";
        }
    }
}

void ShowStaffMenu() {
    system("cls");
    system("Color F0");
    int choice;
    bool inStaffMenu = true;

    while (inStaffMenu) {
        system("cls");
        cout << "\n\n\t\t\t\t\t     ========================================================================\n\t\t\t\n\n";
        cout << "\t\t\t\t\t     ---------------           S T A F F  M E N U             ---------------\n\t\t\t\n\n";
        cout << "\t\t\t\t\t     ========================================================================\t\t\t\n";
        cout << "\t\t\t                                                               \n\t\t\t\n\n";
        cout << "\t\t\t\t\t\t\t        --------------------------------- \n";
        cout << "\t\t\t\t\t\t\t       | 1. Edit Staff Profile           |\n";  
        cout << "\t\t\t\t\t\t\t       | 2. Search Products              |\n";  
        cout << "\t\t\t\t\t\t\t       | 3. View Products                |\n"; 
        cout << "\t\t\t\t\t\t\t       | 4. Product Management           |\n"; 
        cout << "\t\t\t\t\t\t\t       | 5. Order Status                 |\n";
        cout << "\t\t\t\t\t\t\t       | 6. Generate Report              |\n";  
        cout << "\t\t\t\t\t\t\t       | 7. Generate Feedback Report     |\n";  
        cout << "\t\t\t\t\t\t\t       | 8. Logout                       |\n";
        cout << "\t\t\t\t\t\t\t        --------------------------------- " << endl;
        cout << "\n\t\t\t\t\t\t\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            EditStaffProfile();  // Calls the Edit Staff Profile function
            break;
        case 2:
            SearchBar();  // Calls the SearchBar function
            break;
        case 3:
            ViewProducts();  // Calls the View Products function
            break;
        case 4:
            ProductMenu();  // Calls the Product Management Menu
            break;
        case 5:
            OrderStatus();  // Displays order status
            break;
        case 6:
            GenerateReport();  // Generates general reports (daily, monthly, etc.)
            break;
        case 7:
            GenerateFeedbackReport();  // Generates feedback reports
            break;
        case 8:
            cout << "Logging out...\n";
            inStaffMenu = false;  // Exit menu
            return;
        default:
            cout << "Invalid choice, try again.\n";
        }
    }
}

void EditStaffProfile() {
    system("cls");
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    // Check database connection
    if (!conn) {
        cerr << "Database connection failed: " << mysql_error(conn) << endl;
        return;
    }

    string staffID;

    // Step 1: Search for Staff Profile
    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\tStaff Profile\n";
    cout << "\t\t\t\t\t\t\t\t---------------\n\n";
    cout << "Enter Staff ID to search your profile: ";
    cin >> staffID;

    string searchQuery = "SELECT StaffName, StaffEmail, StaffTelNum FROM staff WHERE StaffID = '" + staffID + "'";
    if (mysql_query(conn, searchQuery.c_str()) != 0) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row) {
        cout << "No profile found for Staff ID: " << staffID << ".\n";
        mysql_free_result(res);
        mysql_close(conn);
        system("pause");
        return;
    }

    // Store current profile details into local variables
    string currentName = row[0] ? row[0] : "N/A";
    string currentEmail = row[1] ? row[1] : "N/A";
    string currentTelNum = row[2] ? row[2] : "N/A";

    mysql_free_result(res); 

    // Display current details
    cout << "\nCurrent Profile Information:\n";
    cout << "1. Name: " << currentName << "\n";
    cout << "2. Email: " << currentEmail << "\n";
    cout << "3. Telephone Number: " << currentTelNum << "\n";

    // Ask staff if they want to edit their information
    char choice;
    cout << "\nDo you want to edit your profile? (y/n): ";
    cin >> choice;
    if (tolower(choice) != 'y') {
        cout << "Returning to main menu...\n";
        mysql_close(conn);
        system("pause");
        return;
    }

    // Step 2: Authenticate Before Edit
    string password;
    int attempts = 3;

    while (attempts > 0) {
        cout << "Enter your password to proceed: ";
        cin >> password;

        string hashedPassword = hashPassword(password); // Assuming hashPassword is defined elsewhere
        string validatePasswordQuery = "SELECT * FROM staff WHERE StaffID = '" + staffID + "' AND StaffPassword = '" + password + "'";
        if (mysql_query(conn, validatePasswordQuery.c_str())) {
            cerr << "Error validating password: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) {
            cout << "\nPassword verified. Proceeding with profile update...\n";
            mysql_free_result(res);
            break;
        }
        else {
            attempts--;
            cout << "Incorrect password. You have " << attempts << " attempt(s) left.\n";
            if (attempts == 0) {
                cout << "You have reached the maximum number of attempts. Please contact the admin to reset your password.\n";
                mysql_close(conn);
                system("pause");
                return;
            }
        }
        mysql_free_result(res);
    }

    // Step 3: Edit Profile
    string newName, newEmail, newTelNum;
    cout << "\nYou can now edit your profile.\n";

    // Prompt for new details
    cout << "New Name (leave blank to keep unchanged): ";
    cin.ignore();
    getline(cin, newName);

    cout << "New Email (leave blank to keep unchanged): ";
    getline(cin, newEmail);

    // Validate email if provided
    if (!newEmail.empty() && !isValidEmail(newEmail)) {
        cout << "Invalid email format. Please try again.\n";
        mysql_close(conn);
        return;
    }

    cout << "New Telephone Number (60+) (leave blank to keep unchanged): ";
    getline(cin, newTelNum);

    // Validate phone number if provided
    if (!newTelNum.empty() && !isValidPhoneNumber(newTelNum)) {
        cout << "Invalid phone number. Please enter a valid digit phone number.\n";
        mysql_close(conn);
        return;
    }

    // Display updated profile
    cout << "\nUpdated Profile Information:\n";
    cout << "1. Name: " << (newName.empty() ? currentName : newName) << "\n";  // Use currentName if newName is empty
    cout << "2. Email: " << (newEmail.empty() ? currentEmail : newEmail) << "\n";  // Use currentEmail if newEmail is empty
    cout << "3. Telephone Number: " << (newTelNum.empty() ? currentTelNum : newTelNum) << "\n";  // Use currentTelNum if newTelNum is empty

    // Confirm changes
    char confirm;
    cout << "\nConfirm changes? (y/n): ";
    cin >> confirm;
    if (tolower(confirm) != 'y') {
        cout << "Changes discarded.\n";
        mysql_close(conn);
        return;
    }

    // Build update query
    string updateQuery = "UPDATE staff SET ";
    if (!newName.empty()) updateQuery += "StaffName = '" + newName + "', ";
    if (!newEmail.empty()) updateQuery += "StaffEmail = '" + newEmail + "', ";
    if (!newTelNum.empty()) updateQuery += "StaffTelNum = '" + newTelNum + "', ";

    // Remove trailing comma and space, add condition
    updateQuery = updateQuery.substr(0, updateQuery.size() - 2);
    updateQuery += " WHERE StaffID = '" + staffID + "'";

    // Execute update query
    if (mysql_query(conn, updateQuery.c_str())) {
        cerr << "Error updating profile: " << mysql_error(conn) << endl;
    }
    else {
        cout << "\nProfile updated successfully!\n";
    }
    mysql_close(conn);
    system("pause");
}

void SearchBar() {
    system("cls");

    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        string searchTerm;
        int itemsPerPage = 10;
        int page = 1;

        cout << "\n\n\n\t\t\t\t\t\t\t\t\tSearch Products\n";
        cout << "\t\t\t\t\t\t\t\t\t---------------\n\n";
        cout << "Enter a search term (Product Name or ID): ";
        cin.ignore();  // Clear input buffer
        getline(cin, searchTerm);  // Allow spaces in input

        // Query to count total search results
        string countQuery = "SELECT COUNT(*) FROM `menu item` WHERE ItemName LIKE '%" + searchTerm + "%' OR ItemID LIKE '%" + searchTerm + "%'";
        int qstate = mysql_query(conn, countQuery.c_str());
        int totalItems = 0;
        if (!qstate) {
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            totalItems = atoi(row[0]);
            mysql_free_result(res);
        }
        else {
            cerr << "Error counting search results: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;  // Calculate total pages

        while (true) {
            // SQL query to search for products with pagination and ordering
            string query = "SELECT * FROM `menu item` WHERE ItemName LIKE '%" + searchTerm + "%' OR ItemID LIKE '%" + searchTerm + "%' ORDER BY ItemID ASC LIMIT "
                + to_string(itemsPerPage) + " OFFSET " + to_string((page - 1) * itemsPerPage);
            qstate = mysql_query(conn, query.c_str());

            if (!qstate) {
                res = mysql_store_result(conn);

                cout << "\n\n\n-----------------------------------------------------------------------------------\n";
                cout << std::left << std::setw(10) << "ID"
                    << std::setw(50) << "Name"
                    << std::setw(10) << "Price"
                    << std::setw(15) << "Category" << endl;
                cout << "-----------------------------------------------------------------------------------\n";
                cout << std::fixed << std::setprecision(2);  // Set precision for price

                bool productsFound = false;
                while ((row = mysql_fetch_row(res))) {
                    cout << std::left << std::setw(10) << (row[0] ? row[0] : "")  // ItemID
                        << std::setw(50) << (row[1] ? row[1] : "")               // ItemName
                        << std::setw(10) << (row[2] ? row[2] : "")               // ItemPrice
                        << std::setw(15) << (row[3] ? row[3] : "") << "\n";      // ItemCategory
                    productsFound = true;
                }
                cout << "-----------------------------------------------------------------------------------\n";

                if (!productsFound) {
                    cout << "\nNo products match your search.\n";
                }

                mysql_free_result(res);  // Free the result set

                // Display pagination navigation
                cout << "\nPage " << page << " of " << totalPages << "\n";
                cout << "\nNavigation: ";
                if (page > 1) {
                    cout << "[P] Previous ";
                }
                if (page < totalPages) {
                    cout << "[N] Next ";
                }
                cout << "[E] Exit\n";
                cout << "Choose an option: ";

                char navigationChoice;
                cin >> navigationChoice;

                if (navigationChoice == 'P' || navigationChoice == 'p') {
                    if (page > 1) {
                        page--;  // Go to the previous page
                    }
                }
                else if (navigationChoice == 'N' || navigationChoice == 'n') {
                    if (page < totalPages) {
                        page++;  // Go to the next page
                    }
                }
                else if (navigationChoice == 'E' || navigationChoice == 'e') {
                    break;  // Exit the loop and return to the previous menu
                }
                else {
                    cout << "Invalid choice. Please try again.\n";
                    system("pause");
                }
            }
            else {
                cerr << "Search query failed: " << mysql_error(conn) << endl;
                break;
            }
        }

        mysql_close(conn);
    }
    else {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
    }
    system("pause");
}

void ViewProducts() {
    system("cls");

    int choice;  // User's choice for viewing products
    int itemsPerPage = 20;  // Number of items per page
    int page = 1;           // Start at page 1
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        while (true) {
            // Display a menu to the user
            system("cls");
            cout << "\n\n\n\t\t\t\t\t\t\t\t\tProduct View Options\n";
            cout << "\t\t\t\t\t\t\t\t\t--------------------\n\n\n";
            cout << "1. View All Products\n";
            cout << "2. View Products by Category\n";
            cout << "0. Return to Main Menu\n";  // Return to main menu
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) {
                // Query to get the total number of products
                string countQuery = "SELECT COUNT(*) FROM `menu item`";
                const char* countQ = countQuery.c_str();
                int qstate = mysql_query(conn, countQ);
                int totalItems = 0;
                if (!qstate) {
                    res = mysql_store_result(conn);
                    row = mysql_fetch_row(res);
                    totalItems = atoi(row[0]);
                }
                else {
                    cerr << "Error retrieving total item count: " << mysql_error(conn) << endl;
                    break;
                }

                int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;  // Calculate total pages

                while (true) {
                    // Query to get products for the current page
                    string query = "SELECT * FROM `menu item` ORDER BY CAST(SUBSTRING(ItemID, 2) AS UNSIGNED) ASC "
                        "LIMIT " + to_string(itemsPerPage) + " OFFSET " + to_string((page - 1) * itemsPerPage);
                    const char* q = query.c_str();
                    qstate = mysql_query(conn, q);

                    if (!qstate) {
                        res = mysql_store_result(conn); // Only use res if query is successful
                        system("cls");

                        cout << "\n\n\n\t\t\t\t\t\t\t\t\tView All Menu\n";
                        cout << "\t\t\t\t\t\t\t\t\t-------------\n";
                        cout << "\n\n\n-----------------------------------------------------------------------------------\n";
                        cout << std::left << std::setw(10) << "ID"
                            << std::setw(50) << "Name"
                            << std::setw(10) << "Price"
                            << std::setw(15) << "Category" << endl;
                        cout << "-----------------------------------------------------------------------------------\n";

                        cout << std::fixed << std::setprecision(2);  // Set precision for price

                        // Display the products for the current page
                        bool productsDisplayed = false;
                        while ((row = mysql_fetch_row(res))) {
                            cout << std::left << std::setw(10) << (row[0] ? row[0] : "")  // ItemID
                                << std::setw(50) << (row[1] ? row[1] : "")               // ItemName
                                << std::setw(10) << (row[2] ? row[2] : "")               // ItemPrice
                                << std::setw(15) << (row[3] ? row[3] : "") << "\n";      // ItemCategory
                            productsDisplayed = true;
                        }

                        cout << "--------------------------------------------------------------------------------------\n";

                        if (!productsDisplayed) {
                            cout << "No products available." << endl;
                        }

                        mysql_free_result(res);  // Free the result set

                        // Display the current page and total pages
                        cout << "\nPage " << page << " of " << totalPages << "\n";
                        // Display the current page and total pages
                        cout << "\nNavigation: ";
                        if (page > 1) {
                            cout << "[P] Previous ";
                        }
                        if (page < totalPages) {
                            cout << "[N] Next ";
                        }
                        cout << "[E] Exit\n";
                        cout << "Choose an option: ";

                        char navigationChoice;
                        cin >> navigationChoice;

                        if (navigationChoice == 'P' || navigationChoice == 'p') {
                            if (page > 1) {
                                page--;  // Go to the previous page
                            }
                        }
                        else if (navigationChoice == 'N' || navigationChoice == 'n') {
                            if (page < totalPages) {
                                page++;  // Go to the next page
                            }
                        }
                        else if (navigationChoice == 'E' || navigationChoice == 'e') {
                            break;  // Exit the loop and return to the previous menu
                        }
                        else {
                            cout << "Invalid choice. Please try again.\n";
                            system("pause");
                        }

                    }
                    else {
                        cerr << "Query failed: " << mysql_error(conn) << std::endl;
                        break;
                    }
                }
            }
            else if (choice == 2) {
                // View Products by Category
                string category;
                vector<string> categories;  // To store category names

                // Fetch the available categories from the database
                string categoriesQuery = "SELECT DISTINCT ItemCategory FROM `menu item`";
                int qstate = mysql_query(conn, categoriesQuery.c_str());

                if (!qstate) {
                    res = mysql_store_result(conn);
                    while ((row = mysql_fetch_row(res))) {
                        categories.push_back(row[0] ? row[0] : "");
                    }
                }
                else {
                    cerr << "Error fetching categories: " << mysql_error(conn) << endl;
                    mysql_close(conn);
                    return;
                }

                // Display categories and allow the user to choose one
                while (true) {
                    system("cls");
                    cout << "\n\n\n\t\t\t\t\t\t\t\t\tView By Category\n";
                    cout << "\t\t\t\t\t\t\t\t\t-----------------\n\n\n";

                    // Display categories
                    for (size_t i = 0; i < categories.size(); i++) {
                        cout << i + 1 << ". " << categories[i] << endl;
                    }

                    cout << "Enter the number of the category you want to view (or q to quit): ";
                    string input;
                    cin >> input;

                    // Check if user wants to quit and return to "View Products"
                    if (input == "q" || input == "Q") {
                        break; // Exit this function and go back to "View Products"
                    }

                    int categoryChoice = stoi(input);  // Convert input to integer

                    // Validate category selection
                    if (categoryChoice <= 0 || categoryChoice > categories.size()) {
                        cerr << "Invalid category choice!" << endl;
                        return;
                    }

                    size_t index = static_cast<size_t>(categoryChoice - 1);  // Cast to size_t to avoid overflow
                    category = categories[index];

                    // Pagination setup
                    int page = 1;
                    int itemsPerPage = 10;  // Define number of items per page

                    while (true) {
                        // Query to get the menu items for the selected category
                        string query = "SELECT * FROM `menu item` WHERE ItemCategory = '" + category + "' "
                            "ORDER BY CAST(SUBSTRING(itemID, 2) AS UNSIGNED) ASC "
                            "LIMIT " + to_string(itemsPerPage) + " OFFSET " + to_string((page - 1) * itemsPerPage);

                        qstate = mysql_query(conn, query.c_str());

                        if (!qstate) {
                            res = mysql_store_result(conn);
                            system("cls");

                            // Display the items for the selected category
                            cout << "\n\n\n\t\t\t\t\t\t\t\t\tView By Category\n";
                            cout << "\t\t\t\t\t\t\t\t\t-----------------\n";
                            cout << "\n\n\nMenu Items in Category: " << category << endl;
                            cout << "------------------------------------------------------------------------\n";
                            cout << std::left << std::setw(10) << "ID"
                                << std::setw(50) << "Name"
                                << std::setw(10) << "Price" << endl; 
                            cout << "------------------------------------------------------------------------\n";
                            cout << std::fixed << std::setprecision(2);  // Set precision for price

                            while ((row = mysql_fetch_row(res))) {
                                cout << std::left << std::setw(10) << (row[0] ? row[0] : "")  // ItemID
                                    << std::setw(50) << (row[1] ? row[1] : "")              // ItemName
                                    << std::setw(10) << (row[2] ? row[2] : "") << "\n";     // ItemPrice
                            }
                            cout << "-------------------------------------------------------------------------\n";
                        }
                        else {
                            cerr << "Error retrieving menu items: " << mysql_error(conn) << endl;
                            mysql_close(conn);
                            return;
                        }

                        // Calculate total pages for the selected category
                        string countCategoryQuery = "SELECT COUNT(*) FROM `menu item` WHERE ItemCategory = '" + category + "'";
                        const char* countCategoryQ = countCategoryQuery.c_str();
                        qstate = mysql_query(conn, countCategoryQ);
                        int totalCategoryItems = 0;
                        if (!qstate) {
                            res = mysql_store_result(conn);
                            row = mysql_fetch_row(res);
                            totalCategoryItems = atoi(row[0]);
                        }
                        else {
                            cerr << "Error retrieving total item count: " << mysql_error(conn) << endl;
                            break;
                        }
                        int totalCategoryPages = (totalCategoryItems + itemsPerPage - 1) / itemsPerPage;

                        // Display current page and total pages
                        cout << "\nPage " << page << " of " << totalCategoryPages << "\n";
                        // Display the current page and total pages
                        cout << "\nNavigation: ";
                        if (page > 1) {
                            cout << "[P] Previous ";
                        }
                        if (page < totalCategoryPages) {
                            cout << "[N] Next ";
                        }
                        cout << "[E] Exit\n";
                        cout << "Choose an option: ";

                        char navigationChoice;
                        cin >> navigationChoice;

                        if (navigationChoice == 'P' || navigationChoice == 'p') {
                            if (page > 1) {
                                page--;  // Go to the previous page
                            }
                        }
                        else if (navigationChoice == 'N' || navigationChoice == 'n') {
                            if (page < totalCategoryPages) {
                                page++;  // Go to the next page
                            }
                        }
                        else if (navigationChoice == 'E' || navigationChoice == 'e') {
                            break;  // Exit the loop and return to the previous menu
                        }
                        else {
                            cout << "Invalid choice. Please try again.\n";
                            system("pause");
                        }

                    }
                }
            }
            else if (choice == 0) {
                break;  
            }
            else {
                cout << "Invalid choice, please try again." << endl;
            }
        }
        mysql_close(conn);  // Close the database connection
    }
    else {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
    }
    system("pause"); 
}

void ProductMenu() {
    int choice;

    do {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tProduct Management\n";
        cout << "\t\t\t\t\t\t\t\t\t------------------\n";
        cout << "\n\n\n1. Add Product\n";
        cout << "2. Edit Product\n";
        cout << "3. Update Menu\n";
        cout << "4. Delete Menu\n";
        cout << "0. Return to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            AddProduct();
            break;
        case 2:
            EditProduct();
            break;
        case 3:
            UpdateMenu();
            break;
        case 4:
            DeleteMenu();
            break;
        case 0:
            cout << "Returning to main menu...\n";
            break;
        default:
            cout << "Invalid choice, please try again.\n";
            break;
        }

    } while (choice != 0);  // Loop until user chooses to go back to the main menu
    system("pause");
}

void AddProduct() {
    system("cls");

    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "MySQL Connection Failed: " << mysql_error(conn) << endl;
        return;
    }

    string id, name, category;
    double price;

    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tAdd Product\n";
    cout << "\t\t\t\t\t\t\t\t\t--------------\n";

    cout << "\n\n\nEnter Item ID (e.g., M21): ";
    cin >> id;
    cin.ignore();  // To ignore the newline left in the input buffer
    cout << "Enter Item Name: ";
    getline(cin, name);
    cout << "Enter Category: ";
    getline(cin, category);

    string categoryCheckQuery = "SELECT COUNT(*) FROM `menu item` WHERE ItemCategory = '" + category + "'";
    const char* checkQuery = categoryCheckQuery.c_str();
    int checkState = mysql_query(conn, checkQuery);

    if (checkState) {
        cerr << "Error checking category: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int categoryCount = atoi(row[0]);

    if (categoryCount == 0) {
        cout << "\nCategory does not exist." << endl;
        cout << "Do you want to add this category? (Y/N): ";
        char addNewCategory;
        cin >> addNewCategory;
        cin.ignore();  // To clear the input buffer

        if (addNewCategory == 'Y' || addNewCategory == 'y') {
            string addCategoryQuery = "INSERT INTO `menu item` (ItemCategory) VALUES ('" + category + "')";
            const char* insertQuery = addCategoryQuery.c_str();
            int insertState = mysql_query(conn, insertQuery);

            if (insertState) {
                cerr << "Error adding category: " << mysql_error(conn) << endl;
                mysql_free_result(result);
                mysql_close(conn);
                return;
            }

            cout << "\nCategory added successfully!" << endl;
        }
        else {
            string getCategoriesQuery = "SELECT DISTINCT ItemCategory FROM `menu item`";
            const char* categoryQuery = getCategoriesQuery.c_str();
            int categoryListState = mysql_query(conn, categoryQuery);

            if (!categoryListState) {
                MYSQL_RES* categoryResult = mysql_store_result(conn);
                int categoryIndex = 1;

                cout << "\nAvailable Categories:" << endl;
                while (MYSQL_ROW categoryRow = mysql_fetch_row(categoryResult)) {
                    cout << setw(3) << left << categoryIndex++ << ". " << categoryRow[0] << endl;
                    if (categoryIndex % 10 == 0) {
                        waitForUser();
                    }
                }

                cout << "\nPlease choose a valid category by number: ";
                int categoryChoice;
                cin >> categoryChoice;
                cin.ignore();

                MYSQL_ROW selectedRow;
                mysql_data_seek(categoryResult, 0);
                for (int j = 1; j < categoryChoice; ++j) {
                    selectedRow = mysql_fetch_row(categoryResult);
                }
                selectedRow = mysql_fetch_row(categoryResult);
                category = selectedRow[0];

                cout << "You have selected category: " << category << endl;

                mysql_free_result(categoryResult);
            }
        }
    }

    cout << "Enter Price: ";
    cin >> price;

    // Use setprecision to format the price to 2 decimal places
    stringstream ss;
    ss << fixed << setprecision(2) << price;  // Ensures 2 decimal places
    string priceStr = ss.str();  // Store the formatted price as a string

    // Confirmation before adding the product
    cout << "\nYou are about to add the following product:" << endl;
    cout << "Item ID: " << id << endl;
    cout << "Item Name: " << name << endl;
    cout << "Category: " << category << endl;
    cout << "Price: RM " << priceStr << endl;
    cout << "Are you sure you want to add this product? (Y/N): ";
    char confirmAdd;
    cin >> confirmAdd;

    if (confirmAdd == 'Y' || confirmAdd == 'y') {
        string query = "INSERT INTO `menu item` (ItemID, ItemName, ItemPrice, ItemCategory) "
            "VALUES ('" + id + "', '" + name + "', " + to_string(price) + ", '" + category + "')";
        const char* q = query.c_str();
        int qstate = mysql_query(conn, q);

        if (!qstate) {
            cout << "\nMenu item added successfully!" << endl;
        }
        else {
            cerr << "\nFailed to add menu item: " << mysql_error(conn) << endl;
        }
    }
    else {
        cout << "Product addition cancelled." << endl;
    }

    mysql_free_result(result);
    mysql_close(conn);
    system("pause");
}

void EditProduct() {
    system("cls");

    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        string id, field, newValue;

        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tEdit product\n";
        cout << "\t\t\t\t\t\t\t\t\t--------------\n";
        cout << "\n\n\nEnter Item ID to update: ";
        cin >> id;
        cout << "Enter field to update (ItemName, ItemPrice, ItemCategory): ";
        cin >> field;
        cout << "Enter new value: ";
        cin.ignore();
        getline(cin, newValue);

        // If the field is ItemPrice, format the newValue to 2 decimal places
        if (field == "ItemPrice") {
            double price = stod(newValue);  // Convert string to double

            // Use stringstream to format the price to 2 decimal places
            stringstream ss;
            ss << fixed << setprecision(2) << price;  // Ensures 2 decimal places
            newValue = ss.str();  // Store formatted price as a string
        }

        // Confirmation before updating the product
        cout << "\nYou are about to update the following details:" << endl;
        cout << "Item ID: " << id << endl;
        cout << "Field: " << field << endl;
        cout << "New Value: " << newValue << endl;
        cout << "Are you sure you want to update this product? (Y/N): ";
        char confirmUpdate;
        cin >> confirmUpdate;

        if (confirmUpdate == 'Y' || confirmUpdate == 'y') {
            string query = "UPDATE `menu item` SET " + field + " = '" + newValue + "' WHERE ItemID = '" + id + "'";
            const char* q = query.c_str();
            int qstate = mysql_query(conn, q);

            if (!qstate) {
                cout << "Menu item updated successfully!" << endl;
            }
            else {
                cerr << "Failed to update menu item: " << mysql_error(conn) << endl;
            }
        }
        else {
            cout << "Update cancelled." << endl;
        }
        mysql_close(conn);
    }
    system("pause");
}

void UpdateMenu() {
    system("cls");
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "MySQL Connection Failed: " << mysql_error(conn) << endl;
        return;
    }

    string id;
    string name;
    string category;
    double price;

    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tUpdate Menu\n";
    cout << "\t\t\t\t\t\t\t\t\t--------------\n";

    cout << "\n\n\nEnter the ID of the menu item you want to update: ";
    cin >> id;

    cout << "Enter new name (leave empty to keep unchanged): ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter new category (leave empty to keep unchanged): ";
    getline(cin, category);

    cout << "Enter new price (enter -1 to keep unchanged): ";
    cin >> price;

    // Confirmation before updating the product
    cout << "\nYou are about to update the following details:" << endl;
    cout << "Item ID: " << id << endl;
    cout << "New Name: " << (name.empty() ? "(No change)" : name) << endl;
    cout << "New Category: " << (category.empty() ? "(No change)" : category) << endl;

    if (price == -1) {
        cout << "New Price: (No change)" << endl;
    }
    else {
        // Format price to 2 decimal places using stringstream
        stringstream ss;
        ss << fixed << setprecision(2) << price;
        cout << "New Price: " << ss.str() << endl;
    }

    cout << "Are you sure you want to update this menu item? (Y/N): ";
    char confirmUpdate;
    cin >> confirmUpdate;

    if (confirmUpdate == 'Y' || confirmUpdate == 'y') {
        if (!name.empty()) {
            string query = "UPDATE `menu item` SET ItemName = '" + name + "' WHERE ItemID = '" + id + "'";
            if (mysql_query(conn, query.c_str())) {
                cerr << "Failed to update name: " << mysql_error(conn) << endl;
            }
            else {
                cout << "Name updated successfully." << endl;
            }
        }

        if (!category.empty()) {
            string query = "UPDATE `menu item` SET ItemCategory = '" + category + "' WHERE ItemID = '" + id + "'";
            if (mysql_query(conn, query.c_str())) {
                cerr << "Failed to update category: " << mysql_error(conn) << endl;
            }
            else {
                cout << "Category updated successfully." << endl;
            }
        }

        if (price != -1) {
            // Format price to 2 decimal places using stringstream
            stringstream ss;
            ss << fixed << setprecision(2) << price;
            string priceStr = ss.str();  // Store formatted price as a string

            string query = "UPDATE `menu item` SET ItemPrice = " + priceStr + " WHERE ItemID = '" + id + "'";
            if (mysql_query(conn, query.c_str())) {
                cerr << "Failed to update price: " << mysql_error(conn) << endl;
            }
            else {
                cout << "Price updated successfully." << endl;
            }
        }
    }
    else {
        cout << "Update cancelled." << endl;
    }
    mysql_close(conn);
    system("pause");
}

void DeleteMenu() {
    system("cls");
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        string id;

        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tDelete Menu\n";
        cout << "\t\t\t\t\t\t\t\t\t--------------\n";

        cout << "\n\n\nEnter Item ID to delete: ";
        cin >> id;

        // Fetch the item details based on ItemID
        string fetchQuery = "SELECT ItemID, ItemName, ItemPrice, ItemCategory FROM `menu item` WHERE ItemID = '" + id + "'";
        const char* fetchQueryCStr = fetchQuery.c_str();
        int fetchState = mysql_query(conn, fetchQueryCStr);

        if (fetchState) {
            cerr << "Failed to fetch item details: " << mysql_error(conn) << endl;
            mysql_close(conn);
            system("pause");
            return;
        }

        MYSQL_RES* result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);

        if (row) {
            // Display the fetched item details
            cout << "\nItem Details:\n";
            cout << "Item ID: " << row[0] << endl;
            cout << "Item Name: " << row[1] << endl;
            cout << "Item Price: " << row[2] << endl;
            cout << "Item Category: " << row[3] << endl;

            // Confirm if the user wants to delete this item
            char confirmDelete;
            cout << "\nAre you sure you want to delete this item? (Y/N): ";
            cin >> confirmDelete;

            if (confirmDelete == 'Y' || confirmDelete == 'y') {
                // Delete the item if confirmed
                string deleteQuery = "DELETE FROM `menu item` WHERE ItemID = '" + id + "'";
                const char* deleteQueryCStr = deleteQuery.c_str();
                int deleteState = mysql_query(conn, deleteQueryCStr);

                if (!deleteState) {
                    cout << "Menu item deleted successfully!" << endl;
                }
                else {
                    cerr << "Failed to delete menu item: " << mysql_error(conn) << endl;
                }
            }
            else {
                cout << "Deletion cancelled." << endl;
            }
        }
        else {
            cerr << "Item with ID " << id << " not found." << endl;
        }

        mysql_free_result(result);
        mysql_close(conn);
    }
    else {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
    }
    system("pause");
}

void OrderStatus() {
    system("cls");

    // Connect to the database
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    int choice;
    string orderStatusFilter = "";
    const int PAGE_SIZE = 20; // Number of items per page

    // Main menu for order status selection
    do {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\t   Order\n";
        cout << "\t\t\t\t\t\t\t\t\t   -----\n";
        cout << "\n\n\n1. View Pending Orders\n";
        cout << "2. View Paid Orders\n";
        cout << "3. View All Orders\n";
        cout << "0. Return to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            orderStatusFilter = "'Pending'";
            break;
        case 2:
            orderStatusFilter = "'Paid'";
            break;
        case 3:
            orderStatusFilter = "ALL";
            break;
        case 0:
            cout << "Returning to the previous menu..." << endl;
            mysql_close(conn);
            return;
        default:
            cout << "Invalid choice. Please try again." << endl;
            continue;
        }

        int currentPage = 1;
        int totalOrders = 0;

        // Calculate total number of orders
        string countQuery = "SELECT COUNT(*) FROM orderr";
        if (orderStatusFilter != "ALL") {
            countQuery += " WHERE OrderStatus = " + orderStatusFilter;
        }
        if (mysql_query(conn, countQuery.c_str()) == 0) {
            MYSQL_RES* countRes = mysql_store_result(conn);
            MYSQL_ROW countRow = mysql_fetch_row(countRes);
            totalOrders = stoi(countRow[0]);
            mysql_free_result(countRes);
        }

        int totalPages = (totalOrders + PAGE_SIZE - 1) / PAGE_SIZE; // Calculate total pages

        while (true) {
            system("cls");

            // Query to fetch orders with pagination and sorting
            string query = "SELECT o.OrderID, o.OrderStatus, o.OrderTotalPrice, o.OrderDate, m.ItemName, oi.OrderQuantity "
                "FROM orderr o "
                "JOIN `order item` oi ON o.OrderID = oi.OrderID "
                "JOIN `menu item` m ON oi.ItemID = m.ItemID";

            if (orderStatusFilter != "ALL") {
                query += " WHERE o.OrderStatus = " + orderStatusFilter;
            }

            query += " ORDER BY o.OrderDate DESC, o.OrderID DESC "
                "LIMIT " + to_string((currentPage - 1) * PAGE_SIZE) + ", " + to_string(PAGE_SIZE);

            // Execute the query
            if (mysql_query(conn, query.c_str()) != 0) {
                cerr << "Error fetching orders: " << mysql_error(conn) << endl;
                mysql_close(conn);
                return;
            }

            res = mysql_store_result(conn);

            // Display current page orders
            system("cls");
            cout << "\n\n\n\t\t\t\t\t\t\t\t\t   Order\n";
            cout << "\t\t\t\t\t\t\t\t\t   -----\n";
            cout << "\n\n\n---------------------------------------------------------------------------------------------------------------------------------------\n";
            cout << std::left << std::setw(15) << "Order ID"
                << std::setw(20) << "Order Status"
                << std::setw(50) << "Item Name"
                << std::setw(15) << "Quantity"
                << std::setw(20) << "Total Price"
                << std::setw(20) << "Order Date" << endl;
            cout << "---------------------------------------------------------------------------------------------------------------------------------------\n";
            cout << std::fixed << std::setprecision(2);

            while ((row = mysql_fetch_row(res))) {
                cout << std::left << std::setw(15) << row[0] // OrderID
                    << std::setw(20) << row[1] // OrderStatus
                    << std::setw(50) << row[4] // ItemName
                    << std::setw(15) << row[5] // Quantity
                    << std::setw(20) << row[2] // TotalPrice
                    << std::setw(20) << row[3] << endl; // OrderDate
            }
            mysql_free_result(res);
            cout << "---------------------------------------------------------------------------------------------------------------------------------------\n";
            cout << "\nPage " << currentPage << " of " << totalPages << "\n";

            // Pagination controls
            cout << "\nNavigation: ";
            if (currentPage > 1) {
                cout << "[P] Previous ";
            }
            if (currentPage < totalPages) {
                cout << "[N] Next ";
            }
            cout << "[E] Exit\n";
            cout << "Choose an option: ";

            char navChoice;
            cin >> navChoice;

            if (navChoice == 'P' || navChoice == 'p') {
                if (currentPage > 1) currentPage--;
            }
            else if (navChoice == 'N' || navChoice == 'n') {
                if (currentPage < totalPages) currentPage++;
            }
            else if (navChoice == 'E' || navChoice == 'e') {
                break;
            }
            else {
                cout << "Invalid choice. Please try again.\n";
                system("pause");
            }
        }
    } while (true);
    // Close the connection to the database
    mysql_close(conn);
}

void GenerateReport() {
    system("cls");

    int reportType;
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tReport\n";
    cout << "\t\t\t\t\t\t\t\t\t---------\n";
    cout << "\n\n\nSelect Report Type:" << endl;
    cout << "1. Daily Report" << endl;
    cout << "2. Monthly Report" << endl;
    cout << "3. Category Report" << endl;
    cout << "4. View by Order Status" << endl;
    cout << "0. Return to Main Menu" << endl;
    cout << "Enter your choice (1/2/3/4/0): ";
    cin >> reportType;

    if (reportType == 0) {
        cout << "Returning to Main Menu..." << endl;
        return;
    }

    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    int currentPage = 1, totalRows = 0, pageSize = 20;
    string query, filterQuery = "";
    string fromDate, categoryFilter, orderStatusFilter;
    int month, year;

    // Ask user if they want to save the report
    ofstream file;
    bool saveToFile = false;
    char saveChoice;
    cout << "\nDo you want to download the report as a CSV file? (y/n): ";
    cin >> saveChoice;

    if (tolower(saveChoice) == 'y') {
        file.open("Report.csv");
        if (!file) {
            cerr << "Error creating the report file!" << endl;
            mysql_close(conn);
            return;
        }
        file << "OrderID,OrderDate,ItemName,Quantity,TotalAmount\n";
        saveToFile = true;
        cout << "\nReport successfully saved as 'Report.csv'.\n";
        
    }

    while (true) {
        if (reportType == 1) {
            // Daily Report
            if (currentPage == 1) {
                cout << "Enter the date for the report (YYYY-MM-DD): ";
                cin >> fromDate;
            }
            filterQuery = "WHERE o.OrderDate = '" + fromDate + "'";
        }
        else if (reportType == 2) {
            // Monthly Report
            if (currentPage == 1) {
                cout << "Enter the month (1-12): ";
                cin >> month;
                cout << "Enter the year (YYYY): ";
                cin >> year;
            }
            filterQuery = "WHERE YEAR(o.OrderDate) = " + to_string(year) +
                " AND MONTH(o.OrderDate) = " + (month < 10 ? "0" + to_string(month) : to_string(month));
        }
        else if (reportType == 3) {
            // Category Report
            if (currentPage == 1) {
                cout << "Enter the food category (e.g., Drinks, Appetizers): ";
                cin.ignore();
                getline(cin, categoryFilter);
            }
            filterQuery = "WHERE m.ItemCategory = '" + categoryFilter + "'";
        }
        else if (reportType == 4) {
            // View by Order Status
            if (currentPage == 1) {
                cout << "Available Order Statuses: [Paid, Pending, Cancelled]\n";
                cout << "Enter the status: ";
                cin.ignore();
                getline(cin, orderStatusFilter);
            }
            filterQuery = "WHERE o.OrderStatus = '" + orderStatusFilter + "'";
        }
        else {
            cout << "Invalid choice!" << endl;
            break;
        }

        // Fetch total rows for pagination
        if (currentPage == 1) {
            query = "SELECT COUNT(*) FROM orderr o "
                "JOIN `order item` oi ON o.OrderID = oi.OrderID "
                "JOIN `menu item` m ON oi.ItemID = m.ItemID " + filterQuery;
            mysql_query(conn, query.c_str());
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            totalRows = row ? atoi(row[0]) : 0;
            mysql_free_result(res);
        }

        // Fetch current page data
        query = "SELECT o.OrderID, o.OrderDate, m.ItemName, oi.OrderQuantity, "
            "(oi.OrderQuantity * m.ItemPrice) AS TotalAmount "
            "FROM orderr o "
            "JOIN `order item` oi ON o.OrderID = oi.OrderID "
            "JOIN `menu item` m ON oi.ItemID = m.ItemID " + filterQuery +
            " ORDER BY o.OrderDate ASC, o.OrderID ASC "
            "LIMIT " + to_string(pageSize) + " OFFSET " + to_string((currentPage - 1) * pageSize);
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);

        cout << "\nReport";
        cout << "\n-------------------------------------------------------------------------------------------------------------------\n";
        cout << setw(15) << "Order ID" << setw(20) << "Order Date" << setw(50) << "Item Name"
            << setw(15) << "Quantity" << setw(15) << "Total Amount" << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------\n";

        if (mysql_num_rows(res) > 0) {
            while ((row = mysql_fetch_row(res))) {
                cout << setw(15) << (row[0] ? row[0] : "N/A")
                    << setw(20) << (row[1] ? row[1] : "N/A")
                    << setw(50) << (row[2] ? row[2] : "N/A")
                    << setw(15) << (row[3] ? row[3] : "N/A")
                    << setw(15) << (row[4] ? row[4] : "N/A") << endl;

                if (saveToFile) {
                    file << (row[0] ? row[0] : "") << ","
                        << (row[1] ? row[1] : "") << ","
                        << (row[2] ? row[2] : "") << ","
                        << (row[3] ? row[3] : "") << ","
                        << (row[4] ? row[4] : "") << "\n";
                }
            }
        }
        else {
            cout << "No records found.\n";
        }
        mysql_free_result(res);

        // Pagination options
        cout << "---------------------------------------------------------------------------------------------------------\n";
        cout << "Report Results (Page " << currentPage << " of " << (totalRows + pageSize - 1) / pageSize << "):\n";
        cout << "Navigation: ";
        if (currentPage > 1) {
            cout << "[P] Previous ";
        }
        if (currentPage * pageSize < totalRows) {
            cout << "[N] Next ";
        }
        cout << "[E] Exit\n";
        cout << "Choose an option: ";

        char navChoice;
        cin >> navChoice;

        if (navChoice == 'P' || navChoice == 'p') {
            if (currentPage > 1) {
                currentPage--;
            }
        }
        else if (navChoice == 'N' || navChoice == 'n') {
            if (currentPage * pageSize < totalRows) {
                currentPage++;
            }
        }
        else if (navChoice == 'E' || navChoice == 'e') {
            break;
        }
        else {
            cout << "Invalid choice.\n";
        }
    }

    if (saveToFile) {
        file.close();
        cout << "\nReport successfully saved as 'Report.csv'.\n";
    }
    mysql_close(conn);
}

void GenerateFeedbackReport() {
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Database connection failed: " << mysql_error(conn) << endl;
        return;
    }

    system("cls");
    cout << "\n\n\t\t\tFeedback Report\n";
    cout << "\t\t\t-------------\n";

    string filterChoice;
    cout << "\nChoose a filter option:\n"
        << "1. Filter by Customer ID\n"
        << "2. Filter by Date Range\n"
        << "3. Filter by Rating\n"
        << "4. No Filter (Show All Feedback)\n"
        << "0. Return to Main Menu\n"
        << "Enter your choice: ";
    cin >> filterChoice;
    cin.ignore();

    if (filterChoice == "0") {
        cout << "Returning to Main Menu..." << endl;
        mysql_close(conn);
        return;
    }

    string feedbackQuery = "SELECT f.FeedbackID, f.CustomerID, f.OrderID, f.Rating, f.Comments, f.FeedbackDate, "
        "o.OrderDate FROM feedback f "
        "JOIN orderr o ON f.OrderID = o.OrderID ";
    string filterCondition;

    if (filterChoice == "1") {
        string customerID;
        cout << "Enter CustomerID to filter: ";
        getline(cin, customerID);
        filterCondition = "WHERE f.CustomerID = '" + customerID + "' ";
    }
    else if (filterChoice == "2") {
        string startDate, endDate;
        while (true) {
            cout << "Enter start date (YYYY-MM-DD): ";
            getline(cin, startDate);
            cout << "Enter end date (YYYY-MM-DD): ";
            getline(cin, endDate);

            if (startDate > endDate) {
                cout << "\nError: Start date cannot be later than end date. Please re-enter.\n";
            }
            else {
                break;
            }
        }
        filterCondition = "WHERE f.FeedbackDate BETWEEN '" + startDate + "' AND '" + endDate + "' ";
    }
    else if (filterChoice == "3") {
        int minRating;
        cout << "Enter minimum rating (1-5): ";
        cin >> minRating;
        while (minRating < 1 || minRating > 5) {
            cout << "Invalid rating. Please enter a value between 1 and 5: ";
            cin >> minRating;
        }
        filterCondition = "WHERE f.Rating >= " + to_string(minRating) + " ";
    }

    feedbackQuery += filterCondition + "ORDER BY f.FeedbackDate DESC";

    if (mysql_query(conn, feedbackQuery.c_str())) {
        cerr << "Error fetching feedback report: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cerr << "Error storing feedback result: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    char saveChoice;
    cout << "\nDo you want to download the report as a CSV file? (y/n): ";
    cin >> saveChoice;

    ofstream file;
    if (tolower(saveChoice) == 'y') {
        file.open("FeedbackReport.csv");
        if (!file) {
            cerr << "Error creating the feedback report file!" << endl;
            mysql_free_result(res);
            mysql_close(conn);
            return;
        }
        file << "FeedbackID,CustomerID,OrderID,Rating,Comments,FeedbackDate,OrderDate\n";
    }

    cout << "\nFeedback Report\n";
    cout << "---------------------------------------------------------------------------------------------\n";
    cout << left << setw(10) << "ID"
        << setw(15) << "CustomerID"
        << setw(10) << "OrderID"
        << setw(8) << "Rating"
        << setw(50) << "Comments"
        << setw(20) << "FeedbackDate"
        << setw(20) << "OrderDate" << endl;
    cout << "---------------------------------------------------------------------------------------------\n";

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << (row[0] ? row[0] : "N/A")
            << setw(15) << (row[1] ? row[1] : "N/A")
            << setw(10) << (row[2] ? row[2] : "N/A")
            << setw(8) << (row[3] ? row[3] : "N/A")
            << setw(50) << (row[4] ? row[4] : "N/A")
            << setw(20) << (row[5] ? row[5] : "N/A")
            << setw(20) << (row[6] ? row[6] : "N/A") << endl;

        if (tolower(saveChoice) == 'y') {
            file << (row[0] ? row[0] : "N/A") << ","
                << (row[1] ? row[1] : "N/A") << ","
                << (row[2] ? row[2] : "N/A") << ","
                << (row[3] ? row[3] : "N/A") << ","
                << '"' << (row[4] ? row[4] : "N/A") << '"' << ","
                << (row[5] ? row[5] : "N/A") << ","
                << (row[6] ? row[6] : "N/A") << "\n";
        }
    }

    cout << "---------------------------------------------------------------------------------------------\n";

    if (file.is_open()) {
        file.close();
        cout << "\nFeedback report has been saved as 'FeedbackReport.csv'.\n";
    }

    mysql_free_result(res);
    mysql_close(conn);
    system("pause");
}

void ShowCustomerMenu() {
    system("cls");
    system("Color F4");
    int choice;
    bool inCustomerMenu = true;

    while (inCustomerMenu) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t     ========================================================================\n\t\t\t\n\n";
        cout << "\t\t\t\t\t     -------------         C U S T O M E R  M E N U             -------------\n\t\t\t\n\n";
        cout << "\t\t\t\t\t     ========================================================================\t\t\t\n";
        cout << "\t\t\t                                                               \n\t\t\t\n\n";
        cout << "\t\t\t\t\t\t\t        --------------------------- " << endl;
        std::cout << "\t\t\t\t\t\t\t       | 1. Profile               |\n";  
        std::cout << "\t\t\t\t\t\t\t       | 2. Search Products       |\n";  
        std::cout << "\t\t\t\t\t\t\t       | 3. View Products         |\n";  
        std::cout << "\t\t\t\t\t\t\t       | 4. Cart                  |\n";
        std::cout << "\t\t\t\t\t\t\t       | 5. Order Detail          |\n";
        std::cout << "\t\t\t\t\t\t\t       | 6. Provide Feedback      |\n";  
        std::cout << "\t\t\t\t\t\t\t       | 7. Logout                |\n";
        cout << "\t\t\t\t\t\t\t        --------------------------- " << endl;
        std::cout << "\n\t\t\t\t\t\t\t\t Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            EditCustomerProfile();  // Calls the EditCustomerProfile function
            break;
        case 2:
            SearchBar();  // Calls the SearchBar function
            break;
        case 3:
            ViewProducts();  // Call ViewProducts when the user selects "View Products"
            break;
        case 4:
            ManageCart();  // Call ManageCart for cart-related actions
            break;
        case 5:
            OrderDetail();  // Displays details of the customer's orders
            break;
        case 6: {
            // Call the feedback function
            Feedback();
            break;
        }
        case 7:
            std::cout << "Logging out...\n";
            inCustomerMenu = false;  // Exit customer menu
            return;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    }
}

void EditCustomerProfile() {
    system("cls");
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    // Use loggedInCustomerID instead of asking for CustomerID
    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    // Check database connection
    if (!conn) {
        cerr << "Database connection failed: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch Customer Profile
    string currentName, currentTelNum;  // Variables to store fetched data
    string fetchQuery = "SELECT CustName, CustTelNum FROM customer WHERE CustomerID = '" + customerID + "'";
    if (mysql_query(conn, fetchQuery.c_str())) {
        cerr << "Error fetching profile: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row) {
        cout << "No profile found for Customer ID: " << customerID << ".\n";
        mysql_free_result(res);
        mysql_close(conn);
        system("pause");
        return;
    }

    // Store the fetched data into local variables
    currentName = row[0] ? row[0] : "N/A";
    currentTelNum = row[1] ? row[1] : "N/A";

    mysql_free_result(res); // Free the result set

    // Display current profile details
    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\tCustomer Profile\n";
    cout << "\t\t\t\t\t\t\t---------------";
    cout << "\n\n\nCurrent Profile Information:\n";
    cout << "1. Name: " << currentName << "\n";
    cout << "2. Telephone Number: " << currentTelNum << "\n";

    // Authenticate Before Edit
    string password;
    int attempts = 3;

    while (attempts > 0) {
        cout << "Enter your password to proceed: ";
        cin >> password;

        string hashedPassword = hashPassword(password); // Assuming hashPassword is defined elsewhere
        string validatePasswordQuery = "SELECT * FROM customer WHERE CustomerID = '" + customerID + "' AND CustPassword = '" + password + "'";
        if (mysql_query(conn, validatePasswordQuery.c_str())) {
            cerr << "Error validating password: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) {
            cout << "\nPassword verified. Proceeding with profile update...\n";
            mysql_free_result(res);
            break;
        }
        else {
            attempts--;
            cout << "Incorrect password. You have " << attempts << " attempt(s) left.\n";
            if (attempts == 0) {
                cout << "You have reached the maximum number of attempts. Please contact customer service to reset your password.\n";
                mysql_close(conn);
                system("pause");
                return;
            }
        }
        mysql_free_result(res);
    }

    // Edit Profile
    string newName, newTelNum, newPassword;
    cout << "\nYou can now edit your profile.\n";

    // Prompt for new details
    cout << "New Name (leave blank to keep unchanged): ";
    cin.ignore();
    getline(cin, newName);

    cout << "New Telephone Number (60+) (leave blank to keep unchanged): ";
    getline(cin, newTelNum);

    // Validate phone number if provided
    if (!newTelNum.empty() && !isValidPhoneNumber(newTelNum)) {
        cout << "Invalid phone number. Please enter a valid digit phone number.\n";
        mysql_close(conn);
        return;
    }

    while (true) {
        cout << "New Password (leave blank to keep unchanged): ";
        getline(cin, newPassword);

        if (newPassword.empty()) break; // Skip password update if left blank

        if (isValidPassword(newPassword)) {
            newPassword = hashPassword(newPassword); // Hash new password
            break;
        }
        else {
            cout << "Invalid password. Password must contain at least 8 characters, including uppercase, lowercase, numbers, and symbols.\n";
        }
    }

    // Display updated profile
    cout << "\nUpdated Profile Information:\n";
    cout << "1. Name: " << (newName.empty() ? currentName : newName) << "\n";  // Use currentName if newName is empty
    cout << "2. Telephone Number: " << (newTelNum.empty() ? currentTelNum : newTelNum) << "\n";  // Use currentTelNum if newTelNum is empty
    cout << "3. Password: " << (newPassword.empty() ? "Unchanged" : "******** (hidden)") << "\n";

    // Confirm changes
    char confirm;
    cout << "\nConfirm changes? (y/n): ";
    cin >> confirm;
    if (tolower(confirm) != 'y') {
        cout << "Changes discarded.\n";
        mysql_close(conn);
        return;
    }

    // Build update query
    string updateQuery = "UPDATE customer SET ";
    if (!newName.empty()) updateQuery += "CustName = '" + newName + "', ";
    if (!newTelNum.empty()) updateQuery += "CustTelNum = '" + newTelNum + "', ";
    if (!newPassword.empty()) updateQuery += "CustPassword = '" + newPassword + "', ";

    // Remove trailing comma and space, add condition
    updateQuery = updateQuery.substr(0, updateQuery.size() - 2);
    updateQuery += " WHERE CustomerID = '" + customerID + "'";

    // Execute update query
    if (mysql_query(conn, updateQuery.c_str())) {
        cerr << "Error updating profile: " << mysql_error(conn) << endl;
    }
    else {
        cout << "\nProfile updated successfully!\n";
    }
    mysql_close(conn);
    system("pause");
}

void ManageCart() {
    MYSQL* conn;

    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    int mainChoice;
    while (true) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tManage Cart\n";
        cout << "\t\t\t\t\t\t\t\t\t-------------\n";
        cout << "\n\n\n1. Add to Cart\n";
        cout << "2. View Cart\n";
        cout << "3. Remove from Cart\n";
        cout << "4. Update Cart Quantity\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> mainChoice;

        switch (mainChoice) {
        case 1:
            AddToCart();
            break;
        case 2:
            ViewCart();
            break;
        case 3:
            RemoveFromCart();
            break;
        case 4:
            UpdateCartQuantity();
            break;
        case 0:
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice, please try again.\n";
            break;
        }
    }
}

void AddToCart() {
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;

    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    vector<string> categories;
    string category;
    int itemsPerPage = 20;
    int page = 1;
    char navigationChoice;

    // Fetch categories
    string categoriesQuery = "SELECT DISTINCT ItemCategory FROM `menu item`";
    int qstate = mysql_query(conn, categoriesQuery.c_str());
    if (!qstate) {
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res))) {
            categories.push_back(row[0] ? row[0] : "");
        }
    }
    else {
        cerr << "Error fetching categories: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    while (true) {
        // Display categories
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tAdd To Cart\n";
        cout << "\t\t\t\t\t\t\t\t\t--------------\n";
        cout << "\n\n\nAvailable Categories:\n";
        for (size_t i = 0; i < categories.size(); i++) {
            cout << i + 1 << ". " << categories[i] << endl;
        }

        cout << "Enter the number of the category you want to view (or 0 to go back): ";
        int categoryChoice;
        cin >> categoryChoice;

        cout << "Category Choice: " << categoryChoice << endl;

        if (categoryChoice == 0) {
            return;
        }

        // Check if the input is valid and within the range of categories
        if (categoryChoice > 0 && categoryChoice <= categories.size()) {
            // Use static_cast to ensure safe indexing with size_t
            size_t index = static_cast<size_t>(categoryChoice - 1);
            category = categories[index];
        }
        else {
            cout << "Invalid category choice!" << endl;
            return;
        }
        while (true) {
            // Query to get the menu items for the selected category
            string query = "SELECT * FROM `menu item` WHERE ItemCategory = '" + category + "' "
                "ORDER BY CAST(SUBSTRING(itemID, 2) AS UNSIGNED) ASC "
                "LIMIT " + to_string(itemsPerPage) + " OFFSET " + to_string((page - 1) * itemsPerPage);

            qstate = mysql_query(conn, query.c_str());

            if (!qstate) {
                res = mysql_store_result(conn);
                system("cls");
                cout << "\n\n\n\t\t\t\t\t\t\t\t\tAdd To Cart\n";
                cout << "\t\t\t\t\t\t\t\t\t--------------\n";
                cout << "\n\n\nMenu Items in Category: " << category << endl;
                cout << "------------------------------------------------------------------------------------------------------\n";
                cout << std::left << std::setw(10) << "ID"
                    << std::setw(50) << "Name"
                    << std::setw(10) << "Price" << endl;
                cout << "-----------------------------------------------------------------------------------------------------\n";
                cout << std::fixed << std::setprecision(2); // Set precision for price

                while ((row = mysql_fetch_row(res))) {
                    cout << std::left << std::setw(10) << (row[0] ? row[0] : "")
                        << std::setw(50) << (row[1] ? row[1] : "")
                        << std::setw(10) << (row[2] ? row[2] : "") << endl;
                }
                cout << "-----------------------------------------------------------------------------------------------------\n";
            }
            else {
                cerr << "Error retrieving menu items: " << mysql_error(conn) << endl;
                mysql_close(conn);
                return;
            }

            // Ask user to add items to the cart
            string itemID;
            int orderQuantity;

            cout << "Enter Product ID to add to cart (or 'S' to skip this page): ";
            cin >> itemID;

            if (itemID == "S" || itemID == "s") {
                cout << "Skipping this page...\n";
                cout << "\nEnter 'N' for next page, 'P' for previous page, 'C' to change category, or 'Q' to quit: ";
                cin >> navigationChoice;

                if (navigationChoice == 'N' || navigationChoice == 'n') {
                    page++;  // Go to next page
                }
                else if (navigationChoice == 'P' || navigationChoice == 'p') {
                    if (page > 1) page--;  // Go to previous page
                }
                else if (navigationChoice == 'C' || navigationChoice == 'c') {
                    break;  // Change category
                }
                else if (navigationChoice == 'Q' || navigationChoice == 'q') {
                    return;  // Exit function
                }
            }
            else {
                // Add item to cart
                string checkQuery = "SELECT * FROM `menu item` WHERE itemID = '" + itemID + "'";
                qstate = mysql_query(conn, checkQuery.c_str());
                if (qstate) {
                    cerr << "Error checking item ID: " << mysql_error(conn) << endl;
                    mysql_close(conn);
                    return;
                }

                res = mysql_store_result(conn);
                if (mysql_num_rows(res) == 0) {
                    cout << "Invalid Product ID! Item does not exist. Please try again." << endl;
                    continue;
                }

                cout << "Enter quantity: ";
                cin >> orderQuantity;

                string queryInsert = "INSERT INTO cart (CustomerID, ItemID, OrderQuantity) "
                    "VALUES ('" + customerID + "', '" + itemID + "', " + to_string(orderQuantity) + ")";

                if (mysql_query(conn, queryInsert.c_str())) {
                    cerr << "Error adding to cart: " << mysql_error(conn) << endl;
                    system("pause");
                }
                else {
                    cout << "Item (" << itemID << ") successfully added to your cart!" << endl;
                    system("pause");
                }
            }
        }
    }
    system("pause");
    mysql_close(conn);
    return;
}


void ViewCart() {
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;

    string customerID = loggedInCustomerID; // Assume this is already set

    // Ensure customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Database connection failed: " << mysql_error(conn) << endl;
        return;
    }

    while (true) {
        // Query to fetch cart contents
        string query = "SELECT c.ItemID, m.ItemName, c.OrderQuantity, m.ItemPrice, "
            "(c.OrderQuantity * m.ItemPrice) AS TotalPrice "
            "FROM cart c "
            "JOIN `menu item` m ON c.ItemID = m.ItemID "
            "WHERE c.CustomerID = '" + customerID + "'";
        if (mysql_query(conn, query.c_str())) {
            cerr << "Error fetching cart: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        res = mysql_store_result(conn);
        if (!res) {
            cerr << "Error storing cart result: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tYour Cart\n";
        cout << "\t\t\t\t\t\t\t\t\t--------------\n";
        cout << "\n\n\n--------------------------------------------------------------------------------------------------------------\n";
        // Display cart items
        cout << left << setw(15) << "Item ID"
            << setw(50) << "Item Name"
            << setw(15) << "Quantity"
            << setw(20) << "Price"
            << setw(20) << "Total" << endl;
        cout << "--------------------------------------------------------------------------------------------------------------\n";

        double grandTotal = 0.0;
        bool hasItems = false;

        while ((row = mysql_fetch_row(res))) {
            hasItems = true;
            cout << left << setw(15) << (row[0] ? row[0] : "")
                << setw(50) << (row[1] ? row[1] : "")
                << setw(15) << (row[2] ? row[2] : "")
                << setw(20) << (row[3] ? row[3] : "")
                << setw(20) << (row[4] ? row[4] : "") << endl;

            grandTotal += row[4] ? atof(row[4]) : 0.0;
        }

        mysql_free_result(res);

        if (!hasItems) {
            cout << "\nYour cart is empty.\n";
        }
        else {
            cout << "--------------------------------------------------------------------------------------------------------------\n";
            cout << "Grand Total: " << fixed << setprecision(2) << grandTotal << endl;
        }
        system("pause");
        return;
    }
    mysql_close(conn);
}


void RemoveFromCart() {
    MYSQL* conn;

    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    // Display current cart items before removing
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tCurrent Cart Items\n";
    ViewCart(); // Call the ViewCart function to display current cart items

    string itemID;
    cout << "Enter Product ID to remove from cart: ";
    cin >> itemID;

    string queryRemove = "DELETE FROM cart WHERE CustomerID = '" + customerID + "' AND ItemID = '" + itemID + "'";

    if (mysql_query(conn, queryRemove.c_str())) {
        cerr << "Error removing item from cart: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Item removed from cart successfully!" << endl;
    }
    system("pause");
    mysql_close(conn);
    return;
}

void UpdateCartQuantity() {
    MYSQL* conn;

    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    // Display current cart items before updating
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tCurrent Cart Items\n";
    ViewCart(); // Call the ViewCart function to display current cart items

    string itemID;
    int newQuantity;

    cout << "Enter Product ID to update quantity: ";
    cin >> itemID;

    cout << "Enter new quantity: ";
    cin >> newQuantity;

    string queryUpdate = "UPDATE cart SET OrderQuantity = " + to_string(newQuantity) +
        " WHERE CustomerID = '" + customerID + "' AND ItemID = '" + itemID + "'";

    if (mysql_query(conn, queryUpdate.c_str())) {
        cerr << "Error updating cart quantity: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Cart quantity updated successfully!" << endl;
    }
    system("pause");
    mysql_close(conn);
    return;
}

void OrderDetail() {
    while (true) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tOrder Detail\n";
        cout << "\t\t\t\t\t\t\t\t\t-----------\n";
        cout << "\n\n\n1. Order \n";
        cout << "2. Payment \n";
        cout << "0. Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            OrderManagementMenu(); // Go to Order Management Sub-menu
            break;
        case 2:
            PaymentManagementMenu(); // Go to Payment Management Sub-menu
            break;
        case 0:
            cout << "Exiting the system...\n";
            return;
            system("pause");
        default:
            cout << "Invalid choice! Please try again.\n";
            system("pause");
        }
    }
}

void OrderManagementMenu() {
    while (true) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tOrder Management\n";
        cout << "\t\t\t\t\t\t\t\t\t----------------\n";
        cout << "\n\n\n1. Place an Order\n";
        cout << "2. View Order Confirmation\n";
        cout << "3. Cancel Order\n";       
        cout << "4. Back To Order Detail\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            PlaceOrder(); // Call function to place an order
            break;
        case 2:
            ViewOrderConfirmation(); // Call function to view order confirmation
            break;
        case 3:
            CancelOrder(); // Call function to cancel an order
            break;
        case 4:
            return; // Go back to the OrderDetail menu (Main Menu)
            system("pause");
        default:
            cout << "Invalid choice! Please try again.\n";
            system("pause");
        }
    }
}

void PaymentManagementMenu() {
    while (true) {
        system("cls");
        cout << "\n\n\n\t\t\t\t\t\t\t\t\tPayment Management\n";
        cout << "\t\t\t\t\t\t\t\t\t------------------\n";
        cout << "\n\n\n1. Make a Payment\n";
        cout << "2. View Payment Confirmation\n";
        cout << "3. Back to Order Detail\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            MakePayment(); // Call function to make a payment
            break;
        case 2:
            PaymentConfirmation(); // Call function to view payment confirmation
            break;
        case 3:
            return; // Return to the main menu
            system("pause");
        default:
            cout << "Invalid choice! Please try again.\n";
            system("pause");
        }
    }
}

void PlaceOrder() {
    system("cls");

    string customerID = loggedInCustomerID;  // Customer is already logged in

    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "MySQL Connection Failed: " << mysql_error(conn) << endl;
        return;
    }

    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tPlace Order\n";
    cout << "\t\t\t\t\t\t\t\t\t--------------\n";

    // Fetch items from the cart for the customer
    string fetchCartQuery = "SELECT * FROM cart WHERE CustomerID = '" + customerID + "'";
    if (mysql_query(conn, fetchCartQuery.c_str())) {
        cerr << "Error fetching cart: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW cartRow = mysql_fetch_row(res);

    // Check if the cart is empty
    if (!cartRow) {
        cerr << "Cart is empty, error to place order." << endl;
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    // Create an empty order for the customer with "Pending" status and current date
    string createOrderQuery = "INSERT INTO orderr (CustomerID, OrderStatus, OrderTotalPrice, OrderDate) VALUES ('"
        + customerID + "', 'Pending', 0, NOW())";  // NOW() function will insert current date and time

    if (mysql_query(conn, createOrderQuery.c_str())) {
        cerr << "Error creating order: " << mysql_error(conn) << endl;
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    // Get the newly created OrderID
    string getOrderIDQuery = "SELECT OrderID FROM orderr WHERE CustomerID = '" + customerID + "' ORDER BY OrderDate DESC LIMIT 1";
    if (mysql_query(conn, getOrderIDQuery.c_str())) {
        cerr << "Error fetching OrderID: " << mysql_error(conn) << endl;
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    MYSQL_RES* orderRes = mysql_store_result(conn);
    MYSQL_ROW orderRow = mysql_fetch_row(orderRes);
    string orderID = orderRow[0];

    double totalPrice = 0;

    // Fetch items from the cart again (now we know the cart is not empty)
    string fetchCartAgainQuery = "SELECT * FROM cart WHERE CustomerID = '" + customerID + "'";
    if (mysql_query(conn, fetchCartAgainQuery.c_str())) {
        cerr << "Error fetching cart: " << mysql_error(conn) << endl;
        mysql_free_result(orderRes);
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    MYSQL_RES* cartRes = mysql_store_result(conn);
    while ((cartRow = mysql_fetch_row(cartRes))) {
        string itemID = cartRow[1];  // ItemID
        int quantity = atoi(cartRow[2]);  // OrderQuantity

        // Fetch the item price from the menu item table (corrected query)
        string fetchPriceQuery = "SELECT ItemPrice FROM `menu item` WHERE ItemID = '" + itemID + "'";
        if (mysql_query(conn, fetchPriceQuery.c_str())) {
            cerr << "Error fetching product price: " << mysql_error(conn) << endl;
            mysql_free_result(cartRes);
            mysql_free_result(orderRes);
            mysql_free_result(res);
            mysql_close(conn);
            return;
        }

        MYSQL_RES* priceRes = mysql_store_result(conn);
        MYSQL_ROW priceRow = mysql_fetch_row(priceRes);
        if (priceRow) {
            double price = atof(priceRow[0]);
            totalPrice += price * quantity;

            // Insert item into the order_item table
            string insertItemQuery = "INSERT INTO `order item` (OrderID, ItemID, OrderQuantity, OrderPrice) "
                "VALUES ('" + orderID + "', '" + itemID + "', " + to_string(quantity) + ", " + to_string(price) + ")";

            if (mysql_query(conn, insertItemQuery.c_str())) {
                cerr << "Error adding item to order: " << mysql_error(conn) << endl;
                mysql_free_result(priceRes);
                mysql_free_result(cartRes);
                mysql_free_result(orderRes);
                mysql_free_result(res);
                mysql_close(conn);
                return;
            }
        }
        else {
            cerr << "\n\n\nError: Item not found in menu for ItemID: " << itemID << endl;
            mysql_free_result(priceRes);
            mysql_free_result(cartRes);
            mysql_free_result(orderRes);
            mysql_free_result(res);
            mysql_close(conn);
            return;
        }
    }

    // Update the order total price
    string updateOrderPriceQuery = "UPDATE orderr SET OrderTotalPrice = " + to_string(totalPrice) + " WHERE OrderID = '" + orderID + "'";
    if (mysql_query(conn, updateOrderPriceQuery.c_str())) {
        cerr << "Error updating order total price: " << mysql_error(conn) << endl;
        mysql_free_result(cartRes);
        mysql_free_result(orderRes);
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    // Delete items from the cart
    string deleteCartQuery = "DELETE FROM cart WHERE CustomerID = '" + customerID + "'";
    if (mysql_query(conn, deleteCartQuery.c_str())) {
        cerr << "Error clearing the cart: " << mysql_error(conn) << endl;
        mysql_free_result(cartRes);
        mysql_free_result(orderRes);
        mysql_free_result(res);
        mysql_close(conn);
        return;
    }

    cout << "\n\n\nOrder placed successfully! Your total price is: RM " << totalPrice << endl;

    // Free resources and close the connection
    mysql_free_result(cartRes);
    mysql_free_result(orderRes);
    mysql_free_result(res);
    mysql_close(conn);
    system("pause");
}

void ViewOrderConfirmation() {
    system("cls");

    // Use loggedInCustomerID instead of asking for customer ID
    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        // Query to fetch the pending orders of the logged-in customer and include item details from menu
        string query = "SELECT o.OrderID, oi.ItemID, oi.OrderQuantity, o.OrderTotalPrice, o.OrderStatus, o.OrderDate, "
            "m.ItemName, m.ItemPrice "
            "FROM orderr o "
            "JOIN `order item` oi ON o.OrderID = oi.OrderID "
            "JOIN `menu item` m ON oi.ItemID = m.ItemID "
            "WHERE o.CustomerID = '" + customerID + "' AND o.OrderStatus = 'Pending'";

        // Execute the query
        int qstate = mysql_query(conn, query.c_str());
        if (qstate) {
            cerr << "Error fetching order confirmation: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        res = mysql_store_result(conn);

        // If there are any pending orders, display them
        if (mysql_num_rows(res) > 0) {
            system("cls");
            cout << "\n\n\n\t\t\t\t\t\t\t\t\tOrder Confirmation\n";
            cout << "\t\t\t\t\t\t\t\t\t------------------\n";
            cout << "\n\n\nPending Orders for Customer: " << customerID << endl;
            cout << "----------------------------------------------------------------------------------------------------------------------------------------\n";
            cout << std::left << std::setw(15) << "Order ID"
                << std::setw(40) << "Item Name"
                << std::setw(15) << "Item ID"
                << std::setw(15) << "Quantity"
                << std::setw(30) << "Total Price"
                << std::setw(20) << "Status"
                << std::setw(30) << "Order Date" << endl;
            cout << "----------------------------------------------------------------------------------------------------------------------------------------\n";
            cout << std::fixed << std::setprecision(2); // Set precision for floating-point numbers

            while ((row = mysql_fetch_row(res))) {
                // Display order details along with item details
                string orderID = row[0];  // OrderID
                string itemName = row[6]; // ItemName from `menu item`
                string itemID = row[1];   // ItemID
                string quantity = row[2]; // OrderQuantity
                double totalPrice = row[3] ? atof(row[3]) : 0.0; // OrderTotalPrice as double
                string status = row[4];   // OrderStatus
                string orderDate = row[5]; // OrderDate from `orderr`

                cout << std::left << std::setw(15) << orderID
                    << std::setw(40) << itemName
                    << std::setw(15) << itemID
                    << std::setw(15) << quantity
                    << std::setw(30) << totalPrice
                    << std::setw(20) << status
                    << std::setw(30) << orderDate
                    << endl;
            }
            cout << "-----------------------------------------------------------------------------------------------------------------------------------------\n";
        }
        else {
            cout << "You have no pending orders." << endl;
        }

        // Close the connection to the database
        mysql_close(conn);
    }
    else {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
    }
    system("pause");
}

void MakePayment() {
    system("cls");

    // Use loggedInCustomerID instead of asking for customer ID
    string customerID = loggedInCustomerID;  // No need to ask for customer ID again

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        return;
    }

    // Ask for the Order ID to pay
    string orderID;
    double amount;

    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tPayment\n";
    cout << "\t\t\t\t\t\t\t\t\t--------------\n";
    cout << "\n\n\nEnter Order ID to pay: ";
    cin >> orderID;

    // Query to retrieve the total price of the selected order with 'Pending' status
    string query = "SELECT OrderTotalPrice, OrderStatus FROM orderr WHERE OrderID = '" + orderID + "' AND CustomerID = '" + customerID + "' AND OrderStatus = 'Pending'";

    // Execute the query
    int qstate = mysql_query(conn, query.c_str());
    if (qstate) {
        cerr << "Error fetching order details: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        // Fetch total price of the order
        amount = atof(row[0]);

        // Set precision for displaying amounts
        cout << std::fixed << std::setprecision(2);
        cout << "\n\n\nThe total amount to pay is: " << amount << endl;

        // Ask for payment amount
        double paymentAmount;
        cout << "Enter payment amount: ";
        cin >> paymentAmount;

        if (paymentAmount < amount) {
            cout << "Insufficient payment. Please try again." << endl;
            mysql_close(conn);
            return;
        }

        // Proceed with payment 
        cout << "Processing payment..." << endl;

        // Get current date and time for PaymentDate using localtime_s
        time_t now = time(0);
        struct tm timeInfo;
        errno_t err = localtime_s(&timeInfo, &now); // Use localtime_s for safety
        if (err) {
            cerr << "Error getting local time." << endl;
            mysql_close(conn);
            return;
        }

        // Format the time as YYYY-MM-DD HH:MM:SS
        char dateBuffer[80];
        strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        string paymentDate(dateBuffer);

        // Insert payment record into the payments table
        string insertPaymentQuery = "INSERT INTO payments (OrderID, CustomerID, PaymentAmount, PaymentDate) VALUES ('" + orderID + "', '" + customerID + "', " + to_string(paymentAmount) + ", '" + paymentDate + "')";
        if (mysql_query(conn, insertPaymentQuery.c_str())) {
            cerr << "Error inserting payment: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        // Update the order status to 'Paid'
        string updateQuery = "UPDATE orderr SET OrderStatus = 'Paid' WHERE OrderID = '" + orderID + "'";
        if (mysql_query(conn, updateQuery.c_str())) {
            cerr << "Error updating order status: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        cout << "Payment successful!" << endl;
        cout << "Order has been successfully paid!" << endl;
    }
    else {
        cout << "No pending order found with the given Order ID or it has already been paid." << endl;
    }
    // Close the connection to the database
    mysql_close(conn);
    system("pause");
}

void PaymentConfirmation() {
    system("cls");

    // Use loggedInCustomerID instead of asking for customer ID
    string customerID = loggedInCustomerID;  // No need to ask for customer ID again

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        // Query to fetch the payment details of the logged-in customer
        string query = "SELECT o.OrderID, p.PaymentID, p.PaymentAmount, p.PaymentDate, o.OrderTotalPrice, o.OrderStatus "
            "FROM orderr o "
            "JOIN payments p ON o.OrderID = p.OrderID "
            "WHERE o.CustomerID = '" + customerID + "'";

        // Execute the query
        int qstate = mysql_query(conn, query.c_str());
        if (qstate) {
            cerr << "Error fetching payment confirmation: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        res = mysql_store_result(conn);

        // If there are any payment records, display them
        if (mysql_num_rows(res) > 0) {
            system("cls");
            cout << "\n\n\n\t\t\t\t\t\t\t\t\tPayment Confirmation\n";
            cout << "\t\t\t\t\t\t\t\t\t-------------------\n";
            cout << "\n\n\nPayment Confirmations for Customer: " << customerID << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------\n";
            cout << std::left << std::setw(15) << "Order ID"
                << std::setw(15) << "Payment ID"
                << std::setw(20) << "Payment Amount"
                << std::setw(30) << "Payment Date"
                << std::setw(20) << "Order Status"
                << std::setw(30) << "Order Total Price" << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------\n";
            cout << std::fixed << std::setprecision(2);

            while ((row = mysql_fetch_row(res))) {
                // Display payment details along with item details
                string orderID = row[0];  // OrderID
                string paymentID = row[1];  // PaymentID
                string paymentAmount = row[2];  // PaymentAmount
                string paymentDate = row[3];  // PaymentDate
                string orderStatus = row[5];  // OrderStatus
                string orderTotalPrice = row[4];  // OrderTotalPrice

                cout << std::left << std::setw(15) << orderID
                    << std::setw(15) << paymentID
                    << std::setw(20) << paymentAmount
                    << std::setw(30) << paymentDate
                    << std::setw(20) << orderStatus
                    << std::setw(30) << orderTotalPrice << endl;
            }
            cout << "-------------------------------------------------------------------------------------------------------------------\n";
        }
        else {
            cout << "You have no payment records." << endl;
        }

        // Close the connection to the database
        mysql_close(conn);
    }
    else {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
    }

    // Ask user if they want to go back to the home menu
    char goBack;
    cout << "\nDo you want to go back to the main menu? (y/n): ";
    cin >> goBack;
    if (goBack == 'y' || goBack == 'Y') {
        // Return to main menu 
        cout << "Returning to main menu..." << endl;
    }
    system("pause");
}

void CancelOrder() {
    system("cls");

    // Use loggedInCustomerID instead of asking for customer ID
    string customerID = loggedInCustomerID;  // No need to ask for customer ID again

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    // Connect to the database
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    if (conn) {
        // Query to fetch all the orders for the logged-in customer
        string query = "SELECT o.OrderID, o.OrderStatus "
            "FROM orderr o "
            "WHERE o.CustomerID = '" + customerID + "'";

        // Execute the query to check for orders
        int qstate = mysql_query(conn, query.c_str());
        if (qstate) {
            cerr << "Error fetching orders: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        res = mysql_store_result(conn);

        // Check if there are any orders
        if (mysql_num_rows(res) > 0) {
            system("cls");
            cout << "\n\n\n\t\t\t\t\t\t\t\t\tCancel Order\n";
            cout << "\t\t\t\t\t\t\t\t\t--------------\n";
            cout << "\n\n\nOrders for Customer: " << customerID << endl;
            cout << "--------------------------------------------------------------\n";
            cout << std::left << std::setw(10) << "Order ID"
                << std::setw(15) << "Order Status" << endl;
            cout << "--------------------------------------------------------------\n";

            // Display all orders with their status
            while ((row = mysql_fetch_row(res))) {
                string orderID = row[0];    // OrderID
                string orderStatus = row[1]; // OrderStatus

                cout << std::left << std::setw(10) << orderID
                    << std::setw(15) << orderStatus
                    << endl;
            }

            // Ask the customer to input the order ID they want to cancel
            string orderToCancel;
            cout << "\nEnter the OrderID to cancel: ";
            cin >> orderToCancel;

            // Check if the order status is "Pending" (only pending orders can be canceled)
            string checkQuery = "SELECT OrderStatus FROM orderr WHERE OrderID = '" + orderToCancel + "' AND CustomerID = '" + customerID + "'";

            int checkState = mysql_query(conn, checkQuery.c_str());
            if (checkState) {
                cerr << "Error checking order status: " << mysql_error(conn) << endl;
                mysql_close(conn);
                return;
            }

            MYSQL_RES* checkRes = mysql_store_result(conn);
            if (mysql_num_rows(checkRes) == 0) {
                cout << "Invalid OrderID or this order doesn't belong to you." << endl;
                mysql_free_result(checkRes);
                mysql_close(conn);
                return;
            }

            MYSQL_ROW checkRow = mysql_fetch_row(checkRes);
            string orderStatus = checkRow[0];

            if (orderStatus != "Pending") {
                cout << "Only pending orders can be cancelled. This order is not in 'Pending' status." << endl;
                mysql_free_result(checkRes);
                mysql_close(conn);
                return;
            }

            // Proceed to cancel the order by updating its status to 'Canceled'
            string updateQuery = "UPDATE orderr SET OrderStatus = 'Canceled' WHERE OrderID = '" + orderToCancel + "' AND CustomerID = '" + customerID + "'";

            int updateState = mysql_query(conn, updateQuery.c_str());
            if (updateState) {
                cerr << "Error canceling order: " << mysql_error(conn) << endl;
            }
            else {
                cout << "Order ID " << orderToCancel << " has been cancelled successfully!" << endl;
            }

            mysql_free_result(checkRes);
        }
        else {
            cout << "You have no orders." << endl;
        }

        // Close the connection to the database
        mysql_close(conn);
    }
    else {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
    }

    // Ask if the user wants to go back to the home menu
    char goBack;
    cout << "\nDo you want to go back to the main menu? (y/n): ";
    cin >> goBack;
    if (goBack == 'y' || goBack == 'Y') {
        // Return to main menu 
        cout << "Returning to main menu..." << endl;
    }
    system("pause");
}

void Feedback() {
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    string customerID = loggedInCustomerID;  // Assume loggedInCustomerID is already set

    // Check if customer is logged in
    if (customerID.empty()) {
        cerr << "No customer is logged in!" << endl;
        return;
    }

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "ordering_system", 3306, NULL, 0);

    // Check database connection
    if (!conn) {
        cerr << "Database connection failed: " << mysql_error(conn) << endl;
        return;
    }

    system("cls");
    cout << "\n\n\n\t\t\t\t\t\t\t\t\tFeedback\n";
    cout << "\t\t\t\t\t\t\t\t\t----------\n";

    // Fetch the most recent paid Order ID that does not already have feedback
    string orderID;
    string getOrderQuery =
        "SELECT o.OrderID FROM orderr o "
        "LEFT JOIN feedback f ON o.OrderID = f.OrderID "
        "WHERE o.CustomerID = '" + customerID + "' AND o.OrderStatus = 'Paid' AND f.OrderID IS NULL "
        "ORDER BY o.OrderDate DESC LIMIT 1";

    if (mysql_query(conn, getOrderQuery.c_str())) {
        cerr << "Error fetching order: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row) {
        cout << "\n\n\nNo paid orders found without feedback. Feedback cannot be provided.\n";
        mysql_free_result(res);
        mysql_close(conn);
        system("pause");
        return;
    }

    orderID = row[0];  // Use the fetched Order ID
    mysql_free_result(res);

    cout << "\n\n\nYour feedback is being provided for Order ID: " << orderID << endl;

    // Input Rating
    int rating;
    cout << "\nPlease provide a rating (1-5): ";
    cin >> rating;

    while (rating < 1 || rating > 5) {
        cout << "Invalid rating. Please enter a value between 1 and 5: ";
        cin >> rating;
    }

    cin.ignore(); // Clear the input buffer for comments
    string comments;
    cout << "\nWrite your feedback (press ENTER when done):\n";
    getline(cin, comments);

    // Insert feedback into the database
    string insertFeedbackQuery = "INSERT INTO feedback (CustomerID, OrderID, Rating, Comments) VALUES ('" +
        customerID + "', '" + orderID + "', " + to_string(rating) + ", '" + comments + "')";
    if (mysql_query(conn, insertFeedbackQuery.c_str())) {
        cerr << "\nError submitting feedback: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return;
    }

    cout << "\nThank you for your feedback! It has been successfully submitted.\n";
    mysql_close(conn);
    system("pause");
}