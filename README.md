# Mini-Shell
This project was developed for the Operating Systems course. It simulates a basic command-line interface, offering essential shell functionalities such as command execution, file input handling, input/output redirection, command piping, background process management, and signal handling.

## Installation
```bash
git clone https://github.com/loordoza/Mini-Shell.git
cd Mini-Shell/shell
make
```

## Usage

```bash
cd ../bin
./mshell
```

Once the shell is running, you can type commands directly into the prompt.

## Features

The Mini Shell offers a range of functionalities to enhance user experience:

- **File Input**: Execute commands from a script file.
- **Built-in Shell Commands**: Supports basic shell commands such as `lcd`, `lexit`, and others.
- **Input/Output Redirection**: Redirect input from a file or output to a file using `>` and `<` operators.
- **Command Piping**: Chain commands together using pipes (`|`) to pass the output of one command as input to another.
- **Background Processes**: Execute commands in the background using `&`, allowing the shell to continue running while processes execute.
- **Signal Handling**: Properly handle signals like `SIGINT` (Ctrl+C) to terminate processes gracefully.

