# MiniGit Documentation

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

## Future Improvements
- Implement a proper tree object model for better efficiency and consistency.
- Enhance merge conflict resolution with automatic merging for non-conflicting changes and conflict markers for conflicting ones.
- Add support for remote repositories (push/pull).
- Implement `rm` and `mv` commands.
- Improve error handling and user feedback.


