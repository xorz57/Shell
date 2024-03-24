# Shell

This is a shell implemented in C as a university assignment for an operating systems course. While the assignment received a perfect score, there are areas where improvements can be made. 

## Features

- Basic shell functionalities such as executing commands, handling arguments, and managing processes.
- Pipeline implementation for command chaining.
- Debug messages for troubleshooting (optional).
- Single-file implementation due to assignment constraints.

## Areas for Improvement

- **Pipeline Implementation**: The pipeline implementation could be optimized for better performance and reliability. There were some rare cases where the pipeline got stuck, unlike the behavior of the Bash shell.
  
- **Function Modularity**: Some functions could be broken down into smaller, more modular components for better readability and maintainability.
  
- **Debug Messages**: While debug messages were provided, they could be enhanced by redirecting them to a log file for better organization and analysis.

## Usage

### Prerequisites

- C compiler (e.g., GCC)

### Compilation

```bash
gcc Shell.c -o Shell
```

### Execution

```bash
./Shell
```

## License

This project is licensed under the [MIT License](LICENSE).
