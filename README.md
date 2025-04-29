# TrueVote / EVote : Digital Voting System

eVote is a secure and efficient electronic voting system designed to ensure transparency and integrity in the voting process. It leverages modern cryptographic techniques to protect voter privacy and prevent tampering.

[See our presentation ↗️](https://www.canva.com/design/DAGmAXYr0aM/lYysgOLE0z9kdSsaf3eBKQ/view?utm_content=DAGmAXYr0aM&utm_campaign=designshare&utm_medium=link2&utm_source=uniquelinks&utlId=h3586e21761)

## Features
- Secure and anonymous voting.
- Easy-to-use interface for voters and administrators.
- Tamper-proof vote storage and auditing.

## Requirements

To build and run eVote, ensure the following prerequisites are met:

1. **Operating System**: A Linux-based system or Windows Subsystem for Linux (WSL).
2. **Qt Framework**: Install the Qt development framework. You can download it from [Qt's official website](https://www.qt.io/download) or use your package manager:
    ```bash
    sudo apt-get install qt5-default
    ```
3. **Build Tools**: Ensure `build-essential` is installed, which includes `gcc`, `g++`, `make`, and other necessary tools:
    ```bash
    sudo apt-get install build-essential
    ```
4. **Git**: Install Git to clone the repository if not already installed:
    ```bash
    sudo apt-get install git
    ```

## Basic Setup Steps

1. Clone the repository:
    ```bash
    git clone https://github.com/your-repo/evote.git
    cd evote
    ```

2. Install any additional dependencies specified in the project documentation.

3. Follow the build and run instructions provided below.

## How to Run the Built Executable

 Run the executable:
    ```bash
    ./build/evote
    ```
 Follow the on-screen instructions to set up and start the voting process.

## How to Rebuild the Project

1. Navigate to the build directory:
    ```bash
    cd ./build
    ```

2. Clean any previous builds:
    ```bash
    make clean
    ```

3. Build the project:
    ```bash
    make
    ```

4. The new executable will be available in the `build` directory.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.