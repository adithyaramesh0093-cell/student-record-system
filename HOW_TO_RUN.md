# How to run this project (for beginners)

This is a program written in the **C language**. You cannot just double-click
the source files: C code first has to be **compiled** (translated into a
program the computer understands) by a free tool called a *compiler*. After
that you can run it.

So you need two free things, once:

| Tool | What it is |
|---|---|
| **VS Code** | The editor where you open and run the project |
| **gcc** | The C compiler (the translator) |

The steps below are for **Windows**. (Mac/Linux: see the end.)

---

## Part 1 - Install the two tools (one time only)

1. Click the **Start** button, type **PowerShell**, and open it.
2. Copy this line into PowerShell and press **Enter** (installs VS Code):
   ```
   winget install Microsoft.VisualStudioCode
   ```
3. Then copy this line and press **Enter** (installs the C compiler):
   ```
   winget install BrechtSanders.WinLibs.POSIX.UCRT
   ```
   If it asks you to agree to terms, type `Y` and press Enter.
4. **Close PowerShell completely**, then open it again (this is important - it
   makes Windows notice the new compiler) and type:
   ```
   gcc --version
   ```
   If it prints a few lines starting with `gcc (...)`, the compiler is ready.

   *If it says "gcc is not recognized", see "Problems" at the bottom.*

   *If `winget` itself is not recognized, install VS Code from
   <https://code.visualstudio.com> and the compiler from <https://winlibs.com>
   (see "Problems" below for the PATH step).*

5. Open **VS Code**. Click the **Extensions** icon on the left (four small
   squares, or press `Ctrl+Shift+X`), search for **C/C++** (by Microsoft) and
   click **Install**. (This only adds colours and hints; the project also
   works without it.)

---

## Part 2 - Run the project

1. **Unzip** the project: right-click `student-record-system.zip` ->
   **Extract All**.
2. In VS Code choose **File -> Open Folder...** and select the extracted
   folder named **`student-record-system`** (the one that contains
   `build.bat` and `README.md`).
   If VS Code asks "Do you trust the authors?", click **Yes**.
3. Press **`Ctrl + Shift + B`**.

   (If nothing happens: menu **Terminal -> Run Build Task...** and pick
   *Build and run the program*.)
4. A terminal opens at the bottom of VS Code. After a second you will see
   `Build OK` and then the program:
   ```
   **** STUDENT RECORD MENU ****
   A/a : Add New Record
   ...
   Enter Your Choice:
   ```
5. The program is now running. **Click inside the terminal**, type a letter
   (for example `a`) and press **Enter**. Follow the questions on screen.
6. To quit: type `e`, press Enter, then type `s` (save and exit) or `e` (exit
   without saving).

Your records are saved in a file named **`student.dat`** inside the project
folder when you choose **V** (Save) or **S** when exiting.

**Want some ready-made students to play with?** Before step 3, copy the file
`sample_data\student.dat` into the main project folder (next to `build.bat`).

### Next time

Open VS Code (it remembers the folder) and press **`Ctrl + Shift + B`** again.

### Check that everything works (optional)

**Terminal -> Run Task... -> Run the tests.** The last line should say
`ALL TESTS PASSED`.

### Don't want VS Code?

Open the project folder in File Explorer, click the address bar at the top,
type `cmd` and press Enter, then type:
```
build.bat run
```

---

## Problems

**"gcc is not recognized" / "gcc was not found"**

1. Close VS Code and PowerShell completely and open them again. (Most of the
   time this fixes it.)
2. Still not working? Restart the laptop once.
3. Still not working? The compiler's folder is not on Windows' PATH list.
   Download the compiler **by hand**: <https://winlibs.com> -> choose the
   "UCRT runtime" zip (Win64) -> extract it to `C:\winlibs`. Then:
   Start -> type **Edit the system environment variables** -> **Environment
   Variables** -> select **Path** under "User variables" -> **Edit** ->
   **New** -> type `C:\winlibs\mingw64\bin` -> OK, OK, OK. Close and reopen
   VS Code, and check `gcc --version` again.

**Red error text when building**

Take a screenshot of the terminal and send it to whoever made the project
(it tells exactly which line is wrong).

**Ctrl+Shift+B does nothing**

Use the menu **Terminal -> Run Build Task...**. Also make sure you opened the
folder that directly contains `build.bat` (not a folder one level above it).

**"Another instance of this task is already running"**

The program from the last run is still open in the terminal. Click the little
trash-can icon in the terminal panel (or type `e`, `e` in it) and try again.

---

## Mac or Linux

1. Install a compiler: macOS -> run `xcode-select --install` in Terminal;
   Linux -> `sudo apt install build-essential`.
2. Open the project folder in VS Code (or in a terminal) and run:
   ```
   make run
   ```
   (In VS Code, `Ctrl + Shift + B` does the same thing.)
