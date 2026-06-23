#include <iostream>
#include <string>
#include <iomanip>
#include <mysql.h> // Make sure you have the MySQL library installed
#include <conio.h>
#ifndef ORDERING_SYSTEM_H 
#define ORDERING_SYSTEM_H

//welcome page
void welcome_Page();

//main menu
void Home();
void login();
extern int attemptCount;       // Declaration of attemptCount
extern const int maxAttempts;  // Declaration of maxAttempts
void registerAsCustomer();
void resetPassword();

// Declare the password validation function
bool isValidPassword(const std::string& password);
bool isValidPhoneNumber(const std::string& phoneNumber);
bool isValidEmail(const std::string& email);
std::string hashPassword(const std::string& password);
std::string loggedInCustomerID;  // Global variable to store logged-in customer's ID

// staff menu
void ShowStaffMenu();

// staff flow
void EditStaffProfile();
void SearchBar();
void ViewProducts();
void ProductMenu(); 
void AddProduct();
void EditProduct();
void UpdateMenu();
void DeleteMenu();
void OrderStatus();
void GenerateReport();
void GenerateFeedbackReport();


// customer menu
void ShowCustomerMenu();

// customer flow
void EditCustomerProfile();
void ManageCart();
void AddToCart();
void ViewCart();
void RemoveFromCart();
void UpdateCartQuantity();
void OrderDetail();
void OrderManagementMenu();
void PlaceOrder();
void ViewOrderConfirmation();
void PaymentManagementMenu();
void MakePayment();
void PaymentConfirmation();
void CancelOrder();
void Feedback();

#endif
