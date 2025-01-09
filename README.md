# Enhanced xv6 with Compression and Text Editor

This enhanced version of xv6 includes Huffman compression and a text editor, built for the RISC-V architecture.

## New Features

1. **Huffman Compression System**
   - Automatic file compression
   - Transparent read/write of compressed files
   - Smart compression (only when beneficial)

2. **Text Editor (edit)**
   - Create and edit text files
   - Basic text editing capabilities
   - Command-based interface

3. **Testing Utilities**
   - comprtest: Test file compression
   - mkfile: Create test files
   - testcomp: Run compression tests

## Installation Instructions

### For Linux:
```bash
# Install required packages
sudo apt update
sudo apt install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

# Clone the repository
git clone https://github.com/abhinavlg/xv6-riscv-compressfs.git
cd xv6-riscv

# Build
make clean
make

# Run
make qemu
```

### For macOS:
```bash
# Install Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required packages
brew tap riscv-software-src/riscv
brew install riscv-tools qemu

# Clone the repository
git clone https://github.com/abhinavlg/xv6-riscv-compressfs.git
cd xv6-riscv

# Build
make clean
make

# Run
make qemu
```

### For Windows:
```bash
# 1. Install WSL2 (Windows Subsystem for Linux)
# 2. Install Ubuntu from Microsoft Store
# 3. Open Ubuntu terminal and run:

sudo apt update
sudo apt install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

# Clone the repository
git clone <repository-url>
cd xv6-riscv

# Build
make clean
make

# Run
make qemu
```

## Usage Guide

### Text Editor
```bash
# Start editor
edit filename.txt

# Commands:
# $ - Save file
# @ - Exit editor
# Backspace - Delete character
```

### File Operations
```bash
# Create a test file
mkfile test.txt "This is test content"

# View file
cat test.txt

# Test compression
comprtest test.txt

# Run all compression tests
testcomp
```

### Example Workflow
1. Create and edit a file:
```bash
$ edit myfile.txt
# Type your content
# Press $ to save
# Press @ to exit
```

2. Test compression:
```bash
$ comprtest myfile.txt
```

3. View file:
```bash
$ cat myfile.txt
```

## Troubleshooting

### Common Issues:

1. **Build Failures**
   - Ensure all dependencies are installed
   - Try `make clean` before rebuilding
   - Check RISC-V toolchain installation

2. **QEMU Issues**
   - Verify QEMU installation
   - Check if QEMU supports RISC-V
   - Try reinstalling QEMU

3. **Editor Issues**
   - Maximum file size is 1024 bytes
   - Save before exiting
   - Use simple ASCII text

### Getting Help
- Check error messages
- Verify installation steps
- Ensure correct permissions

## System Requirements

- 64-bit operating system
- At least 2GB RAM
- 1GB free disk space
- Internet connection for installation

## License

Same as original xv6 - see LICENSE file.

