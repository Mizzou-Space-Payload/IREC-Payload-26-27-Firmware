# IREC Payload Firmware — New Member Guide

## Welcome!

This guide will help you get the IREC payload firmware running on your computer and introduce you to the workflow used to contribute to the project.

You do not need prior Git or GitHub experience to follow this guide.

This guide assumes you are using Windows.

---

## 1. What is this repository?

This repository contains the firmware for the electrical payload being developed for the Mizzou Space Program's 26-27 IREC rocket.

The firmware runs on the XIAO ESP32-S3 Plus and is developed using ESP-IDF and FreeRTOS. The payload firmware currently interfaces with sensors including the BNO085 IMU, BMP390 barometric pressure sensor, and BME688 environmental sensor, along with microSD storage.

### GitHub and Git

**GitHub** is where the team's source code is stored and shared.

**Git** is the tool we use to track changes to the code and synchronize our local copies with the GitHub repository.

You will work with a local copy of the repository on your computer. Changes are made locally, then committed and pushed to GitHub when you are ready to share them with the team.

You do not need to understand Git completely to get started. This guide will introduce the commands you need as you encounter them.

---

## 2. Install Git

Download and install Git for Windows:

https://git-scm.com/install/windows

Git Bash is included with the Git for Windows installation. You will use Git Bash for Git commands throughout this guide.

After installation, open **Git Bash** and verify that Git is installed by running:

```bash
git --version
```

You should see a Git version displayed.

---

## 3. Install ESP-IDF

This project currently uses **ESP-IDF v5.5.4**. New members should use this version to ensure everyone is developing with the same ESP-IDF environment.

Download and install the **ESP-IDF Installation Manager (EIM)** using Espressif's official installation instructions:

https://docs.espressif.com/projects/idf-im-ui/en/latest/

During installation:

1. Select **New Installation**.
2. Select **ESP-IDF v5.5.4** as the IDF version.
3. Leave the required development tools selected.
4. Complete the installation.

After installation, use the **IDF PowerShell** shortcut created by the installer when working with ESP-IDF.

To verify the installation, open IDF PowerShell and run:

```bash
idf.py --version
```

The output should indicate that you are using **ESP-IDF v5.5.4**.

---

## 4. Get Access to the Repository

You will need a GitHub account and access to the Mizzou Space Payload organization before you can contribute to the firmware.

### GitHub Account

If you do not already have a GitHub account, create one at:

https://github.com/

Send your GitHub username to the Payload Lead so you can be added to the organization and given access to the firmware repository.

The IREC Payload firmware repository is:

https://github.com/Mizzou-Space-Payload/IREC-Payload-26-27-Firmware

Once you have been given access, you can download the repository to your computer.

---

## 5. Download the Firmware

Open **Git Bash** and navigate to the folder where you want to store the project.

For example:

```bash
cd Documents
```

Then clone the repository:

```bash
git clone --recurse-submodules https://github.com/Mizzou-Space-Payload/IREC-Payload-26-27-Firmware.git
```

The `--recurse-submodules` option is important because this project uses a separate Git repository as a **Git submodule** for the BNO085 driver. This option downloads the submodule along with the main firmware repository.

After cloning, enter the project directory:

```bash
cd IREC-Payload-26-27-Firmware
```

You now have a local copy of the IREC payload firmware on your computer.

---

## 6. Open the Firmware in ESP-IDF

The repository is already set up as an ESP-IDF project. You do not need to create a new ESP-IDF project or import the firmware into another project.

### 6.1 Open IDF PowerShell

Open the **IDF PowerShell** shortcut created when ESP-IDF was installed.

Navigate to the firmware repository. For example:

```bash
cd Documents/IREC-Payload-26-27-Firmware
```

If you cloned the repository somewhere else, navigate to that location instead.

### 6.2 Set the ESP32-S3 Target

This project is designed for the **ESP32-S3**.

Run:

```bash
idf.py set-target esp32s3
```

This configures the project for the ESP32-S3.

### 6.3 Build the Firmware

Build the project by running:

```bash
idf.py build
```

The first build may take several minutes while ESP-IDF processes the project's components and dependencies.

If the build completes successfully, the firmware is ready to be flashed to an ESP32-S3.

### 6.4 Flash the Firmware

Connect the XIAO ESP32-S3 Plus to your computer using USB.

Determine which COM port the board is using, then run:

```bash
idf.py -p COMX flash
```

Replace `COMX` with the actual COM port. For example:

```bash
idf.py -p COM6 flash
```

### 6.5 Monitor the ESP32-S3

To view output from the ESP32-S3, run:

```bash
idf.py -p COMX monitor
```

For example:

```bash
idf.py -p COM6 monitor
```

You can also build, flash, and open the monitor in one command:

```bash
idf.py -p COMX flash monitor
```

To exit the serial monitor, press:

```text
Ctrl + ]
```

---

## 7. Basic Git Workflow

Git is used to keep track of changes to the firmware and share those changes with the team.

Before beginning work, make sure your local copy is up to date:

```bash
git pull
```

### 7.1 Create a Branch

For changes that you intend to contribute to the project, create a new branch before making your changes.

For example:

```bash
git switch -c feature/my-new-feature
```

Use a short name that describes what you are working on.

Examples:

```bash
git switch -c feature/telemetry
git switch -c feature/sd-logging
git switch -c fix/bno085-init
```

Your changes will be made on this branch instead of directly on `main`.

### 7.2 Make and Test Your Changes

Make your changes to the firmware and test them.

Build the project to make sure it still compiles:

```bash
idf.py build
```

### 7.3 Check Your Changes

In Git Bash, run:

```bash
git status
```

This shows which files have been modified.

You can also view the specific changes with:

```bash
git diff
```

### 7.4 Stage Your Changes

When you are ready to save your changes in Git, run:

```bash
git add .
```

This stages the changes for your next commit.

### 7.5 Commit Your Changes

Create a commit with a short description of what you changed:

```bash
git commit -m "Add BNO085 initialization handling"
```

Try to make commit messages describe the change rather than simply saying something like `updated code`.

Examples:

```bash
git commit -m "Add temperature sensor driver"
git commit -m "Fix SD card initialization"
git commit -m "Update payload data structure"
```

### 7.6 Push Your Branch

Send your branch to the team's GitHub repository:

```bash
git push -u origin feature/my-new-feature
```

Replace `feature/my-new-feature` with the name of your branch.

After the first push, you can generally use:

```bash
git push
```

---

## 8. Create a Pull Request

After pushing your branch, go to the IREC Payload firmware repository on GitHub.

Create a **Pull Request (PR)** from your branch into `main`.

A Pull Request allows other team members to review your changes before they are added to the official `main` branch.

When creating a Pull Request:

* Give it a clear title describing your changes.
* Briefly describe what you changed.
* Mention anything that needs to be tested or reviewed.

Do not merge your own Pull Request unless the team has established that you are allowed to do so.

Once the changes have been reviewed and approved, the Pull Request can be merged into `main`.

---

## 9. The Basic Team Workflow

The typical workflow for contributing to the firmware is:

```text
Update your local repository
        ↓
Create a branch
        ↓
Make and test your changes
        ↓
Check your changes
        ↓
Commit your changes
        ↓
Push your branch
        ↓
Create a Pull Request
        ↓
Review
        ↓
Merge into main
```

In commands, this will generally look like:

```bash
git pull

git switch -c feature/my-new-feature

# Make and test your changes

git status
git add .
git commit -m "Describe your changes"
git push -u origin feature/my-new-feature
```

Then create a Pull Request on GitHub.

The `main` branch should represent the current official version of the payload firmware. Use branches and Pull Requests when contributing changes so that changes can be reviewed before being added to `main`.
