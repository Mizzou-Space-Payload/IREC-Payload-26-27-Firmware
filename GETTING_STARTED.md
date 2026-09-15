\# IREC Payload Firmware — New Member Guide



\## Welcome!



This guide will help you get the IREC payload firmware running on your computer.

You do not need prior Git or GitHub experience to follow this guide.



\---



\## 1. What is this repository?



This repository contains the firmware for the electrical payload being developed for the Mizzou Space Program's 26-27 IREC rocket. The firmware runs on the XIAO ESP32-S3 Plus and is developed using ESP-IDF and FreeRTOS. The payload firmware currently interfaces with sensors including the BNO085 IMU, BMP390 barometric pressure sensor, and BME688 environmental sensor, along with microSD storage.



\### GitHub and Git



\*\*GitHub\*\* is where the team's source code is stored and shared.



\*\*Git\*\* is the tool we use to track changes to the code and synchronize our local copies with the GitHub repository.

You will work with a local copy of the repository on your computer. Changes are made locally, then committed and pushed to GitHub when you are ready to share them with the team.



You do not need to understand Git completely to get started. This guide will introduce the commands you need as you encounter them.



This guide assumes you are using Windows.



\## 2. Install Git



Download and install Git for Windows. Link: https://git-scm.com/install/windows



\## 3. Install ESP-IDF



This project currently uses \*\*ESP-IDF v5.5.4\*\*. New members should use this

version to ensure everyone is developing with the same ESP-IDF environment.



Download and install the \*\*ESP-IDF Installation Manager (EIM)\*\* using

Espressif's official installation instructions:



https://docs.espressif.com/projects/idf-im-ui/en/latest/



During installation:



1\. Select \*\*New Installation\*\*.

2\. Select \*\*ESP-IDF v5.5.4\*\* as the IDF version.

3\. Leave the required development tools selected.

4\. Complete the installation.



After installation, use the \*\*IDF PowerShell\*\* shortcut created by the

installer when working with ESP-IDF.



To verify the installation, open IDF PowerShell and run:



```text

idf.py --version



