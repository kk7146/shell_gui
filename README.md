# custm-shell

## cmd
custm-shell is a custom shell that supports a variety of standard Unix-like commands. It provides a simplified command-line interface where users can execute various commands including file management, process handling, and more.

## Supported Commands
The following commands are available in `custm-shell`:

- `ls`
- `chmod`
- `cat`
- `cd`
- `mkdir`
- `rmdir`
- `rename`
- `ln`
- `rm`
- `cp`
- `ps`
- `kill`
- `exec`

**Built-in Commands**:
- `help`: Displays information about available commands.
- `quit`: Exits the custom shell.

`help` and `quit` are built-in commands and are available by default in the shell.

## How to Build
The custm-shell uses a Makefile for building the project. Follow these steps to compile and install:
1.	Build the project:
```bash
make
```
or
```bash
make all
```

2.	Clean compiled files:
```bash
make fclean
```
or
```bash
make clean
```

3.	Rebuild the project from scratch:
```bash
make re
```

## Usage
Launch the custm-shell by executing the compiled program:
```bash
./custom-shell <command-list>
```
For example:
```bash
./custom-shell ls chmod cat cd mkdir rmdir rename ln rm cp ps kill exec
```
