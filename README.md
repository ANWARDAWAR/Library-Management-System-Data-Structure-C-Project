# Library Management System

A single-file C++17 console application that demonstrates a feature-rich Library Management System built around core data structures. The project is designed as both a practical LMS prototype and a compact data structures showcase.

## Overview

This application manages books through an interactive terminal menu and uses multiple custom structures to model common library workflows:

- A singly linked list stores the primary book catalog.
- A binary search tree provides faster lookup by book ID.
- A queue tracks pending issue requests when copies are unavailable.
- A stack supports undo for recent delete and issue actions.

The code is intentionally self-contained in `lms.cpp`, making it easy to compile, inspect, and extend without external dependencies.

## Features

- Add new books with title, author, category, quantity, and availability tracking
- Display the full catalog in a formatted table
- Search books by ID or name
- Update book title and author information
- Delete books with undo support
- Issue books and automatically queue requests when stock is exhausted
- Return books with inventory validation
- View all pending issue requests
- Undo the last delete or issue action
- Display summary statistics for the collection
- Preloaded sample books for immediate testing

## Data Structures Used

| Structure | Purpose |
| --- | --- |
| Singly Linked List | Primary storage for all books |
| Binary Search Tree | Fast book lookup by ID |
| Queue | Pending issue requests |
| Stack | Undo history for delete and issue actions |

## Requirements

- A C++17-compatible compiler such as `g++`
- A terminal or command prompt

## Build

From the project root:

```bash
g++ -std=c++17 -g -Wall -Wextra -pedantic lms.cpp -o lms
```

The repository also includes a VS Code build task that uses the same compiler flags for the active file.

## Run

```bash
./lms
```

## Usage

When the program starts, it loads a small set of sample books and presents a numbered menu. Choose an option by entering the corresponding number.

Typical workflow:

1. Display the catalog to inspect the current inventory.
2. Search for a book by ID or name.
3. Add, update, or delete records as needed.
4. Issue and return books while tracking availability.
5. Review the queue and undo stack behavior.
6. Check statistics for a quick summary of the library state.

## Project Structure

```text
.
├── lms.cpp
├── LMS_Documentation.pdf
├── lms
└── .vscode/
```

## Notes

- The project is a console application and does not require a database.
- Book records are stored in memory only, so all data is reset when the program exits.
- The binary search tree is used as an index over the linked list data, avoiding duplicate book copies.

## Possible Improvements

- Persist data to disk so books survive restarts
- Add stronger validation for quantities and duplicate titles
- Support deleting or updating queued issue requests
- Introduce member records and issue history
- Add automated tests for the data structure operations

## License

No license has been specified for this project.
