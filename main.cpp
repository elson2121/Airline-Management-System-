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


// ===================== GLOBAL VARIABLES =====================
vector<Flight> flights;
vector<Passenger> passengers;
vector<Booking> bookings;
vector<Aircraft> aircrafts;
vector<BankAccount> bankRecords = {
    {"Abebe Bikila", 8500.00},
    {"Abel Tesfaye", 12000.00},
    {"Haile Gebre", 15000.00},
    {"Tirunesh Dibaba", 9000.00},
    {"Abe Kebe", 18000.00},
    {"Meseret Yimer", 7500.00},
    {"Hanan Daye", 6000.00},
    {"Abiy Yosi", 5000.00}
};
queue<string> userQueue;



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

// ===================== SEAT MANAGEMENT =====================
void initializeSeats(Flight& flight) {
    flight.seatMap.clear();
    for (int row = 1; row <= 10; row++) {
        for (char col = 'A'; col <= 'J'; col++) {
            string seat = string(1, col) + to_string(row);
            flight.seatMap[seat] = false;
        }
    }
}

void displaySeatMap(const Flight& flight) {
    cout << "\n===== SEAT MAP FOR FLIGHT " << flight.flightNo << " =====";
    cout << "\n\n  ";
    for (char col = 'A'; col <= 'J'; col++) {
        cout << setw(4) << col;
    }
    cout << "\n";

    for (int row = 1; row <= 10; row++) {
        cout << setw(2) << row;
        for (char col = 'A'; col <= 'J'; col++) {
            string seat = string(1, col) + to_string(row);
            cout << setw(4) << (flight.seatMap.at(seat) ? "[X]" : "[ ]");
        }
        cout << "\n";
    }
    cout << "\n[X] = Booked\t[ ] = Available\n";
}

bool bookSeat(Flight& flight, Passenger* passenger, const string& seatNumber) {
    string upperSeat = seatNumber;
    transform(upperSeat.begin(), upperSeat.end(), upperSeat.begin(), ::toupper);

    if (upperSeat.length() < 2 || !isalpha(upperSeat[0]) || !isdigit(upperSeat[1])) {
        cout << "Invalid seat format! Use format like A1, B2, etc.\n";
        return false;
    }

    if (flight.seatMap.find(upperSeat) == flight.seatMap.end()) {
        cout << "Seat doesn't exist on this aircraft!\n";
        return false;
    }

    if (flight.seatMap[upperSeat]) {
        cout << "Seat already booked! Please choose another seat.\n";
        return false;
    }

    flight.seatMap[upperSeat] = true;
    passenger->seatNumber = upperSeat;
    return true;
}

// ===================== PASSENGER LIST MANAGEMENT =====================
void addPassengerToFlight(Flight& flight, Passenger* passenger) {
    passenger->next = nullptr;
    passenger->prev = nullptr;
    
    if (!flight.passengerHead) {
        flight.passengerHead = passenger;
    } else {
        Passenger* current = flight.passengerHead;
        while (current->next) {
            current = current->next;
        }
        current->next = passenger;
        passenger->prev = current;
    }
    flight.totalSeats--;
}

void removePassengerFromFlight(Flight& flight, const string& passengerId) {
    Passenger* current = flight.passengerHead;
    while (current) {
        if (current->id == passengerId) {
            if (current->prev) current->prev->next = current->next;
            if (current->next) current->next->prev = current->prev;
            if (current == flight.passengerHead) flight.passengerHead = current->next;
            
            flight.seatMap[current->seatNumber] = false;
            flight.totalSeats++;
            delete current;
            return;
        }
        current = current->next;
    }
}

// ===================== FLIGHT FUNCTIONS =====================
void viewAllFlights() {
    loadData();
    cout << "\n===== AVAILABLE FLIGHTS =====";
    cout << left << setw(8) << "\nCode" << setw(15) << "Destination" 
         << setw(14) << "Departure" << setw(10) << "Distance"
         << setw(10) << "Duration" << setw(12) << "Aircraft" 
         << setw(6) << "Seats" << "Price\n";
    
    for (const auto& f : flights) {
        cout << setw(8) << f.flightNo << setw(15) << f.destination 
             << setw(14) << f.dayTime << setw(10) << f.distance
             << setw(10) << f.duration << setw(12) << f.plane 
             << setw(6) << f.totalSeats << "$" << f.price << "\n";
    }
}

void searchByDestination() {
    string dest;
    cout << "Enter destination: ";
    cin.ignore();
    getline(cin, dest);
    transform(dest.begin(), dest.end(), dest.begin(), ::tolower);

    cout << "\n===== SEARCH RESULTS =====\n";
    bool found = false;
    for (const auto& f : flights) {
        string fdest = f.destination;
        transform(fdest.begin(), fdest.end(), fdest.begin(), ::tolower);
        if (fdest.find(dest) != string::npos) {
            cout << "Flight: " << f.flightNo << " | " << f.destination 
                 << " | " << f.dayTime << " | " << f.duration 
                 << " | Seats: " << f.totalSeats << " | Price: $" << f.price << "\n";
            found = true;
        }
    }
    if (!found) cout << "No flights found!\n";
}

// ===================== BOOKING FUNCTIONS =====================
void bookFlight() {
    viewAllFlights();
    string flightNo;
    cout << "\nEnter flight number: ";
    cin >> flightNo;
    
    auto flightIt = find_if(flights.begin(), flights.end(), 
        [&flightNo](const Flight& f) { return f.flightNo == flightNo; });
    
    if (flightIt == flights.end()) {
        cout << "Flight not found!\n";
        return;
    }

    if (flightIt->totalSeats <= 0) {
        cout << "No seats available!\n";
        return;
    }

    displaySeatMap(*flightIt);

    Passenger* p = new Passenger();
    do {
        cout << "Enter your name (max 20 chars): ";
        cin.ignore();
        getline(cin, p->name);
    } while (!validateInput(p->name));

    do {
        cout << "Enter passport (max 10 chars): ";
        cin >> p->passport;
    } while (!validatePassport(p->passport));

    do {
        cout << "Enter ID (max 10 digits): ";
        cin >> p->id;
    } while (!validateID(p->id));

    do {
        cout << "Enter phone (max 15 digits): ";
        cin >> p->contact;
    } while (!validatePhone(p->contact));

    p->destination = flightIt->destination;
    p->registrationDate = getCurrentTime();

    string seat;
    bool seatBooked = false;
    while (!seatBooked) {
        cout << "Choose your seat (e.g., A1, B3): ";
        cin >> seat;
        seatBooked = bookSeat(*flightIt, p, seat);
    }

    // Enhanced Payment Processing
    if (isPassengerInBankSystem(p->name)) {
        double currentBalance = getPassengerBalance(p->name);
        cout << "\nFlight cost: " << flightIt->price << " ETB";
        cout << "\nYour current balance: " << currentBalance << " ETB";
        
        if (currentBalance >= flightIt->price) {
            if (processPayment(p->name, flightIt->price)) {
                cout << "\nPayment processed successfully!";
                cout << "\nNew balance: " << getPassengerBalance(p->name) << " ETB";
                
                Booking b;
                b.bookingId = "B" + to_string(bookings.size() + 1000);
                b.flightNo = flightNo;
                b.passengerId = p->id;
                b.seatNumber = seat;
                b.bookingTime = getCurrentTime();
                b.isPaid = true;

                addPassengerToFlight(*flightIt, p);
                passengers.push_back(*p);
                bookings.push_back(b);
                userQueue.push(p->id);

                cout << "\nBooking successful! Your Booking ID: " << b.bookingId << "\n";
            } else {
                cout << "\nPayment processing failed!\n";
                delete p;
                return;
            }
        } else {
            cout << "\nInsufficient funds!\n";
            delete p;
            return;
        }
    } else {
        cout << "\nTotal to pay: " << flightIt->price << " ETB";
        cout << "\nConfirm payment? (1=Yes, 0=No): ";
        int confirm;
        cin >> confirm;

        if (confirm == 1) {
            Booking b;
            b.bookingId = "B" + to_string(bookings.size() + 1000);
            b.flightNo = flightNo;
            b.passengerId = p->id;
            b.seatNumber = seat;
            b.bookingTime = getCurrentTime();
            b.isPaid = true;

            addPassengerToFlight(*flightIt, p);
            passengers.push_back(*p);
            bookings.push_back(b);
            userQueue.push(p->id);

            cout << "\nBooking successful! Your Booking ID: " << b.bookingId << "\n";
        } else {
            delete p;
            cout << "Booking cancelled.\n";
        }
    }
    saveData();
}

void cancelBooking() {
    string bookingId;
    cout << "Enter booking ID: ";
    cin >> bookingId;

    auto booking = find_if(bookings.begin(), bookings.end(), 
        [&bookingId](const Booking& b) { return b.bookingId == bookingId; });

    if (booking == bookings.end()) {
        cout << "Booking not found!\n";
        return;
    }

    auto flight = find_if(flights.begin(), flights.end(), 
        [&booking](const Flight& f) { return f.flightNo == booking->flightNo; });

    if (flight != flights.end()) {
        removePassengerFromFlight(*flight, booking->passengerId);
    }

    bookings.erase(booking);
    saveData();
    cout << "Booking cancelled successfully!\n";
}

void viewCurrentBooking() {
    string passengerId;
    cout << "Enter your ID: ";
    cin >> passengerId;

    bool found = false;
    for (const auto& b : bookings) {
        if (b.passengerId == passengerId) {
            found = true;
            cout << "\n===== YOUR BOOKING =====";
            cout << "\nBooking ID: " << b.bookingId;
            cout << "\nFlight: " << b.flightNo;
            cout << "\nSeat: " << b.seatNumber;
            cout << "\nBooking Time: " << timeToString(b.bookingTime);
            cout << "\nStatus: " << (b.isPaid ? "Paid" : "Unpaid") << "\n";
            
            auto passenger = find_if(passengers.begin(), passengers.end(),
                [&passengerId](const Passenger& p) { return p.id == passengerId; });
            
            if (passenger != passengers.end()) {
                cout << "\nPassenger Details:";
                cout << "\nName: " << passenger->name;
                cout << "\nPassport: " << passenger->passport;
                cout << "\nContact: " << passenger->contact << "\n";
            }
            break;
        }
    }

    if (!found) {
        cout << "No booking found for this ID!\n";
    }
}

void postponeBooking() {
    string bookingId, password;
    cout << "Enter your booking ID: ";
    cin >> bookingId;
    cout << "Enter flight password to verify: ";
    cin >> password;

    auto booking = find_if(bookings.begin(), bookings.end(),
        [&bookingId](const Booking& b) { return b.bookingId == bookingId; });

    if (booking == bookings.end()) {
        cout << "Booking not found!\n";
        return;
    }

    if (password != "flight123") {
        cout << "Invalid password!\n";
        return;
    }

    auto flight = find_if(flights.begin(), flights.end(),
        [&booking](const Flight& f) { return f.flightNo == booking->flightNo; });

    if (flight != flights.end()) {
        flight->seatMap[booking->seatNumber] = false;
        flight->totalSeats++;

        Passenger p;
        cout << "\nEnter new booking details:\n";
        cout << "Name: ";
        cin.ignore();
        getline(cin, p.name);
        
        cout << "Passport: ";
        cin >> p.passport;
        
        cout << "ID: ";
        cin >> p.id;
        
        cout << "Contact: ";
        cin >> p.contact;
        
        displaySeatMap(*flight);
        
        string newSeat;
        bool seatBooked = false;
        while (!seatBooked) {
            cout << "Choose your new seat: ";
            cin >> newSeat;
            seatBooked = bookSeat(*flight, &p, newSeat);
        }

        booking->passengerId = p.id;
        booking->seatNumber = newSeat;
        booking->bookingTime = getCurrentTime();

        cout << "Booking postponed successfully!\n";
        saveData();
    } else {
        cout << "Flight not found!\n";
    }
}


// ===================== ADMIN FUNCTIONS =====================
bool authenticateAdmin() {
    string password;
    cout << "Enter admin password: ";
    cin >> password;
    return password == "ela2121";
}

void addAircraft() {
    Aircraft a;
    cout << "Enter plane model: ";
    cin.ignore();
    getline(cin, a.model);
    
    cout << "Enter total seats: ";
    cin >> a.totalSeats;
    
    cout << "Enter features (comma separated): ";
    string features;
    cin.ignore();
    getline(cin, features);
    
    stringstream ss(features);
    string feature;
    while (getline(ss, feature, ',')) {
        a.features.push_back(feature);
    }
    
    aircrafts.push_back(a);
    cout << "Aircraft added successfully!\n";
    saveData();
}

void addFlight() {
    if (aircrafts.empty()) {
        cout << "No aircrafts available! Add one first.\n";
        return;
    }

    cout << "\nAvailable Aircrafts:\n";
    for (const auto& a : aircrafts) {
        cout << "- " << a.model << " (" << a.totalSeats << " seats)\n";
    }

    Flight f;
    cout << "\nEnter flight number: ";
    cin >> f.flightNo;
    
    cout << "Select plane model: ";
    string selectedModel;
    cin.ignore();
    getline(cin, selectedModel);
    
    auto plane = find_if(aircrafts.begin(), aircrafts.end(),
        [&selectedModel](const Aircraft& a) { return a.model == selectedModel; });
    
    if (plane == aircrafts.end()) {
        cout << "Invalid plane model!\n";
        return;
    }

    f.plane = plane->model;
    f.totalSeats = plane->totalSeats;
    f.passengerHead = nullptr;
    initializeSeats(f);
    
    cout << "Enter destination: ";
    getline(cin, f.destination);
    
    cout << "Enter day/time: ";
    getline(cin, f.dayTime);
    
    cout << "Enter distance: ";
    getline(cin, f.distance);
    
    cout << "Enter duration: ";
    getline(cin, f.duration);
    
    cout << "Enter price: $";
    cin >> f.price;

    flights.push_back(f);
    saveData();
    cout << "Flight added successfully using " << plane->model << "!\n";
}

void displayCurrentState() {
    cout << "\n===== CURRENT SYSTEM STATE =====";
    cout << "\n\nAIRCRAFTS:\n";
    if (aircrafts.empty()) {
        cout << "No aircrafts available.\n";
    } else {
        for (const auto& a : aircrafts) {
            cout << "- " << a.model << " (" << a.totalSeats << " seats)\n";
        }
    }

    cout << "\nFLIGHTS:\n";
    if (flights.empty()) {
        cout << "No flights scheduled.\n";
    } else {
        for (const auto& f : flights) {
            cout << f.flightNo << " to " << f.destination 
                 << " (" << f.plane << ") - " << f.totalSeats << " seats available\n";
        }
    }
}

void deleteAircraft() {
    displayCurrentState();
    if (aircrafts.empty()) {
        cout << "No aircrafts to delete.\n";
        return;
    }

    string model;
    cout << "\nEnter aircraft model to delete: ";
    cin.ignore();
    getline(cin, model);

    auto it = find_if(aircrafts.begin(), aircrafts.end(),
        [&model](const Aircraft& a) { return a.model == model; });

    if (it != aircrafts.end()) {
        bool inUse = any_of(flights.begin(), flights.end(),
            [&model](const Flight& f) { return f.plane == model; });

        if (inUse) {
            cout << "Cannot delete! Aircraft is in use by flights.\n";
        } else {
            aircrafts.erase(it);
            cout << "Aircraft deleted successfully!\n";
            saveData();
        }
    } else {
        cout << "Aircraft not found!\n";
    }
}

void deleteFlight() {
    displayCurrentState();
    if (flights.empty()) {
        cout << "No flights to delete.\n";
        return;
    }

    string flightNo;
    cout << "\nEnter flight number to delete: ";
    cin >> flightNo;

    auto it = find_if(flights.begin(), flights.end(), 
        [&flightNo](const Flight& f) { return f.flightNo == flightNo; });

    if (it != flights.end()) {
        // Free passenger memory
        Passenger* current = it->passengerHead;
        while (current) {
            Passenger* temp = current;
            current = current->next;
            delete temp;
        }
        
        flights.erase(it);
        cout << "Flight deleted successfully!\n";
        saveData();
    } else {
        cout << "Flight not found!\n";
    }
}

void displayPassengerWithDestination() {
    if (passengers.empty()) {
        cout << "\nNo passengers registered yet.\n";
        return;
    }

    cout << "\n===== ALL PASSENGERS =====";
    cout << left << setw(20) << "\nName" << setw(15) << "Destination" 
         << setw(12) << "Passport" << setw(12) << "ID" 
         << "Registration Date\n";
    
    for (const auto& p : passengers) {
        cout << setw(20) << p.name << setw(15) << p.destination
             << setw(12) << p.passport << setw(12) << p.id
             << timeToString(p.registrationDate) << "\n";
    }
}

void displayBookingsWithEmptyCheck() {
    if (bookings.empty()) {
        cout << "\nNo bookings found in the system.\n";
        return;
    }

    cout << "\n===== ALL BOOKINGS =====";
    cout << left << setw(10) << "\nBooking ID" << setw(10) << "Flight" 
         << setw(12) << "Passenger ID" << setw(10) << "Seat" 
         << setw(20) << "Booking Time" << "Status\n";
    
    for (const auto& b : bookings) {
        cout << setw(10) << b.bookingId << setw(10) << b.flightNo 
             << setw(12) << b.passengerId << setw(10) << b.seatNumber
             << setw(20) << timeToString(b.bookingTime) 
             << (b.isPaid ? "Paid" : "Unpaid") << "\n";
    }
}

void adminCancelBooking() {
    displayBookingsWithEmptyCheck();
    if (bookings.empty()) return;

    string bookingId;
    cout << "Enter booking ID to cancel: ";
    cin >> bookingId;

    auto booking = find_if(bookings.begin(), bookings.end(), 
        [&bookingId](const Booking& b) { return b.bookingId == bookingId; });

    if (booking == bookings.end()) {
        cout << "Booking not found!\n";
        return;
    }

    auto flight = find_if(flights.begin(), flights.end(), 
        [&booking](const Flight& f) { return f.flightNo == booking->flightNo; });

    if (flight != flights.end()) {
        removePassengerFromFlight(*flight, booking->passengerId);
    }

    bookings.erase(booking);
    saveData();
    cout << "Admin: Booking cancelled successfully!\n";

}
// ===================== BANK FUNCTIONS =====================
void displayBankStatement() {
    cout << "\n===== BANK STATEMENT =====";
    cout << "\n" << left << setw(25) << "Name" << "Balance (ETB)\n";
    cout << "--------------------------------\n";
    for (const auto& account : bankRecords) {
        cout << setw(25) << account.name << account.balance << "\n";
    }
}



// ===================== MENU FUNCTIONS =====================
void adminMenu() {
    if (!authenticateAdmin()) {
        cout << "Access denied!\n";
        return;
    }

    int choice;
    do {
        cout << "\n===== ADMIN MENU =====";
        cout << "\n1. Add Aircraft";
        cout << "\n2. Add Flight";
        cout << "\n3. Delete Aircraft";
        cout << "\n4. Delete Flight";
        cout << "\n5. View Current State";
        cout << "\n6. View All Passengers";
        cout << "\n7. View All Bookings";
        cout << "\n8. Cancel Any Booking";
        cout << "\n9. Return to Main Menu";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addAircraft(); break;
            case 2: addFlight(); break;
            case 3: deleteAircraft(); break;
            case 4: deleteFlight(); break;
            case 5: displayCurrentState(); break;
            case 6: displayPassengerWithDestination(); break;
            case 7: displayBookingsWithEmptyCheck(); break;
            case 8: adminCancelBooking(); break;
            case 9: break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 9);
}

void passengerMenu() {
    loadData();
    int choice;
    do {
        cout << "\n===== PASSENGER MENU =====";
        cout << "\n1. View All Flights";
        cout << "\n2. Search by Destination";
        cout << "\n3. Book a Flight";
        cout << "\n4. View Current Booking";
        cout << "\n5. Postpone Booking";
        cout << "\n6. Cancel Booking";
        cout << "\n7. Back to Main Menu";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: viewAllFlights(); break;
            case 2: searchByDestination(); break;
            case 3: bookFlight(); break;
            case 4: viewCurrentBooking(); break;
            case 5: postponeBooking(); break;
            case 6: cancelBooking(); break;
            case 7: break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 7);
}



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












