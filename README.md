# Student Record Management System (C mini project)

A console program, written in plain C (C11), that manages student records
(**roll number, name, percentage**) in a **singly linked list** and keeps them
in a binary file, `student.dat`.

The program is called **`student`**. It is built from several source files, as
the assignment asks: `stud_add.c`, `stud_del.c`, `stud_show.c`, `stud_mod.c`
and `stud_save.c` (plus `stud_sort.c`, the linked-list code and `main.c`).

No external libraries. Works on Windows, Linux and macOS.

> **New to C or VS Code? Start with [HOW_TO_RUN.md](HOW_TO_RUN.md)** - a
> step-by-step guide (install, open in VS Code, press `Ctrl+Shift+B`).

## The menu

```
**** STUDENT RECORD MENU ****
A/a : Add New Record
D/d : Delete a Record
S/s : Show the List
M/m : Modify a Record
V/v : Save
T/t : Sort the List
E/e : Exit
Enter Your Choice:
```

Upper or lower case both work. Typing anything else just asks again.

| Choice | What happens |
|---|---|
| **A** | Asks for the **name** and **percentage**. The roll number is assigned automatically: the smallest positive number not in use (so a deleted roll number is reused). |
| **D** | Submenu: **R** = delete by roll number (deleted directly), **N** = delete by name (shows all matching records, then asks which roll number to delete). |
| **S** | Prints every record in a table. |
| **M** | Step 1 - find the record by **R**oll number (used directly), **N**ame or **P**ercentage (matches are shown, then you pick a roll number). Step 2 - choose **N** (new name) or **P** (new percentage). |
| **V** | Saves all records to `student.dat`. |
| **T** | **N** = sort by name (A-Z), **P** = sort by percentage (lowest first). |
| **E** | **S** = save and exit, **E** = exit without saving. |

Nothing is written to disk until you choose **V** (or **S** when exiting).
`student.dat` is loaded automatically when the program starts.

Example of the table (**S**):

```
--------------------------------------
Roll No     Name         Percentage
--------------------------------------
1           Arun         85.5
2           Priya        91.0
--------------------------------------
```

## 1. Install a C compiler (one time)

You need `gcc` (or any C11 compiler).

* **Windows** - install MinGW-w64, for example the "WinLibs" build from
  <https://winlibs.com> or via [MSYS2](https://www.msys2.org). Add its `bin`
  folder to your PATH, then open a **new** terminal and check:
  ```
  gcc --version
  ```
* **Linux** - `sudo apt install build-essential` (Debian/Ubuntu)
* **macOS** - `xcode-select --install`

## 2. Build and run

**Windows (Command Prompt or PowerShell), inside this folder:**
```
build.bat run
```

**Linux / macOS (or MSYS2 / Git Bash):**
```
make run
```

Both compile the program into the `build/` folder and start it. Later you can
start it directly: `build\student.exe` (Windows) or `./build/student`
(Linux/macOS).

**Where is the data?** `student.dat` is created in the folder you start the
program *from* (the current folder). Start it from the same folder each time
and your records will be there.

**Want demo data first?** Copy `sample_data/student.dat` into the folder you
run the program from (e.g. the project folder) before starting it - it holds 8
students.

## 3. Run the tests

```
build.bat test        (Windows)
make test             (Linux / macOS)
```
This checks the student rules, the linked list (add, remove, search, sort,
"smallest free roll number"), and saving/loading `student.dat` including
damaged files. It should end with `ALL TESTS PASSED`.

## Project layout

```
student-record-system/
|-- include/            header files (the "public interface" of each module)
|-- src/                implementation (.c files)
|-- tests/test_main.c   automated tests for the backend
|-- sample_data/        demo student.dat (8 students)
|-- Makefile            build script for Linux / macOS
|-- build.bat           build script for Windows
```

| File | Responsibility |
|---|---|
| `main.c` | Shows the menu, calls the `stud_*()` function you chose, loads at start-up |
| `stud_add.c` | `stud_add()` - A/a |
| `stud_del.c` | `stud_del()` - D/d |
| `stud_show.c` | `stud_show()` - S/s, and the table printer |
| `stud_mod.c` | `stud_mod()` - M/m |
| `stud_sort.c` | `stud_sort()` - T/t |
| `stud_save.c` | `stud_save()` - V/v, `stud_load()` at start-up, `stud_backup()` |
| `stud_common.c` | Small helpers shared by add/delete/modify (ask for a name, search + pick a record) |
| `student_list.c` | **The linked list**: append, find, remove, search, merge sort, smallest free roll |
| `student.c` | The `Student` node: creation (`malloc`) and validation rules |
| `input.c` | Safe keyboard input (reads whole lines, asks again on bad input) |
| `util.c`, `status.c`, `platform.c` | String helpers, error codes, the one bit of OS-specific code |

### Architecture

Each layer only calls the layers below it. The lower layers never print
anything, which is what makes them easy to test.

```
   main.c                          menu loop, start-up / shut-down
      |
   stud_add.c  stud_del.c  stud_show.c  stud_mod.c  stud_sort.c   <- one file per menu item
   stud_common.c   input.c         (talk to the user)
      |
   student_list.c                  linked list logic (no printing)
   stud_save.c                     reading / writing student.dat
      |
   student.c                       the node + validation
   util.c  status.c  platform.c    helpers
```

The linked list:

```
 head -> [1|Arun|85.5] -> [2|Priya|91.0] -> [3|Kiran|60.0] -> NULL
```

Every node is created with `calloc` in `student_create()` and released with
`free` (when deleted, or all at once by `student_list_free()` on exit).

## The `student.dat` file

A small binary file (this is why you cannot read it in Notepad):

| Bytes | Content |
|---|---|
| 0-3 | the letters `STUD` (identifies the file type) |
| 4-7 | number of records |
| then, per record (40 bytes) | roll number (4) - name (32, padded with 0) - percentage in hundredths (4), e.g. 85.5 is stored as 8550 |

Numbers are written byte by byte (little-endian), so the file can be copied to
another computer. When loading, every value is checked; if the file is damaged
the program keeps a copy as `student.dat.bak`, tells you, and starts with an
empty list instead of crashing or overwriting your data.

## Rules and choices worth knowing

* **Roll numbers** are never typed by the user and are always unique.
* **Name**: 1-31 characters (spaces inside are fine, two students may share a
  name - that is why delete/modify by name asks for the roll number).
  Searching by name ignores upper/lower case and matches part of a name
  (`arun` finds `Arun` and `Arun Raj`).
* **Percentage**: 0 to 100, stored with 2 decimals. Searching by percentage
  needs an exact match.
* When you pick a roll number after a name/percentage search, it must be one of
  the records that were just shown (this prevents deleting the wrong student).
* **Sort by percentage** puts the *lowest* first. If your teacher wants highest
  first, change `compare_by_percentage()` in `src/student_list.c` (swap the
  `-1` and `1`).
* Sorting changes the order of the list in memory; the new order is written to
  the file the next time you save.

## C concepts used (handy for a viva)

* `struct` with a `next` pointer - a **singly linked list**
* Dynamic memory: `calloc` for each node, `free` to release it, no leaks
* Pointer-to-pointer technique for removing a node (`student_list_remove`)
* **Merge sort on a linked list** - sorts by re-linking nodes, no copying
* Binary file I/O: `fopen("rb"/"wb")`, `fread`, `fwrite`, safe save via a temp
  file, validation when loading
* Function pointers (the comparator and the "matcher" used by the searches)
* Modular design: headers (interfaces) vs source files (implementations),
  enums for result codes, input validation without `scanf`

## How to extend it

* **Add a field** (e.g. phone number): add it to `Student` in `student.h`,
  validate it in `student_create()`, then update `stud_save.c` (record size),
  the prompts in `stud_add.c` / `stud_mod.c` and the table in `stud_show.c`.
  The tests tell you what you broke.
* **Add a menu item**: write a new `stud_xxx.c`, declare it in `stud_ops.h`,
  add a line to `print_menu()` and a `case` in `main()`. Both build scripts
  pick up new `.c` files in `src/` automatically.
