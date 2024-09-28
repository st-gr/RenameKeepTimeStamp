# RenameKeepTimestamp
## Overview
**RenameKeepTimestamp** is a Windows utility that allows you to rename files without altering their original timestamps (creation time and last modification time). The application integrates seamlessly into the Windows Explorer context menu for quick and easy access.

## Features
- **Preserve Original Timestamps:** Renames files while keeping their original creation and modification dates intact.
- **Explorer Context Menu Integration:** Adds a convenient option to the right-click context menu in Windows Explorer.
- **Interface:** Displays a dialog with the current filename prefilled.
## Dependencies
- **Windows Operating System:** Designed for Windows 7 and above.
- **Visual Studio 2017:** Required for building the application from source.
- **CMake (>=3.13):** Used for configuring and generating the build system. Upgrade, if `cmake --version` is below 3.13
- **Windows SDK:** Includes necessary headers and libraries for Windows API functions.
## Building the Application
### Prerequisites
- **Visual Studio 2017:** Ensure it is installed on your system.
- **CMake:** Install CMake if it's not already available.
### Steps to Build
1. **Clone the Repository**

   Clone the project repository to your local machine:
   `git clone https://github.com/st-gr/RenameKeepTimestamp.git`
2. **Navigate to the Project Directory**

   `cd RenameKeepTimestamp`
   
3. **Create a Build Directory**

   It's good practice to build in a separate directory:

   ```
   mkdir build
   cd build
   ```
4. **Configure the Project with CMake**

   Run CMake to generate the Visual Studio solution:

   `cmake .. -G "Visual Studio 15 2017"`
5. **Build the Project**

   Build the application using the generated solution:

   `cmake --build . --config Release`
   
   The executable `RenameKeepTimestamp.exe` will be located in the `Release` folder inside the build directory.

## Integrating into Windows Explorer Context Menu
To add the application to the Windows Explorer context menu for easy access:

1. **Modify the Registry File**

   Create or edit the `AddRenameKeepTimestampContextMenuTemplate.reg` file with the following content:

   ```
   Windows Registry Editor Version 5.00
   
   [HKEY_CLASSES_ROOT\*\shell\RenameKeepTimestamp]
   @="My Rename keep timestamp"
   "Icon"="\"C:\\Path\\To\\Your\\Executable\\RenameKeepTimestamp.exe\""
   
   [HKEY_CLASSES_ROOT\*\shell\RenameKeepTimestamp\command]
   @="\"C:\\Path\\To\\Your\\Executable\\RenameKeepTimestamp.exe\" \"%1\""
   ```
   **Important:**

   - Replace C:\\Path\\To\\Your\\Executable\\RenameKeepTimestamp.exe with the actual path to your RenameKeepTimestamp.exe file.

   - Remember to escape backslashes with double backslashes (\\) in the registry file.
2. **Apply the Registry Changes**

   - Double-click the .reg file.
   - Confirm the changes when prompted by Windows Registry Editor.
   - To uninstall double-click the `UninstallRenameKeepTimestampContextMenu.reg` file and confirm
3. **Verify the Context Menu Entry**

   - Right-click any file in Windows Explorer.
   - You should see the option **"My Rename keep timestamp"** in the context menu.

## Usage Instructions
1. **Right-Click a File**

   - In Windows Explorer, navigate to the file you want to rename.
   - Right-click on the file to open the context menu.
2. **Select "My Rename keep timestamp"**

   - Click on the new context menu entry to launch the application.
3. **Modify the Filename**

   - A dialog box will appear with the current filename prefilled.
   - Edit the filename as desired. You can change the name, add prefixes or suffixes, etc.
4. **Confirm the Renaming**

   - Click **OK** to rename the file.
   - The file will be renamed, and its original timestamps will be preserved.

## License
MIT License, see LICENSE.txt file