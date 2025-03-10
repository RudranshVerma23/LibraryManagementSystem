/**************************************************
 * main.cpp
 **************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <unordered_map>

long long currentDaysSinceEpoch() {
    using namespace std::chrono;
    system_clock::time_point tp = system_clock::now();
    auto dur = tp.time_since_epoch();
    auto days = duration_cast<std::chrono::hours>(dur).count() / 24;
    return static_cast<long long>(days);
}

long long daysDifferenceFromNow(long long dayStamp) {
    return currentDaysSinceEpoch() - dayStamp;
}

// --------------------------------------------------
// enum for simpler and clearer approach for assigning status
// --------------------------------------------------
enum class BookStatus {
    AVAILABLE,
    BORROWED
};

std::string bookStatusToString(BookStatus st) {
    if (st == BookStatus::BORROWED)  return "Borrowed";
    return "Available"; // default
}

BookStatus stringToBookStatus(const std::string &s) {
    return (s == "Borrowed") ? BookStatus::BORROWED : BookStatus::AVAILABLE;
}

class Account; // defined later

// User class, which is inherited by Student, Faculty and Librarian child classes
class User {
protected:
    std::string userID;
    std::string password;
    std::string name;
    std::string role;  // "Student", "Faculty" or "Librarian"
    double fine;
public:
    Account* account;

    User(const std::string& u="", const std::string& p="",
         const std::string& n="", const std::string& r="", double f=0.0)
        : userID(u), password(p), name(n), role(r), fine(f), account(nullptr) {}

    virtual ~User() {}

    std::string getUserID()   const { return userID; }
    std::string getPassword() const { return password; }
    std::string getName()     const { return name; }
    std::string getRole()     const { return role; }
    double      getFine()     const { return fine; }

    void setFine(double f) { fine = f; }

    virtual int  getMaxBooksAllowed() const = 0;
    virtual int  getMaxBorrowDays()   const = 0;
    virtual bool hasFines()           const { return (fine > 0); }

    virtual void borrowBook(const std::string& ISBN) = 0;
    virtual void returnBook(const std::string& ISBN) = 0;

    void payFine() {
        std::cout << "Your outstanding fine is: " << fine << "\n";
        std::cout << "Enter amount to pay: ";
        double amt;
        std::cin >> amt;
        if(amt >= fine) {
            std::cout << "Fine cleared!\n";
            fine = 0.0;
        } else {
            fine -= amt;
            std::cout << "Partial payment done. Remaining fine: " << fine << "\n";
        }
    }
};

struct BorrowInfo {
    std::string ISBN;
    long long borrowDay;
};

class Account {
private:
    std::vector<BorrowInfo> currentlyBorrowed;
    std::vector<std::string> borrowHistory;
    int reservations;
public:
    Account() {reservations = 0;}

    void addBorrowed(const std::string &isbn) {
        BorrowInfo bi { isbn, currentDaysSinceEpoch() };
        currentlyBorrowed.push_back(bi);
    }

    bool returnBorrowed(const std::string &isbn) {
        for(auto it = currentlyBorrowed.begin(); it != currentlyBorrowed.end(); ++it) {
            if(it->ISBN == isbn) {
                borrowHistory.push_back(isbn);
                currentlyBorrowed.erase(it);
                return true;
            }
        }
        return false;
    }

    bool isBorrowing(const std::string &isbn) const {
        for(const auto &b : currentlyBorrowed) {
            if(b.ISBN == isbn) return true;
        }
        return false;
    }

    int borrowedCount() const {
        return (int) currentlyBorrowed.size();
    }

    long long getBorrowDay(const std::string &isbn) const {
        for(const auto &b : currentlyBorrowed) {
            if(b.ISBN == isbn) return b.borrowDay;
        }
        return -1;
    }

    const std::vector<BorrowInfo>& getCurrentBorrows() const {
        return currentlyBorrowed;
    }
    const std::vector<std::string>& getHistory() const {
        return borrowHistory;
    }

    void updateReservations(int r) { reservations = r; }
    int getReservations() const { return reservations; }
};

class Student : public User {
public:
    Student(const std::string &u, const std::string &p,
            const std::string &n, double f=0.0)
        : User(u,p,n,"Student",f) {}

    int getMaxBooksAllowed() const override { return 3; }
    int getMaxBorrowDays()   const override { return 15; }

    void borrowBook(const std::string &isbn) override {
        std::cout << "Borrow request by Student: " << name
                  << " for ISBN: " << isbn << "\n";
    }

    void returnBook(const std::string &isbn) override {
        std::cout << "Return request by Student: " << name
                  << " for ISBN: " << isbn << "\n";
    }
};

class Faculty : public User {
public:
    Faculty(const std::string &u, const std::string &p,
            const std::string &n, double f=0.0)
        : User(u,p,n,"Faculty",f) {}

    int getMaxBooksAllowed() const override { return 5; }
    int getMaxBorrowDays()   const override { return 30; }

    void borrowBook(const std::string &isbn) override {
        std::cout << "Borrow request by Faculty: " << name
                  << " for ISBN: " << isbn << "\n";
    }

    void returnBook(const std::string &isbn) override {
        std::cout << "Return request by Faculty: " << name
                  << " for ISBN: " << isbn << "\n";
    }

    // Typically faculty have no monetary fines
    bool hasFines() const override { return false; }
};

class Librarian : public User {
public:
    Librarian(const std::string &u, const std::string &p,
              const std::string &n)
        : User(u,p,n,"Librarian",0.0) {}

    int getMaxBooksAllowed() const override { return 0; }
    int getMaxBorrowDays()   const override { return 0; }

    void borrowBook(const std::string &isbn) override {
        std::cout << "Librarian cannot borrow books.\n";
    }

    void returnBook(const std::string &isbn) override {
        std::cout << "Librarian cannot return books.\n";
    }
};


class Book {
private:
    std::string ISBN;
    std::string title;
    std::string author;
    std::string publisher;
    int year;
    BookStatus status;
    std::string reservedBy;
public:
    Book()
        : year(0), status(BookStatus::AVAILABLE), reservedBy("") {}
    Book(const std::string &i, const std::string &t, const std::string &a,
         const std::string &pub, int y, BookStatus st)
        : ISBN(i), title(t), author(a), publisher(pub), year(y), status(st),
          reservedBy("") {}

    const std::string& getISBN()      const { return ISBN; }
    const std::string& getTitle()     const { return title; }
    const std::string& getAuthor()    const { return author; }
    const std::string& getPublisher() const { return publisher; }
    int  getYear()                    const { return year; }
    BookStatus getStatus()            const { return status; }
    std::string getStatusString()     const { return bookStatusToString(status); }
    const std::string& getReservedBy() const { return reservedBy; }

    void setStatus(BookStatus s)           { status = s; }
    void setReservedBy(const std::string &uid) { reservedBy = uid; }

    void setISBN(const std::string &i)      { ISBN = i; }
    void setTitle(const std::string &t)     { title = t; }
    void setAuthor(const std::string &a)    { author = a; }
    void setPublisher(const std::string &p) { publisher = p; }
    void setYear(int y)                     { year = y; }
};


class Library {
private:
    std::vector<Book> books;
    std::vector<User*> users;

public:
    Library() {}
    ~Library() {
        // Clean up allocated users
        for(User* u : users) {
            delete u->account;
            delete u;
        }
    }

    
    void loadBooks(const std::string &filename) {
        std::ifstream fin(filename);
        if(!fin.is_open()) {
            std::cerr << "Could not open " << filename << "\n";
            return;
        }
        books.clear();
        std::string line;
        while(std::getline(fin, line)) {
            if(line.empty()) continue;
            std::stringstream ss(line);
            std::string i, t, a, pub, ystr, st;
            std::getline(ss, i, ',');
            std::getline(ss, t, ',');
            std::getline(ss, a, ',');
            std::getline(ss, pub, ',');
            std::getline(ss, ystr, ',');
            std::getline(ss, st, ',');

            int y = std::stoi(ystr);
            BookStatus bst = stringToBookStatus(st);
            Book bk(i, t, a, pub, y, bst);
            bk.setReservedBy(""); // not storing reserved user in file (can be known while reading through the transactions
            books.push_back(bk);
        }
        fin.close();
    }

    void loadUsers(const std::string &filename) {
        std::ifstream fin(filename);
        if(!fin.is_open()) {
            std::cerr << "Could not open " << filename << "\n";
            return;
        }
        users.clear();
        std::string line;
        while(std::getline(fin, line)) {
            if(line.empty()) continue;
            std::stringstream ss(line);
            std::string uid, pwd, nm, rl, fineStr;
            std::getline(ss, uid, ',');
            std::getline(ss, pwd, ',');
            std::getline(ss, nm, ',');
            std::getline(ss, rl, ',');
            std::getline(ss, fineStr, ',');

            double f = std::stod(fineStr);

            User* uPtr = nullptr;
            if(rl == "Student") {
                uPtr = new Student(uid, pwd, nm, f);
            } else if(rl == "Faculty") {
                uPtr = new Faculty(uid, pwd, nm, f);
            } else if(rl == "Librarian") {
                uPtr = new Librarian(uid, pwd, nm);
            }
            if(uPtr) {
                uPtr->account = new Account();
                uPtr->setFine(f);
                users.push_back(uPtr);
            }
        }
        fin.close();
    }

    
    void loadTransactions(const std::string &filename) {
        std::ifstream fin(filename);
        if(!fin.is_open()) {
            std::cerr << "Could not open " << filename << "\n";
            return;
        }
        std::string line;
        while(std::getline(fin, line)) {
            if(line.empty()) continue;
            std::stringstream ss(line);
            std::string uid, isbn, op, dayStr;
            std::getline(ss, uid, ',');
            std::getline(ss, isbn, ',');
            std::getline(ss, op, ',');
            std::getline(ss, dayStr, ',');

            // We ignore dayStamp except for ordering
            long long dayStamp = std::stoll(dayStr);

            User* u = findUser(uid);
            Book* b = findBook(isbn);
            if(!u || !b) continue; // skip bad lines

            if(op == "borrow") {
                b->setStatus(BookStatus::BORROWED);
                b->setReservedBy(""); 
                u->account->addBorrowed(isbn);
            }
            else if(op == "return") {
                u->account->returnBorrowed(isbn);
                b->setStatus(BookStatus::AVAILABLE);
            }
            else if(op == "reserve") {
                if(b->getStatus() == BookStatus::BORROWED &&
                   b->getReservedBy().empty())
                {
                    b->setReservedBy(uid);
                }
            }
        }
        fin.close();
    }

    
    User* findUser(const std::string &uid) {
        for(auto *u : users) {
            if(u->getUserID() == uid) return u;
        }
        return nullptr;
    }

    Book* findBook(const std::string &isbn) {
        for(auto &b : books) {
            if(b.getISBN() == isbn) return &b;
        }
        return nullptr;
    }

    void appendTransaction(const std::string &uid,
                           const std::string &isbn,
                           const std::string &op) {
        std::ofstream fout("transactions.txt", std::ios::app);
        if(!fout.is_open()) {
            std::cerr << "Cannot open transactions.txt for append.\n";
            return;
        }
        long long dayStamp = currentDaysSinceEpoch();
        fout << uid << "," << isbn << "," << op << "," << dayStamp << "\n";
        fout.close();
    }

    
    bool hasOverdueMoreThan60Days(User &faculty) {
        auto &borrows = faculty.account->getCurrentBorrows();
        int maxDays = faculty.getMaxBorrowDays(); // 30 for Faculty
        for(auto &binfo : borrows) {
            long long diff = currentDaysSinceEpoch() - binfo.borrowDay;
            if(diff > (maxDays + 60)) {
                return true;
            }
        }
        return false;
    }

    void borrowBook(User &user, const std::string &isbn) {
        // Check if user is already borrowing
        if(user.account->isBorrowing(isbn)) {
            std::cout << "You are already borrowing this book; can't borrow/reserve it.\n";
            return;
        }
        // Librarian can't borrow
        if(user.getRole() == "Librarian") {
            std::cout << "Librarian cannot borrow.\n";
            return;
        }
        // Student must pay fine first
        if(user.getRole() == "Student" && user.getFine() > 0.0) {
            std::cout << "You have unpaid fines; pay first.\n";
            return;
        }
        // Check limit: borrowed+reserved should be less than max allowed
        if(user.account->borrowedCount() + user.account->getReservations() >= user.getMaxBooksAllowed()) {
            std::cout << "You reached max books allowed.\n";
            return;
        }
        // Faculty check overdue > 60 days
        if(user.getRole() == "Faculty") {
            if(hasOverdueMoreThan60Days(user)) {
                std::cout << "Cannot borrow; you have a book overdue > 60 days.\n";
                return;
            }
        }

        Book* b = findBook(isbn);
        if(!b) {
            std::cout << "Book not found.\n";
            return;
        }

        // If someone else is borrowing it, offer reservation
        if(b->getStatus() == BookStatus::BORROWED) {
            std::cout << "This book is already borrowed by someone else.\n";
            if(!b->getReservedBy().empty()) {
                std::cout << "It's already reserved by: " << b->getReservedBy() << "\n";
            } else {
                std::cout << "Do you want to reserve it? (y/n): ";
                char c;
                std::cin >> c;
                if(c=='y' || c=='Y') {
                    b->setReservedBy(user.getUserID());
                    appendTransaction(user.getUserID(), isbn, "reserve");
                    user.account->updateReservations(user.account->getReservations() + 1);
                    std::cout << "Book reserved successfully.\n";
                }
            }
            return;
        }

        // Otherwise it's available => borrow now
        b->setStatus(BookStatus::BORROWED);
        user.account->addBorrowed(isbn);
        // Clear any previous reservation just in case
        b->setReservedBy("");
        appendTransaction(user.getUserID(), isbn, "borrow");
        std::cout << "Book borrowed successfully.\n";
    }

    
    void returnBook(User &user, const std::string &isbn) {
        if(user.getRole() == "Librarian") {
            std::cout << "Librarian doesn't borrow books.\n";
            return;
        }
        // Must actually be borrowing
        if(!user.account->isBorrowing(isbn)) {
            std::cout << "You are not borrowing this book.\n";
            return;
        }

        // Overdue check
        long long dayStamp = user.account->getBorrowDay(isbn);
        long long diff = currentDaysSinceEpoch() - dayStamp;
        int maxDays = user.getMaxBorrowDays();
        if(diff > maxDays && user.getRole() == "Student") {
            long long overdueDays = diff - maxDays;
            double addedFine = overdueDays * 10.0;
            user.setFine(user.getFine() + addedFine);
            std::cout << "Book overdue by " << overdueDays
                      << " days. Fine added: " << addedFine << "\n";
        } else if(diff > maxDays && user.getRole() == "Faculty") {
            long long overdueDays = diff - maxDays;
            std::cout << "Returned " << overdueDays 
                      << " days late. (No fine for faculty)\n";
        }

        // Remove from user's borrowed list
        user.account->returnBorrowed(isbn);

        Book* b = findBook(isbn);
        if(!b) {
            // Should never happen if user had it, but just in case
            std::cout << "Book not found in library list.\n";
            return;
        }

        // Step 1: Append the "return" transaction now
        // so that the transaction log sees them returning
        appendTransaction(user.getUserID(), isbn, "return");

        // Step 2: Set the book to AVAILABLE in memory first
        b->setStatus(BookStatus::AVAILABLE);

        // Step 3: If it was reserved by someone else, give it to them immediately
        if(!b->getReservedBy().empty()) {
            std::string reservedUID = b->getReservedBy();
            User* reservedUser = findUser(reservedUID);
            b->setReservedBy(""); // clear reservation

            if(reservedUser) {
                // set it borrowed by that user
                b->setStatus(BookStatus::BORROWED);
                reservedUser->account->addBorrowed(isbn);

                // record the auto-borrow in transactions
                appendTransaction(reservedUID, isbn, "borrow");

                std::cout << "Book auto-borrowed by reserved user: " 
                          << reservedUID << "\n";
            }
            else {
                // if no such user actually exists, remain available
                b->setStatus(BookStatus::AVAILABLE);
            }
        }
        // else if no reservation, remain AVAILABLE

        std::cout << "Book returned successfully.\n";
    }

    void addBook() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string i,t,a,p;
        int y;
        std::cout << "Enter ISBN: ";
        std::getline(std::cin, i);
        if(findBook(i)) {
            std::cout << "Book with this ISBN already exists!\n";
            return;
        }
        std::cout << "Enter Title: ";
        std::getline(std::cin, t);
        std::cout << "Enter Author: ";
        std::getline(std::cin, a);
        std::cout << "Enter Publisher: ";
        std::getline(std::cin, p);
        std::cout << "Enter Year: ";
        std::cin >> y;

        Book bk(i,t,a,p,y,BookStatus::AVAILABLE);
        books.push_back(bk);
        std::cout << "Book added.\n";
    }

    void removeBook() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string isbn;
        std::cout << "Enter ISBN to remove: ";
        std::getline(std::cin, isbn);
        for(auto it = books.begin(); it != books.end(); ++it) {
            if(it->getISBN() == isbn) {
                if(it->getStatus() == BookStatus::BORROWED) {
                    std::cout << "Cannot remove a borrowed book.\n";
                    return;
                }
                books.erase(it);
                std::cout << "Book removed.\n";
                return;
            }
        }
        std::cout << "No such book.\n";
    }

    void updateBook() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string isbn;
        std::cout << "Enter ISBN to update: ";
        std::getline(std::cin, isbn);
        Book* b = findBook(isbn);
        if(!b) {
            std::cout << "No such book.\n";
            return;
        }
        std::string newTitle, newAuthor, newPub;
        int newYear;
        std::cout << "Enter new Title (or . to skip): ";
        std::getline(std::cin, newTitle);
        if(newTitle != ".") b->setTitle(newTitle);

        std::cout << "Enter new Author (or . to skip): ";
        std::getline(std::cin, newAuthor);
        if(newAuthor != ".") b->setAuthor(newAuthor);

        std::cout << "Enter new Publisher (or . to skip): ";
        std::getline(std::cin, newPub);
        if(newPub != ".") b->setPublisher(newPub);

        std::cout << "Enter new Year (or 0 to skip): ";
        std::cin >> newYear;
        if(newYear != 0) b->setYear(newYear);

        std::cout << "Book updated.\n";
    }

    void addUser() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string uid, pwd, nm, rl;
        std::cout << "Enter userID: ";
        std::getline(std::cin, uid);
        if(findUser(uid)) {
            std::cout << "User with this ID already exists.\n";
            return;
        }
        std::cout << "Enter password: ";
        std::getline(std::cin, pwd);
        std::cout << "Enter name: ";
        std::getline(std::cin, nm);
        std::cout << "Enter role (Student/Faculty/Librarian): ";
        std::getline(std::cin, rl);

        User* uPtr = nullptr;
        if(rl == "Student") {
            uPtr = new Student(uid, pwd, nm);
        } else if(rl == "Faculty") {
            uPtr = new Faculty(uid, pwd, nm);
        } else if(rl == "Librarian") {
            uPtr = new Librarian(uid, pwd, nm);
        } else {
            std::cout << "Invalid role.\n";
            return;
        }
        uPtr->account = new Account();
        users.push_back(uPtr);
        std::cout << "User added.\n";
    }

    void removeUser() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string uid;
        std::cout << "Enter userID to remove: ";
        std::getline(std::cin, uid);
        for(auto it = users.begin(); it != users.end(); ++it) {
            if((*it)->getUserID() == uid) {
                if((*it)->account->borrowedCount() > 0) {
                    std::cout << "Cannot remove user who still borrows a book.\n";
                    return;
                }
                delete (*it)->account;
                delete (*it);
                users.erase(it);
                std::cout << "User removed.\n";
                return;
            }
        }
        std::cout << "No such user.\n";
    }

    void showAllBooks() {
        std::cout << "\n----- All Books -----\n";
        for(const auto &b : books) {
            std::cout << "ISBN: " << b.getISBN()
                      << "\nTitle: " << b.getTitle()
                      << "\nAuthor: " << b.getAuthor()
                      << "\nPublisher: " << b.getPublisher()
                      << "\nYear: " << b.getYear()
                      << "\nStatus: " << b.getStatusString()
                      << "\nReservedBy: "
                      << (b.getReservedBy().empty() ? "None" : b.getReservedBy())
                      << "\n\n";
        }
        std::cout << "---------------------\n";
    }

    void showAllUsers() {
        std::cout << "\n----- All Users -----\n";
        for(auto *u : users) {
            std::cout << "UserID: " << u->getUserID()
                      << ", Name: " << u->getName()
                      << ", Role: " << u->getRole()
                      << ", Fine: " << u->getFine()
                      << "\n";
        }
        std::cout << "---------------------\n";
    }

    void showUserAccount() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string uid;
        std::cout << "Enter userID: ";
        std::getline(std::cin, uid);
        User* u = findUser(uid);
        if(!u) {
            std::cout << "No such user.\n";
            return;
        }
        std::cout << "User: " << u->getName() << " ("
                  << u->getRole() << "), Fine: " << u->getFine() << "\n";
        auto &cb = u->account->getCurrentBorrows();
        std::cout << "Currently Borrowed:\n";
        for(const auto &bi : cb) {
            std::cout << "  ISBN: " << bi.ISBN 
                      << ", BorrowedDay: " << bi.borrowDay << "\n";
        }
        std::cout << "History:\n";
        for(const auto &h : u->account->getHistory()) {
            std::cout << "  ISBN: " << h << "\n";
        }
    }

    // Save data
    void saveBooks(const std::string &filename) {
        std::ofstream fout(filename);
        if(!fout.is_open()) {
            std::cerr << "Could not open " << filename << "\n";
            return;
        }
        for(const auto &b : books) {
            fout << b.getISBN() << ","
                 << b.getTitle() << ","
                 << b.getAuthor() << ","
                 << b.getPublisher() << ","
                 << b.getYear() << ","
                 << b.getStatusString() << "\n";
        }
        fout.close();
    }

    void saveUsers(const std::string &filename) {
        std::ofstream fout(filename);
        if(!fout.is_open()) {
            std::cerr << "Could not open " << filename << "\n";
            return;
        }
        for(auto *u : users) {
            fout << u->getUserID() << ","
                 << u->getPassword() << ","
                 << u->getName() << ","
                 << u->getRole() << ","
                 << u->getFine() << "\n";
        }
        fout.close();
    }
};


int main() {
    Library lib;
    // Load all data once at program start
    lib.loadBooks("books.txt");
    lib.loadUsers("users.txt");
    lib.loadTransactions("transactions.txt");

    while(true) {
        std::cout << "\n=====================\n"
                  << "Welcome to the Library!\n"
                  << "1. Login\n"
                  << "0. Exit\n"
                  << "Choice: ";
        int choice;
        std::cin >> choice;

        if(choice == 0) {
            // Exit the entire program
            // We could optionally save here if we want the final state
            lib.saveBooks("books.txt");
            lib.saveUsers("users.txt");
            std::cout << "Exiting... Data saved.\n";
            break;
        }
        else if(choice == 1) {
            // Prompt for login
            std::string uid, pwd;
            std::cout << "UserID: ";
            std::cin >> uid;
            std::cout << "Password: ";
            std::cin >> pwd;

            // Validate
            User* currentUser = lib.findUser(uid);
            if(!currentUser || currentUser->getPassword() != pwd) {
                std::cout << "Invalid credentials.\n";
                // Return to main menu
                continue;
            }

            // We have a valid user. Now present the user's session menu.
            // We'll allow them to do normal library operations or logout (0).
            while(true) {
                // If Student or Faculty
                if(currentUser->getRole() == "Student" || currentUser->getRole() == "Faculty") {
                    std::cout << "\n---- Menu (" << currentUser->getRole() << ") ----\n"
                              << "1. Show all books\n"
                              << "2. Borrow a book\n"
                              << "3. Return a book\n"
                              << "4. View Borrowings\n"
                              << "5. View Transaction History\n"
                              << "6. Pay fines\n"
                              << "0. Save and Logout\n"
                              << "Choice: ";
                    int ch;
                    std::cin >> ch;

                    if(ch == 0) {
                        // User wants to logout
                        lib.saveBooks("books.txt");
                        lib.saveUsers("users.txt");
                        std::cout << "Library data saved. Logging out...\n";
                        break;  // exit this user session, go back to main
                    }
                    else if(ch == 1) {
                        lib.showAllBooks();
                    } else if(ch == 2) {
                        std::string isbn;
                        std::cout << "Enter ISBN to borrow: ";
                        std::cin >> isbn;
                        lib.borrowBook(*currentUser, isbn);
                    } else if(ch == 3) {
                        std::string isbn;
                        std::cout << "Enter ISBN to return: ";
                        std::cin >> isbn;
                        lib.returnBook(*currentUser, isbn);
                    }else if(ch==4){
                        auto &cb = currentUser->account->getCurrentBorrows();
                        std::cout << "Currently Borrowed:\n";
                        if(cb.empty()) {
                            std::cout << "  None\n";
                        } else {
                            for(const auto &bi : cb) {
                                std::cout << "  ISBN: " << bi.ISBN
                                          << ", BorrowedDay: " << bi.borrowDay << "\n";
                            }
                        }
                    } else if(ch==5){
                        auto &h = currentUser->account->getHistory();
                        std::cout << "History:\n";
                        if(h.empty()) {
                            std::cout << "  No History\n";
                        } else {
                            for(const auto &isbn : h) {
                                std::cout << "  ISBN: " << isbn << "\n";
                            }
                        }
                    } else if(ch == 6) {
                        currentUser->payFine();
                    } else {
                        std::cout << "Invalid choice.\n";
                    }
                }
                // Librarian menu
                else if(currentUser->getRole() == "Librarian") {
                    std::cout << "\n---- Menu (Librarian) ----\n"
                              << "1. Show all books\n"
                              << "2. Show all users\n"
                              << "3. Show all transactions\n"
                              << "4. Show user account\n"
                              << "5. Manage library (add/remove/update books, add/remove users)\n"
                              << "0. Save and Logout\n"
                              << "Choice: ";
                    int ch;
                    std::cin >> ch;

                    if(ch == 0) {
                        lib.saveBooks("books.txt");
                        lib.saveUsers("users.txt");
                        std::cout << "Library data saved. Logging out...\n";
                        break;  // return to main
                    }
                    else if(ch == 1) {
                        lib.showAllBooks();
                    } else if(ch == 2) {
                        lib.showAllUsers();
                    } else if(ch == 3) {
                        // Show all transactions
                        std::ifstream fin("transactions.txt");
                        if(!fin.is_open()) {
                            std::cerr << "Could not open transactions.txt\n";
                            continue;
                        }
                        std::string line;
                        std::cout << "\n----- All Transactions -----\n";
                        while(std::getline(fin, line)) {
                            // get comma seperated elements and use them
                            std::stringstream ss(line);
                            std::string uid, isbn, op, dayStr;
                            std::getline(ss, uid, ',');
                            std::getline(ss, isbn, ',');
                            std::getline(ss, op, ',');
                            std::getline(ss, dayStr, ',');
                            std::cout << "UserID: " << uid
                                      << ", ISBN: " << isbn
                                      << ", Operation: " << op
                                      << ", DayStamp: " << dayStr << "\n";
                        }
                        std::cout << "-----------------------------\n";
                        fin.close();
                    } else if(ch == 4) {
                        lib.showUserAccount();
                    } else if(ch == 5) {
                        // sub-menu for library management
                        while(true) {
                            std::cout << "\n-- Manage Library --\n"
                                      << "a) Add Book\n"
                                      << "b) Remove Book\n"
                                      << "c) Update Book\n"
                                      << "d) Add User\n"
                                      << "e) Remove User\n"
                                      << "f) Back\n"
                                      << "Choice: ";
                            char c; 
                            std::cin >> c;
                            if(c == 'a') {
                                lib.addBook();
                            } else if(c == 'b') {
                                lib.removeBook();
                            } else if(c == 'c') {
                                lib.updateBook();
                            } else if(c == 'd') {
                                lib.addUser();
                            } else if(c == 'e') {
                                lib.removeUser();
                            } else if(c == 'f') {
                                break; // exit sub-menu
                            } else {
                                std::cout << "Invalid choice.\n";
                            }
                        }
                    } else {
                        std::cout << "Invalid choice.\n";
                    }
                }
            } // end of user-session while(true)

        } // end choice == 1
        else {
            std::cout << "Invalid choice.\n";
        }
    } // end main while(true)

    return 0;
}
