#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <cctype>
#include <queue>
#include <map>

using namespace std;

// ===================== DATA STRUCTURES =====================
struct BankAccount {
    string name;
    double balance;
};

struct Passenger {
    string name, passport, id, contact;
    string seatNumber, destination;
    time_t registrationDate;
    Passenger* next;
    Passenger* prev;
};

struct Flight {
    string flightNo, destination, dayTime, distance, plane, duration;
    int totalSeats;
    float price;
    Passenger* passengerHead;
    map<string, bool> seatMap;
};

struct Booking {
    string bookingId, flightNo, passengerId, seatNumber;
    time_t bookingTime;
    bool isPaid;
};

struct Aircraft {
    string model;
    int totalSeats;
    vector<string> features;
};



// ===================== FUNCTION DECLARATIONS =====================
// Utility Functions
time_t getCurrentTime();
string timeToString(time_t t);
bool validatePhone(const string& phone);
bool validatePassport(const string& passport);
bool validateID(const string& id);
bool validateInput(const string& input);
bool isPassengerInBankSystem(const string& name);
double getPassengerBalance(const string& name);
bool processPayment(const string& name, double amount);

// File Handling
void saveData();
void loadData();

// Seat Management
void initializeSeats(Flight& flight);
void displaySeatMap(const Flight& flight);
bool bookSeat(Flight& flight, Passenger* passenger, const string& seatNumber);

// Passenger List Management
void addPassengerToFlight(Flight& flight, Passenger* passenger);
void removePassengerFromFlight(Flight& flight, const string& passengerId);

// Flight Functions
void viewAllFlights();
void searchByDestination();

// Booking Functions
void bookFlight();
void cancelBooking();
void viewCurrentBooking();
void postponeBooking();

// Admin Functions
bool authenticateAdmin();
void addAircraft();
void addFlight();
void displayCurrentState();
void deleteAircraft();
void deleteFlight();
void displayPassengerWithDestination();
void displayBookingsWithEmptyCheck();
void adminCancelBooking();

// Bank Functions
void displayBankStatement();

// Menu Functions
void adminMenu();
void passengerMenu();

// ===================== UTILITY FUNCTIONS =====================
time_t getCurrentTime() { return time(nullptr); }

string timeToString(time_t t) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buffer);
}

bool validatePhone(const string& phone) {
    return phone.length() <= 15 && all_of(phone.begin(), phone.end(), ::isdigit);
}

bool validatePassport(const string& passport) {
    return passport.length() <= 10 && all_of(passport.begin(), passport.end(), ::isalnum);
}

bool validateID(const string& id) {
    return id.length() <= 10 && all_of(id.begin(), id.end(), ::isdigit);
}

bool validateInput(const string& input) {
    return !input.empty() && input.length() <= 20;
}

bool isPassengerInBankSystem(const string& name) {
    return any_of(bankRecords.begin(), bankRecords.end(),
        [&name](const BankAccount& acc) { return acc.name == name; });
}

double getPassengerBalance(const string& name) {
    auto it = find_if(bankRecords.begin(), bankRecords.end(),
        [&name](const BankAccount& acc) { return acc.name == name; });
    return (it != bankRecords.end()) ? it->balance : 0.0;
}

bool processPayment(const string& name, double amount) {
    auto it = find_if(bankRecords.begin(), bankRecords.end(),
        [&name](const BankAccount& acc) { return acc.name == name; });
    
    if (it != bankRecords.end() && it->balance >= amount) {
        it->balance -= amount;
        return true;
    }
    return false;
}

// ===================== FILE HANDLING =====================
void saveData() {
    ofstream ffile("flights.txt");
    for (const auto& f : flights) {
        ffile << f.flightNo << "," << f.destination << "," << f.dayTime << ","
              << f.distance << "," << f.plane << "," << f.duration << ","
              << f.totalSeats << "," << f.price << "\n";
    }
    ffile.close();

    ofstream pfile("passengers.txt");
    for (const auto& p : passengers) {
        pfile << p.name << "," << p.passport << "," << p.id << "," 
              << p.contact << "," << p.destination << "," << p.registrationDate << "\n";
    }
    pfile.close();

    ofstream bfile("bookings.txt");
    for (const auto& b : bookings) {
        bfile << b.bookingId << "," << b.flightNo << "," << b.passengerId << ","
              << b.seatNumber << "," << b.bookingTime << "," << b.isPaid << "\n";
    }
    bfile.close();
}

void loadData() {
    ifstream ffile("flights.txt");
    if (ffile) {
        flights.clear();
        string line;
        while (getline(ffile, line)) {
            stringstream ss(line);
            Flight f;
            getline(ss, f.flightNo, ',');
            getline(ss, f.destination, ',');
            getline(ss, f.dayTime, ',');
            getline(ss, f.distance, ',');
            getline(ss, f.plane, ',');
            getline(ss, f.duration, ',');
            ss >> f.totalSeats;
            ss.ignore();
            ss >> f.price;
            f.passengerHead = nullptr;
            flights.push_back(f);
        }
        ffile.close();
    }

    ifstream pfile("passengers.txt");
    if (pfile) {
        passengers.clear();
        string line;
        while (getline(pfile, line)) {
            stringstream ss(line);
            Passenger p;
            getline(ss, p.name, ',');
            getline(ss, p.passport, ',');
            getline(ss, p.id, ',');
            getline(ss, p.contact, ',');
            getline(ss, p.destination, ',');
            ss >> p.registrationDate;
            passengers.push_back(p);
        }
        pfile.close();
    }

    ifstream bfile("bookings.txt");
    if (bfile) {
        bookings.clear();
        string line;
        while (getline(bfile, line)) {
            stringstream ss(line);
            Booking b;
            getline(ss, b.bookingId, ',');
            getline(ss, b.flightNo, ',');
            getline(ss, b.passengerId, ',');
            getline(ss, b.seatNumber, ',');
            ss >> b.bookingTime;
            ss.ignore();
            ss >> b.isPaid;
            bookings.push_back(b);
        }
        bfile.close();
    }
}





here is your code !!!















// ===================== MAIN FUNCTION =====================
int main() {
    if (flights.empty()) {
        flights = {
            {"AF101", "Cairo", "Mon 08:00 AM", "1200 km", "Boeing 737", "2h", 100, 2500.00f, nullptr, {}},
            {"AF202", "Nairobi", "Tue 10:30 AM", "1800 km", "Airbus A320", "3h", 100, 3000.00f, nullptr, {}},
        };
    }

    for (auto& flight : flights) {
        initializeSeats(flight);
    }
    loadData();

    int choice;
    do {
        cout << "\n===== AIRPLANE MANAGEMENT SYSTEM =====";
        cout << "\n1. Admin Login";
        cout << "\n2. Passenger Menu";
        cout << "\n3. Bank Statement";
        cout << "\n4. Exit";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: adminMenu(); break;
            case 2: passengerMenu(); break;
            case 3: displayBankStatement(); break;
            case 4: cout << "Exiting system...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 4);

    // Cleanup
    for (auto& flight : flights) {
        Passenger* current = flight.passengerHead;
        while (current) {
            Passenger* temp = current;
            current = current->next;
            delete temp;
        }
    }
    saveData();
    return 0;
}












