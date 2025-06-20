# MiniGit Documentation

**Authors:** Samuel Godad and Firamit Megersa  
**Email:** godadsamuel@gmail.com  
**Project:** Custom Version Control System Implementation  

## Introduction
MiniGit is a simplified version of the Git version control system, implemented in C++. It supports basic functionalities like initializing a repository, adding files, committing changes, viewing commit history, creating branches, switching between branches, and merging branches. An optional `diff` command is also implemented to show differences between two files.

## Features
- `init`: Initializes a new MiniGit repository.
- `add <filename>`: Adds a file to the staging area.
- `commit -m "<message>"`: Records changes to the repository with a message.
- `log`: Displays the commit history.
- `branch <branch-name>`: Creates a new branch.
- `checkout <branch-name>|<commit-hash>`: Switches to a specified branch or commit.
- `merge <branch-name>`: Merges a specified branch into the current branch.
- `diff <file1> <file2>`: (Optional) Shows the differences between two files.

## How to Compile and Run
1. Navigate to the `minigit_project` directory.
2. Compile the source code using a C++17 compatible compiler (e.g., g++):
   ```bash
   g++ -std=c++17 minigit.cpp sha1.cpp -o minigit
   ```
3. Run MiniGit commands:
   ```bash
   ./minigit <command> [args]
   ```

## Implementation Details
### Repository Structure
MiniGit creates a `.minigit` directory in the project root. This directory contains:
- `objects/`: Stores file contents (blobs) and commit objects, identified by their SHA-1 hashes.
- `refs/heads/`: Stores pointers to the latest commit of each branch.
- `HEAD`: A file that points to the current branch or commit.
- `index`: A temporary file that stores information about files in the staging area.

### SHA-1 Hashing
SHA-1 hashing is used to uniquely identify file contents and commit objects. A custom SHA-1 implementation is included for this purpose.

### Commit Object Structure
Each commit object stores:
- A reference to its parent commit(s) (for merges).
- Author and committer information with timestamps.
- The commit message.
- A list of files and their corresponding SHA-1 hashes at the time of the commit.

### Branching and Merging
Branches are simply pointers to commit objects. The `checkout` command updates the `HEAD` pointer and restores the working directory to the state of the target commit/branch. The `merge` command performs a 3-way merge, identifying common ancestors and applying changes. Basic conflict detection is implemented.

### Diff
The `diff` command compares two files line by line and highlights additions (`+`), deletions (`-`), and unchanged lines (`  `).

## Limitations
- Simplified tree object model: Unlike Git, MiniGit does not implement a full tree object model. Instead, commit objects directly store file SHA-1s and filenames.
- Basic conflict resolution: Merge conflicts are simply reported, and manual resolution is required. There are no conflict markers added to files.
- No remote repositories: MiniGit operates only on local repositories.
- No `rm` or `mv` commands: Files must be manually deleted/moved from the working directory and then re-added/committed.

## Step-by-Step Demonstration

### Prerequisites
Ensure you have a C++17 compatible compiler installed (g++, clang++, etc.).

### Step 1: Compile the Project
```bash
g++ -std=c++17 minigit.cpp sha1.cpp -o minigit
```

### Step 2: Initialize a Repository
```bash
./minigit init
```
This creates a `.minigit/` directory with the following structure:
- `objects/` - Stores file contents (blobs) and commit objects
- `refs/heads/` - Branch pointers
- `HEAD` - Current branch/commit pointer
- `index` - Staging area

### Step 3: Create and Add Files
```bash
# Create test files
echo 'Hello, this is my first file!' > file1.txt
echo 'This is the second file content.' > file2.txt

# Add files to staging area
./minigit add file1.txt
./minigit add file2.txt
```

### Step 4: Make Your First Commit
```bash
./minigit commit -m 'Initial commit with two files'
```

### Step 5: View Commit History
```bash
./minigit log
```

### Step 6: Create and Switch to a New Branch
```bash
# Create a new branch
./minigit branch feature-branch

# Switch to the new branch
./minigit checkout feature-branch
```

### Step 7: Make Changes in the Feature Branch
```bash
# Create a new file
echo 'This is a new feature file!' > feature.txt

# Modify existing file
echo 'Hello, this is my UPDATED first file!' > file1.txt

# Stage and commit changes
./minigit add feature.txt
./minigit add file1.txt
./minigit commit -m 'Added feature file and updated file1'
```

### Step 8: Switch Back to Master and Make Different Changes
```bash
# Switch back to master
./minigit checkout master

# Create a master-specific file
echo 'This is a master branch addition!' > master-only.txt

# Stage and commit
./minigit add master-only.txt
./minigit commit -m 'Added master-only file'
```

### Step 9: Merge the Feature Branch
```bash
./minigit merge feature-branch
```

### Step 10: Verify the Merge
```bash
# Check the merged files
ls -la
cat file1.txt       # Should show updated content
cat feature.txt     # Should show feature content
cat master-only.txt # Should show master content

# View complete history
./minigit log
```

### Step 11: Demonstrate Diff Functionality (Bonus)
```bash
# Create two files to compare
echo 'Original content' > original.txt
echo 'Modified content with changes' > modified.txt

# Show differences
./minigit diff original.txt modified.txt
```

### Step 12: Checkout Specific Commit (Detached HEAD)
```bash
# Get a commit hash from log and checkout
./minigit checkout <commit-hash>

# Verify detached HEAD state
cat .minigit/HEAD
```

### Expected Output Examples

**After initialization:**
```
Initialized empty MiniGit repository in /path/to/your/project/.minigit
```

**After adding files:**
```
Added file1.txt (4aa180dddb23c5e3caceb608f180095b31db12d5)
Added file2.txt (3f5766d6cde842728b23cb07161cab7274ecf195)
```

**After committing:**
```
[master (root-commit) e0a5e91] Initial commit with two files
```

**After merging:**
```
Merged branch "feature-branch" into master
Merge commit: d82e250
```

**Log output:**
```
commit d82e2508dd103f6b9c406fe3103a5bacd5bee848
author Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> Fri Jun 20 10:31:58 2025
committer Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> Fri Jun 20 10:31:58 2025

    Merge branch "feature-branch"
```

**Diff output:**
```
- Original content
+ Modified content with changes
```

## Internal Architecture Overview

### Object Storage
Files are stored in `.minigit/objects/` using their SHA-1 hash as filename:
- **Blob objects**: Raw file content
- **Commit objects**: Metadata + file references

### Commit Object Format
```
tree 
parent <parent-commit-hash>
author Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> <timestamp>
committer Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> <timestamp>

<commit-message>
<file-sha1> <filename>
<file-sha1> <filename>
...
```

### Branch Management
- **HEAD**: Points to current branch (`ref: refs/heads/master`) or commit hash (detached)
- **Branch files**: Store commit hashes in `.minigit/refs/heads/<branch-name>`

### Merge Algorithm
1. Find common ancestor between branches
2. Compare files in current, merge, and ancestor commits
3. Apply 3-way merge logic:
   - File unchanged in both: keep current
   - File changed in one branch: apply change
   - File changed in both: report conflict
4. Create merge commit with dual parents

## Troubleshooting

### Compilation Issues
- Ensure C++17 support: `g++ --version`
- Check include paths for `<filesystem>`

### Runtime Issues
- Verify `.minigit/` directory exists before running commands
- Check file permissions for object storage

## Future Improvements
- Implement a proper tree object model for better efficiency and consistency.
- Enhance merge conflict resolution with automatic merging for non-conflicting changes and conflict markers for conflicting ones.
- Add support for remote repositories (push/pull).
- Implement `rm` and `mv` commands.
- Improve error handling and user feedback.


