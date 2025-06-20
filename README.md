# MiniGit: A Custom Version Control System

**Authors:** Samuel Godad and Firamit Megersa  
**Email:** godadsamuel@gmail.com  
**Project:** C++ Implementation of Git-like Version Control System  

## Overview

We have implemented MiniGit, a simplified but fully functional version control system inspired by Git. Our implementation demonstrates core VCS concepts including content-addressable storage, branching, merging, and commit history tracking. The project features a custom SHA-1 implementation and a sophisticated 3-way merge algorithm.

## Features Implemented

Our MiniGit supports the following commands:

### Core Repository Operations
- `init` - Initialize a new MiniGit repository
- `add <filename>` - Stage files for commit
- `commit -m "<message>"` - Record changes with commit message
- `log` - Display complete commit history

### Branch Management
- `branch` - List all branches (marks current branch with *)
- `branch <name>` - Create a new branch
- `checkout <branch|commit>` - Switch between branches or checkout specific commits
- `status` - Show current branch and repository status

### Advanced Operations  
- `merge <branch>` - Perform 3-way merge with conflict detection
- `diff <file1> <file2>` - Compare files line by line

## Technical Architecture

### Repository Structure
Our implementation creates a `.minigit` directory containing:
- `objects/` - Content-addressable storage for blobs and commits using SHA-1 hashes
- `refs/heads/` - Branch pointers to latest commits
- `HEAD` - Current branch reference or detached commit hash
- `index` - Staging area for tracking files to be committed

### Custom SHA-1 Implementation
We implemented SHA-1 hashing from scratch (in `sha1.cpp`) for:
- Unique content identification
- Commit object integrity
- File change detection
- Object addressing in storage

### Commit Object Format
Our commit objects store:
```
tree <tree-placeholder>
parent <parent-commit-hash>
author Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> <timestamp>
committer Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> <timestamp>

<commit-message>
<file-sha1> <filename>
<file-sha1> <filename>
...
```

### 3-Way Merge Algorithm
Our merge implementation:
1. Finds common ancestor between branches using commit history traversal
2. Compares files across current, merge, and ancestor commits
3. Applies intelligent merge logic:
   - Files unchanged in both branches: keep current
   - Files changed in one branch only: apply the change
   - Files changed differently in both: detect and report conflict
4. Creates merge commit with dual parent references

## Compilation and Usage

### Building the Project
```bash
g++ -std=c++17 minigit.cpp sha1.cpp -o minigit
```

### Command Examples
```bash
# Initialize repository
./minigit init

# Stage and commit files
./minigit add file.txt
./minigit commit -m "Initial commit"

# Branch operations
./minigit branch                    # List branches
./minigit branch feature-xyz        # Create branch
./minigit checkout feature-xyz      # Switch to branch
./minigit status                    # Check current status

# Merge operations
./minigit merge feature-xyz

# View history and differences
./minigit log
./minigit diff file1.txt file2.txt
```

## Complete Demonstration Walkthrough

This section provides our comprehensive demonstration of MiniGit functionality:

### Prerequisites
```bash
# Compile the project first
g++ -std=c++17 minigit.cpp sha1.cpp -o minigit

# Create clean demo workspace
mkdir demo_workspace && cd demo_workspace
```

### Complete MiniGit Workflow Demonstration

#### **Step 1: Initialize Repository**
```bash
../minigit init
```
**Expected Output:**
```
Initialized empty MiniGit repository in /path/to/demo_workspace/.minigit
```
**Result:** Creates `.minigit` directory structure with `objects/`, `refs/heads/`, `HEAD`, and `index`

#### **Step 2: Create Initial Files**
```bash
echo 'Welcome to MiniGit!' > README.txt
echo 'print("Hello, World!")' > hello.py
echo 'function greet() { console.log("Hi!"); }' > app.js
```
**Result:** Three different file types created

#### **Step 3: Stage Files and Check Status**
```bash
../minigit status
../minigit add README.txt
../minigit add hello.py
../minigit status
```
**Expected Output:**
```
On branch master
nothing to commit, working tree clean

Added README.txt (a1b2c3d4e5f6...)
Added hello.py (f6e5d4c3b2a1...)

On branch master
Changes to be committed:
  new file:   README.txt
  new file:   hello.py
```

#### **Step 4: First Commit**
```bash
../minigit commit -m "Initial commit with README and Python script"
../minigit status
../minigit log
```
**Expected Output:**
```
[master (root-commit) abc1234] Initial commit with README and Python script

On branch master
nothing to commit, working tree clean

commit abc1234567890abcdef1234567890abcdef12
author Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> Fri Jun 20 15:30:45 2025
committer Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> Fri Jun 20 15:30:45 2025

    Initial commit with README and Python script
```

#### **Step 5: Branch Operations**
```bash
../minigit branch
../minigit branch feature-javascript
../minigit branch
../minigit checkout feature-javascript
../minigit status
```
**Expected Output:**
```
* master

Branch "feature-javascript" created at abc1234

* master
  feature-javascript

Switched to feature-javascript

On branch feature-javascript
nothing to commit, working tree clean
```

#### **Step 6: Feature Development**
```bash
../minigit add app.js
echo 'const version = "1.0.0";' >> app.js
echo 'greet();' >> app.js
../minigit add app.js
../minigit commit -m "Add JavaScript application with greeting function"
../minigit log
```
**Expected Output:**
```
Added app.js (def5678901234...)
Added app.js (updated hash...)
[feature-javascript (root-commit) def5678] Add JavaScript application with greeting function
```

#### **Step 7: Switch Back to Master and Make Parallel Changes**
```bash
../minigit checkout master
cat app.js  # Should not exist
../minigit add README.txt
echo 'Version: 1.0' >> README.txt
../minigit add README.txt
../minigit commit -m "Update README with version information"
```
**Expected Output:**
```
Switched to master
cat: app.js: No such file or directory
Added README.txt (updated hash...)
[master (root-commit) ghi9012] Update README with version information
```

#### **Step 8: Demonstrate Branch Differences**
```bash
../minigit branch
ls -la
../minigit checkout feature-javascript
ls -la
cat app.js
../minigit checkout master
cat README.txt
```
**Result:** Demonstrates how different files exist in different branches

#### **Step 9: Successful Merge (Non-conflicting)**
```bash
../minigit merge feature-javascript
ls -la
cat README.txt
cat app.js
../minigit log
```
**Expected Output:**
```
Merged branch "feature-javascript" into master
Merge commit: jkl3456

# Shows both files now exist
# Shows complete commit history including merge commit
```

#### **Step 10: Advanced Features Demo**
```bash
# Show diff functionality
echo 'Original content' > file1.txt
echo 'Modified content with changes' > file2.txt
../minigit diff file1.txt file2.txt

# Show detached HEAD
../minigit checkout abc1234  # Use actual commit hash from earlier
../minigit status
../minigit checkout master
```
**Expected Output:**
```
- Original content
+ Modified content with changes

HEAD detached at abc1234
nothing to commit, working tree clean

Switched to master
```

#### **Step 11: Conflict Demonstration**
```bash
# Create scenario for conflict
../minigit branch conflict-test
../minigit checkout conflict-test
echo 'Modified in conflict branch' > README.txt
../minigit add README.txt
../minigit commit -m "Modify README in conflict branch"

../minigit checkout master
echo 'Modified in master branch' > README.txt
../minigit add README.txt
../minigit commit -m "Modify README in master branch"

../minigit merge conflict-test
```
**Expected Output:**
```
Conflict in file: README.txt
Merge failed due to conflicts. Please resolve them manually.
```

#### **Step 12: Final Status and Cleanup**
```bash
../minigit status
../minigit branch
../minigit log --oneline  # If implemented, or just log
```

### Key Features Demonstrated

Our walkthrough showcases the following technical achievements:

1. **Repository Structure**: Proper `.minigit` directory initialization
2. **Content-Addressable Storage**: SHA-1 hashing and object storage
3. **Staging Area**: Index file management and file tracking
4. **Branch Management**: Branch creation, listing, and switching
5. **Commit History**: Linked commit objects with parent relationships
6. **3-Way Merge Algorithm**: Intelligent conflict detection and resolution
7. **Status Reporting**: Current branch and working tree state

### Demonstration Scenarios
- **Complete workflow**: All 12 steps demonstrating full functionality
- **Core features**: Steps 1-9 showing essential VCS operations
- **Quick overview**: Steps 1-4 and 8-9 for basic understanding

### Demonstration Highlights

We have tested our implementation with comprehensive scenarios:

### Basic Workflow
1. **Repository Creation**: Initialize new repository with proper directory structure
2. **File Management**: Add multiple files to staging area with SHA-1 tracking
3. **Commit Process**: Create commits with metadata and file snapshots
4. **History Tracking**: Maintain linked commit history with parent relationships

### Branching and Merging
1. **Branch Creation**: Create feature branches from any commit point
2. **Parallel Development**: Make independent changes on different branches
3. **Clean Merges**: Successfully merge non-conflicting changes from different files
4. **Conflict Detection**: Identify and report conflicts when same files modified differently

### Advanced Scenarios Tested
- **Non-conflicting merges**: Different files modified in parallel branches
- **Conflict scenarios**: Same files modified with different content
- **Detached HEAD**: Checkout specific commits directly
- **Branch listing**: Visual indication of current branch
- **Status reporting**: Clear indication of staged files and clean working tree

## Implementation Highlights

### Object Storage System
- Content-addressable storage using SHA-1 hashes ensures data integrity
- Efficient blob storage prevents duplication of identical file content
- Commit objects maintain complete project snapshots

### Branch Management
- Lightweight branches as simple pointers to commits
- HEAD pointer system supports both branch references and detached states
- Branch listing with clear current branch indication

### Merge Algorithm Sophistication
- True 3-way merge comparing ancestor, current, and merge commits
- Conservative conflict detection prioritizes data safety
- Proper merge commit creation with dual parent tracking

### Custom Cryptographic Implementation
- Full SHA-1 algorithm implemented from RFC specifications
- Proper bit manipulation and message padding
- Consistent hash generation across different content types

## Design Decisions

### Conservative Merge Strategy
We chose a conservative approach to merging - when the same file is modified in both branches (even if changes don't technically conflict), we report it as a conflict. This prioritizes data safety over automation.

### Simplified Tree Model
Unlike Git's complex tree objects, we store file references directly in commit objects. This simplifies implementation while maintaining core functionality.

### Custom Hash Implementation
Rather than using external libraries, we implemented SHA-1 ourselves to demonstrate understanding of cryptographic hashing and its role in version control.

## Project Structure and File Descriptions

### Core Implementation Files

#### **minigit.cpp**
The main implementation file containing all version control functionality:

- **Repository Management**: `init()` function creates `.minigit` directory structure
- **File Operations**: `add()` function handles staging with SHA-1 hashing and object storage
- **Commit System**: `commit()` function creates commit objects with metadata and parent linking
- **Branch Operations**: `branch()` and `list_branches()` functions manage branch creation and listing
- **Navigation**: `checkout()` function switches branches and restores working directory state
- **History**: `log()` function traverses commit history and displays formatted output
- **Merging**: `merge()` function implements 3-way merge algorithm with conflict detection
- **Status Reporting**: `status()` function shows current branch and staging area state
- **Utilities**: `diff()` function for file comparison, plus helper functions for commit parsing

**Key Features Implemented:**
- Content-addressable object storage using SHA-1 hashes
- Commit object creation with proper parent relationships
- Branch pointer management in `.minigit/refs/heads/`
- HEAD pointer system supporting both branch refs and detached commits
- Index file management for staging area
- 3-way merge algorithm with common ancestor detection

#### **sha1.cpp**
Our complete implementation of the SHA-1 cryptographic hash algorithm:

- **Core Algorithm**: `hash_bs()` function implements the full SHA-1 specification
- **Message Padding**: Proper bit padding and length encoding as per RFC 3174
- **Hash Computation**: 80-round compression function with left rotation operations
- **Block Processing**: Handles 512-bit message blocks with proper word expansion
- **Binary Operations**: Implements all SHA-1 logical functions (Ch, Parity, Maj)
- **Memory Management**: Safe allocation and deallocation of hash results
- **String Interface**: `hash()` function provides convenient string-to-hash conversion
- **Hex Conversion**: `sig2hex()` transforms binary hash to readable hexadecimal

**Technical Implementation:**
- Full 160-bit hash state management (h0-h4 variables)
- Proper message scheduling with 80-word expansion
- All four SHA-1 rounds with correct constants (0x5A827999, 0x6ED9EBA1, etc.)
- Big-endian byte ordering compliance
- Efficient bit rotation using `leftRotate32bits()` helper

#### **sha1.h**
Header file defining the SHA-1 namespace and function interfaces:

- **Namespace Organization**: Clean `hashing::sha1` namespace encapsulation
- **Function Declarations**: All public SHA-1 algorithm interfaces
- **Type Safety**: Proper parameter types and const-correctness
- **Documentation**: Clear function purpose descriptions
- **Memory Interface**: Void pointer handling for binary hash data

**Public Interface:**
- `hash()` - Main string hashing function
- `hash_bs()` - Binary data hashing function  
- `sig2hex()` - Hash-to-hex string conversion
- `leftRotate32bits()` - Bit rotation utility

### Project File Structure
```
minigit_project/
├── README.md              # This comprehensive documentation
├── minigit.cpp           # Main VCS implementation
├── sha1.cpp              # Custom SHA-1 algorithm
├── sha1.h                # SHA-1 header definitions
├── minigit               # Compiled executable
├── .gitignore            # Project ignore patterns
└── demo_workspace/       # Testing workspace (ignored)
```

### Code Organization Philosophy

Our implementation follows a clear separation of concerns:

1. **Cryptographic Layer** (`sha1.h` + `sha1.cpp`): Provides secure, RFC-compliant hashing
2. **Version Control Layer** (`minigit.cpp`): Implements all VCS logic using the crypto layer
3. **Interface Layer** (`main()` in minigit.cpp): Command-line parsing and user interaction

This modular design allows for:
- **Independent testing** of SHA-1 implementation
- **Clear responsibility boundaries** between components  
- **Easy maintenance** and potential feature additions
- **Academic clarity** in understanding each component's role

## Testing and Validation

We have thoroughly tested our implementation with:
- Multiple branching scenarios
- Complex merge operations  
- File modification patterns
- Edge cases (empty repos, detached HEAD, etc.)
- Large file handling
- Concurrent development workflows

## Limitations and Future Work

### Current Limitations
- **Simplified tree model**: Direct file storage in commits rather than tree objects
- **Basic conflict resolution**: Reports conflicts without inline markers
- **Local repositories only**: No remote repository support
- **Limited file operations**: No built-in file removal or renaming

### Potential Enhancements
- Implement proper tree object model for scalability
- Add automatic conflict resolution for non-overlapping changes
- Support for remote repositories (push/pull operations)
- File removal and renaming commands
- Performance optimizations for large repositories

## Academic Significance

This project demonstrates mastery of:
- **Data structures**: Graph algorithms for commit history and merge operations
- **Cryptography**: Custom hash function implementation
- **File systems**: Content-addressable storage design
- **Software engineering**: Modular architecture and error handling
- **Algorithm design**: 3-way merge and conflict detection logic

Our implementation successfully recreates core Git functionality while maintaining code clarity and educational value. The project showcases understanding of distributed version control principles and practical C++ programming skills.

## Conclusion

MiniGit represents a complete, working version control system that captures the essential concepts of modern VCS tools. Through careful design and implementation, we have created a tool that not only functions correctly but also serves as an educational demonstration of version control internals.

The project successfully bridges theoretical computer science concepts with practical software development, resulting in a robust and extensible foundation for version control operations.


