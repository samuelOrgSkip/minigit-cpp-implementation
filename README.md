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

## Demonstration Walkthrough

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

## Project Structure
```
minigit_project/
├── README.md              # This documentation
├── minigit.cpp           # Main VCS implementation (858 lines)
├── sha1.cpp              # Custom SHA-1 algorithm (136 lines)  
├── sha1.h                # SHA-1 header definitions
├── minigit               # Compiled executable
├── .gitignore            # Project ignore patterns
└── demo_workspace/       # Testing workspace (ignored)
```

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


