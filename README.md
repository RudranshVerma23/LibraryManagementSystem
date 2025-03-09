# Library Management System

This repository contains a **Library Management System** implemented in C++ using Object-Oriented Programming (OOP) principles. It demonstrates a simplified approach to **borrowing, returning, and reserving books** in a library, with user roles for **Students**, **Faculty**, and a **Librarian**.

## Installation and Setup

1. **Clone or Download** this repository to your local machine.
2. Ensure you have a **C++ compiler** (e.g., `g++`, `clang++`, or MSVC) installed.
3. Locate or create the required data files:
   - **`books.txt`**: Contains the list of books (ISBN, title, author, publisher, year, status).
   - **`users.txt`**: Contains user records (userID, password, name, role, fine).
   - **`transactions.txt`** (optional at first): Will be created or appended to automatically for logging borrow/return/reserve transactions.

   Place these files in the same folder as `main.cpp`.

4. **Build** the program:
   ```bash
   g++ -o library main.cpp
   ```
   (Or use your preferred C++ compiler and build system.)

5. **Run** the resulting executable:
   ```bash
   ./library
   ```

## How to Use

1. **At startup**, the program asks whether you want to **Login** or **Exit**.
2. **Login** requires a valid `userID` and `password` that must exist in `users.txt`.
3. Depending on the **role** of the logged-in user, you see different menu options:

   ### Student / Faculty
   - **Show all books**  
   - **Borrow a book** (if available)  
   - **Return a borrowed book**  
   - **View borrowed books**  
   - **View transaction history** (their own borrow/return history)  
   - **Pay fines** (Students only pay if overdue; Faculty never accumulate fines)

   ### Librarian
   - **Show all books**
   - **Show all users**
   - **Show entire transaction log** (`transactions.txt`)
   - **Show a particular user’s account** (borrowed books, fines, etc.)
   - **Manage** library’s books and users (add, remove, update)

4. The code automatically logs **transactions** (borrow/return/reserve) by appending lines to `transactions.txt`.
5. When **returning** a reserved book:
   - The returned book is marked **Available** in real-time,  
   - If reserved by someone else, it **immediately** gets auto-borrowed by that reserved user and a **second** “borrow” transaction is logged for them.  
   - In `loadTransactions()`, the `return` lines simply set the book to **Available** again (i.e., do not auto-borrow for the reserved user). Instead, the separate `borrow` transaction line for the reserved user ensures consistent replay of the library state.

## Files Description

- **`main.cpp`**  
  Single-file C++ source containing the entire Library Management code (see above for the approach details).

- **`books.txt`**  
  CSV lines describing books in the format:
  ```
  ISBN,Title,Author,Publisher,Year,Status
  ```

- **`users.txt`**  
  CSV lines describing users in the format:
  ```
  userID,password,name,role,fine
  ```

- **`transactions.txt`**  
  Appended in real-time whenever a user borrows, returns, or reserves a book. Format:
  ```
  userID,ISBN,operation,dayStamp
  ```
  Day stamp is the number of days since epoch, used to calculate overdue and keep chronological order.

## Credits

- **Author & Code**: [**Rudransh Verma**](https://github.com/RudranshVerma23)

## Contributing

Feel free to open an issue or pull request if you would like to improve or extend the project.
