#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <sstream>
#include "sha1.h"

namespace fs = std::filesystem;

// Function to compute SHA-1 hash of a file
std::string calculate_file_sha1(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return "";
    }

    // Read file content into a vector of chars
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Convert vector<char> to string for hashing
    std::string file_content(buffer.begin(), buffer.end());

    void* hash_result = hashing::sha1::hash(file_content);
    std::string sha1_hex = hashing::sha1::sig2hex(hash_result);
    delete[] static_cast<uint8_t*>(hash_result); // Free the allocated memory

    return sha1_hex;
}

// Helper function to get files from a commit
std::map<std::string, std::string> get_files_from_commit(const std::string& commit_hash)
{
    std::map<std::string, std::string> files;
    std::string commit_content_path = ".minigit/objects/" + commit_hash;
    std::ifstream commit_file(commit_content_path);
    std::string line;

    bool in_files_section = false;
    while (std::getline(commit_file, line))
    {
        if (line.rfind("tree ", 0) == 0 || line.rfind("parent ", 0) == 0 || line.rfind("author ", 0) == 0 || line.rfind("committer ", 0) == 0)
        {
            continue;
        }
        if (line.empty())
        {
            in_files_section = true;
            continue;
        }
        if (in_files_section)
        {
            size_t first_space = line.find(" ");
            if (first_space != std::string::npos)
            {
                std::string file_sha1 = line.substr(0, first_space);
                std::string filename = line.substr(first_space + 1);
                files[filename] = file_sha1;
            }
        }
    }
    commit_file.close();
    return files;
}

// Helper function to find common ancestor
std::string find_common_ancestor(const std::string& commit1_hash, const std::string& commit2_hash)
{
    std::vector<std::string> path1;
    std::string current_commit = commit1_hash;
    while (!current_commit.empty())
    {
        path1.push_back(current_commit);
        std::string commit_path = ".minigit/objects/" + current_commit;
        std::ifstream commit_file(commit_path);
        std::string line;
        std::string parent_line;
        while (std::getline(commit_file, line))
        {
            if (line.rfind("parent ", 0) == 0)
            {
                parent_line = line;
                break;
            }
        }
        commit_file.close();
        if (!parent_line.empty())
        {
            current_commit = parent_line.substr(7);
        }
        else
        {
            current_commit = "";
        }
    }

    std::string common_ancestor = "";
    current_commit = commit2_hash;
    while (!current_commit.empty())
    {
        for (const std::string& c : path1)
        {
            if (c == current_commit)
            {
                common_ancestor = c;
                return common_ancestor;
            }
        }
        std::string commit_path = ".minigit/objects/" + current_commit;
        std::ifstream commit_file(commit_path);
        std::string line;
        std::string parent_line;
        while (std::getline(commit_file, line))
        {
            if (line.rfind("parent ", 0) == 0)
            {
                parent_line = line;
                break;
            }
        }
        commit_file.close();
        if (!parent_line.empty())
        {
            current_commit = "";
        }
    }
    return common_ancestor;
}

// Helper function to read file content into a string
std::string read_file_content(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Initializes a new MiniGit repository.
void init()
{
    // Create .minigit directory if it doesn't exist
    if (!fs::exists(".minigit"))
    {
        fs::create_directory(".minigit");
        std::cout << "Initialized empty MiniGit repository in " << fs::current_path() / ".minigit" << std::endl;
    }
    else
    {
        std::cout << "MiniGit repository already initialized in " << fs::current_path() / ".minigit" << std::endl;
    }

    // Create necessary subdirectories and files
    if (!fs::exists(".minigit/objects"))
        fs::create_directory(".minigit/objects");
    if (!fs::exists(".minigit/refs"))
        fs::create_directory(".minigit/refs");
    if (!fs::exists(".minigit/refs/heads")) // Ensure this is created before HEAD is written
        fs::create_directory(".minigit/refs/heads");
    if (!fs::exists(".minigit/HEAD"))
    {
        std::ofstream head_file(".minigit/HEAD");
        head_file << "ref: refs/heads/master\n";
        head_file.close();
    }
    if (!fs::exists(".minigit/index"))
    {
        std::ofstream index_file(".minigit/index");
        index_file.close();
    }
}

// Adds a file to the staging area.
void add(const std::string& filename)
{
    if (!fs::exists(filename))
    {
        std::cerr << "Error: file not found " << filename << std::endl;
        return;
    }

    std::string file_sha1 = calculate_file_sha1(filename);
    if (file_sha1.empty())
    {
        std::cerr << "Error: Could not calculate SHA-1 for " << filename << std::endl;
        return;
    }

    // Store blob in .minigit/objects/
    std::string object_path = ".minigit/objects/" + file_sha1;
    std::ifstream src(filename, std::ios::binary);
    std::ofstream dst(object_path, std::ios::binary);
    dst << src.rdbuf();

    std::cout << "Added " << filename << " (" << file_sha1 << ")" << std::endl;

    // Update index file
    std::ofstream index_file(".minigit/index", std::ios::app);
    index_file << file_sha1 << " " << filename << "\n";
    index_file.close();
}

// Records changes to the repository with a message.
void commit(const std::string& message)
{
    // Read index file to get staged files
    std::ifstream index_file(".minigit/index");
    std::string line;
    std::string commit_content = "tree "; // Placeholder for tree hash
    std::string parent_commit_hash = "";

    // Get parent commit hash from HEAD
    std::ifstream head_file_read(".minigit/HEAD");
    std::string head_ref;
    std::getline(head_file_read, head_ref);
    head_file_read.close();

    std::string current_branch_name = "";
    if (head_ref.rfind("ref: refs/heads/", 0) == 0) // If HEAD points to a ref (branch)
    {
        current_branch_name = head_ref.substr(16);
        std::string ref_path = ".minigit/" + head_ref.substr(5);
        if (fs::exists(ref_path))
        {
            std::ifstream ref_file(ref_path);
            std::getline(ref_file, parent_commit_hash);
            ref_file.close();
        }
    }
    else // If HEAD points directly to a commit (detached HEAD)
    {
        parent_commit_hash = head_ref;
    }

    if (!parent_commit_hash.empty())
    {
        commit_content += "\nparent " + parent_commit_hash;
    }

    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&current_time), "%c");
    commit_content += "\nauthor Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> " + ss.str();
    commit_content += "\ncommitter Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> " + ss.str();
    commit_content += "\n\n" + message + "\n";

    // Add staged files to commit content (simplified - in a real Git, this would involve creating a tree object)
    while (std::getline(index_file, line))
    {
        commit_content += line + "\n";
    }
    index_file.close();

    // Calculate commit hash
    void* commit_hash_result = hashing::sha1::hash(commit_content);
    std::string commit_sha1 = hashing::sha1::sig2hex(commit_hash_result);
    delete[] static_cast<uint8_t*>(commit_hash_result);

    // Save commit object
    std::ofstream commit_file(".minigit/objects/" + commit_sha1);
    commit_file << commit_content;
    commit_file.close();

    // Update HEAD and branch pointer
    std::ofstream head_update_file(".minigit/HEAD");
    if (!current_branch_name.empty())
    {
        head_update_file << "ref: refs/heads/" << current_branch_name << "\n";
        std::ofstream branch_update_file(".minigit/refs/heads/" + current_branch_name);
        branch_update_file << commit_sha1 << "\n";
        branch_update_file.close();
    }
    else
    {
        // If HEAD was detached or initial commit, set it to master branch
        head_update_file << "ref: refs/heads/master\n";
        std::ofstream master_branch_file(".minigit/refs/heads/master");
        master_branch_file << commit_sha1 << "\n";
        master_branch_file.close();
    }
    head_update_file.close();

    // Clear index after commit
    std::ofstream clear_index(".minigit/index", std::ios::trunc);
    clear_index.close();

    std::cout << "[" << (current_branch_name.empty() ? "master" : current_branch_name) << " (root-commit) " << commit_sha1.substr(0, 7) << "] " << message << std::endl;
}

// Displays the commit history.
void log()
{
    std::string current_commit_hash;
    std::ifstream head_file(".minigit/HEAD");
    std::getline(head_file, current_commit_hash);
    head_file.close();

    if (current_commit_hash.rfind("ref: refs/heads/", 0) == 0)
    {
        std::string ref_path = ".minigit/" + current_commit_hash.substr(5);
        std::ifstream ref_file(ref_path);
        std::getline(ref_file, current_commit_hash);
        ref_file.close();
    }

    while (!current_commit_hash.empty())
    {
        std::string commit_path = ".minigit/objects/" + current_commit_hash;
        std::ifstream commit_file(commit_path);
        std::string line;
        std::string message;
        std::string author;
        std::string committer;
        std::string parent_line;

        std::cout << "commit " << current_commit_hash << std::endl;

        while (std::getline(commit_file, line))
        {
            if (line.rfind("author ", 0) == 0)
            {
                author = line;
            }
            else if (line.rfind("committer ", 0) == 0)
            {
                committer = line;
            }
            else if (line.rfind("parent ", 0) == 0)
            {
                parent_line = line;
            }
            else if (line.empty())
            {
                // The empty line separates headers from the commit message
                std::getline(commit_file, message);
                break;
            }
        }
        commit_file.close();

        std::cout << author << std::endl;
        std::cout << committer << std::endl;
        std::cout << "\n    " << message << std::endl;
        std::cout << std::endl;

        if (!parent_line.empty())
        {
            current_commit_hash = parent_line.substr(7); // "parent " is 7 chars
        }
        else
        {
            current_commit_hash = ""; // No more parents, end of history
        }
    }
}

// Creates a new branch.
void branch(const std::string& branch_name)
{
    std::string head_commit_hash;
    std::ifstream head_file(".minigit/HEAD");
    std::getline(head_file, head_commit_hash);
    head_file.close();

    if (head_commit_hash.rfind("ref: ", 0) == 0)
    {
        // If HEAD is a symbolic ref, resolve it to the actual commit hash
        std::string ref_path = ".minigit/" + head_commit_hash.substr(5);
        std::ifstream ref_file(ref_path);
        std::getline(ref_file, head_commit_hash);
        ref_file.close();
    }

    std::string branch_path = ".minigit/refs/heads/" + branch_name;
    if (fs::exists(branch_path))
    {
        std::cerr << "Error: A branch named \"" << branch_name << "\" already exists." << std::endl;
        return;
    }

    std::ofstream new_branch_file(branch_path);
    new_branch_file << head_commit_hash << "\n";
    new_branch_file.close();

    std::cout << "Branch \"" << branch_name << "\" created at " << head_commit_hash.substr(0, 7) << std::endl;
}

// Switches to a specified branch or commit.
void checkout(const std::string& target)
{
    std::string commit_hash_to_checkout;
    std::string head_content;

    // Check if target is a branch name
    std::string branch_path = ".minigit/refs/heads/" + target;
    if (fs::exists(branch_path))
    {
        std::ifstream branch_file(branch_path);
        std::getline(branch_file, commit_hash_to_checkout);
        branch_file.close();
        head_content = "ref: refs/heads/" + target;
    }
    else // Assume target is a commit hash
    {
        std::string commit_path = ".minigit/objects/" + target;
        if (!fs::exists(commit_path))
        {
            std::cerr << "Error: Branch or commit \"" << target << "\" not found." << std::endl;
            return;
        }
        commit_hash_to_checkout = target;
        head_content = target;
    }

    // Update HEAD
    std::ofstream head_file(".minigit/HEAD");
    head_file << head_content << "\n";
    head_file.close();

    // Read commit content to restore files (simplified - in a real Git, this would involve reading tree objects)
    std::string commit_content_path = ".minigit/objects/" + commit_hash_to_checkout;
    std::ifstream commit_file(commit_content_path);
    std::string line;
    std::vector<std::pair<std::string, std::string>> files_in_commit;

    bool in_files_section = false;
    while (std::getline(commit_file, line))
    {
        if (line.rfind("tree ", 0) == 0 || line.rfind("parent ", 0) == 0 || line.rfind("author ", 0) == 0 || line.rfind("committer ", 0) == 0)
        {
            continue;
        }
        if (line.empty())
        {
            in_files_section = true;
            continue;
        }
        if (in_files_section)
        {
            size_t first_space = line.find(" ");
            if (first_space != std::string::npos)
            {
                std::string file_sha1 = line.substr(0, first_space);
                std::string filename = line.substr(first_space + 1);
                files_in_commit.push_back({file_sha1, filename});
            }
        }
    }
    commit_file.close();

    // Clear working directory (except .minigit)
    for (const auto& entry : fs::directory_iterator(fs::current_path()))
    {
        if (entry.path().filename() != ".minigit")
        {
            fs::remove_all(entry.path());
        }
    }

    // Restore files from commit
    for (const auto& file_entry : files_in_commit)
    {
        std::string blob_path = ".minigit/objects/" + file_entry.first;
        std::ifstream src(blob_path, std::ios::binary);
        std::ofstream dst(file_entry.second, std::ios::binary);
        dst << src.rdbuf();
    }

    std::cout << "Switched to " << target << std::endl;
}

// Merges a specified branch into the current branch.
void merge(const std::string& branch_to_merge)
{
    std::string current_branch_hash;
    std::ifstream head_file(".minigit/HEAD");
    std::string head_ref;
    std::getline(head_file, head_ref);
    head_file.close();

    if (head_ref.rfind("ref: refs/heads/", 0) == 0)
    {
        std::string ref_path = ".minigit/" + head_ref.substr(5);
        std::ifstream ref_file(ref_path);
        std::getline(ref_file, current_branch_hash);
        ref_file.close();
    }
    else
    {
        std::cerr << "Error: Detached HEAD. Cannot merge." << std::endl;
        return;
    }

    std::string merge_branch_hash;
    std::string merge_branch_path = ".minigit/refs/heads/" + branch_to_merge;
    if (!fs::exists(merge_branch_path))
    {
        std::cerr << "Error: Branch \"" << branch_to_merge << "\" not found." << std::endl;
        return;
    }
    std::ifstream merge_ref_file(merge_branch_path);
    std::getline(merge_ref_file, merge_branch_hash);
    merge_ref_file.close();

    if (current_branch_hash == merge_branch_hash)
    {
        std::cout << "Already up to date." << std::endl;
        return;
    }

    std::string common_ancestor_hash = find_common_ancestor(current_branch_hash, merge_branch_hash);

    std::map<std::string, std::string> current_files = get_files_from_commit(current_branch_hash);
    std::map<std::string, std::string> merge_files = get_files_from_commit(merge_branch_hash);
    std::map<std::string, std::string> ancestor_files = get_files_from_commit(common_ancestor_hash);

    bool conflict = false;
    std::string merge_commit_message = "Merge branch \"" + branch_to_merge + "\"";

    // Apply changes from merge_files to current_files
    for (const auto& [filename, merge_sha1] : merge_files)
    {
        std::string current_sha1 = current_files[filename];
        std::string ancestor_sha1 = ancestor_files[filename];

        if (current_sha1 == merge_sha1) {
            // No change or same change in both branches
            continue;
        } else if (current_sha1 == ancestor_sha1) {
            // File changed in merge branch, not in current branch
            // Copy file from merge branch to working directory and add to index
            std::string blob_path = ".minigit/objects/" + merge_sha1;
            std::ifstream src(blob_path, std::ios::binary);
            std::ofstream dst(filename, std::ios::binary);
            dst << src.rdbuf();
            add(filename); // Add to index
        } else if (merge_sha1 == ancestor_sha1) {
            // File changed in current branch, not in merge branch
            // Do nothing, current version is fine
        } else {
            // Conflict: file changed in both branches differently
            std::cerr << "Conflict in file: " << filename << std::endl;
            conflict = true;
            // For simplicity, we'll just leave the current version and report conflict
            // In a real Git, this would involve conflict markers in the file
        }
    }

    // Handle files deleted in merge branch but present in current branch
    for (const auto& [filename, current_sha1] : current_files)
    {
        if (merge_files.find(filename) == merge_files.end() && ancestor_files.find(filename) != ancestor_files.end())
        {
            // File deleted in merge branch, but present in ancestor and current
            // For simplicity, we'll delete it from working directory and index
            fs::remove(filename);
            // TODO: Remove from index
        }
    }

    if (conflict)
    {
        std::cerr << "Merge failed due to conflicts. Please resolve them manually." << std::endl;
        return;
    }

    // Create merge commit
    std::string commit_content = "tree "; // Placeholder for tree hash
    commit_content += "\nparent " + current_branch_hash;
    commit_content += "\nparent " + merge_branch_hash;

    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&current_time), "%c");
    commit_content += "\nauthor Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> " + ss.str();
    commit_content += "\ncommitter Samuel Godad and Firamit Megersa <godadsamuel@gmail.com> " + ss.str();
    commit_content += "\n\n" + merge_commit_message + "\n";

    // Add current working directory files to commit content (simplified)
    for (const auto& entry : fs::directory_iterator(fs::current_path()))
    {
        if (entry.path().filename() != ".minigit")
        {
            std::string file_sha1 = calculate_file_sha1(entry.path().string());
            if (!file_sha1.empty())
            {
                commit_content += file_sha1 + " " + entry.path().filename().string() + "\n";
            }
        }
    }

    void* commit_hash_result = hashing::sha1::hash(commit_content);
    std::string merge_commit_sha1 = hashing::sha1::sig2hex(commit_hash_result);
    delete[] static_cast<uint8_t*>(commit_hash_result);

    std::ofstream merge_commit_file(".minigit/objects/" + merge_commit_sha1);
    merge_commit_file << commit_content;
    merge_commit_file.close();

    // Update HEAD and current branch pointer
    std::string current_branch_name = head_ref.substr(16);
    std::ofstream head_update_file(".minigit/HEAD");
    head_update_file << "ref: refs/heads/" << current_branch_name << "\n";
    head_update_file.close();

    std::ofstream branch_update_file(".minigit/refs/heads/" + current_branch_name);
    branch_update_file << merge_commit_sha1 << "\n";
    branch_update_file.close();

    std::cout << "Merged branch \"" << branch_to_merge << "\" into " << current_branch_name << std::endl;
    std::cout << "Merge commit: " << merge_commit_sha1.substr(0, 7) << std::endl;
}

// Shows the differences between two files.
void diff(const std::string& file1_path, const std::string& file2_path) {
    std::string content1 = read_file_content(file1_path);
    std::string content2 = read_file_content(file2_path);

    std::vector<std::string> lines1, lines2;
    std::string line;
    std::stringstream ss1(content1);
    while (std::getline(ss1, line)) {
        lines1.push_back(line);
    }
    std::stringstream ss2(content2);
    while (std::getline(ss2, line)) {
        lines2.push_back(line);
    }

    size_t i = 0, j = 0;
    while (i < lines1.size() || j < lines2.size()) {
        if (i < lines1.size() && j < lines2.size()) {
            if (lines1[i] == lines2[j]) {
                std::cout << "  " << lines1[i] << std::endl;
                i++;
                j++;
            } else {
                // Find if line from lines1 exists later in lines2
                size_t temp_j = j;
                bool found_in_lines2 = false;
                while (temp_j < lines2.size()) {
                    if (lines1[i] == lines2[temp_j]) {
                        found_in_lines2 = true;
                        break;
                    }
                    temp_j++;
                }

                // Find if line from lines2 exists later in lines1
                size_t temp_i = i;
                bool found_in_lines1 = false;
                while (temp_i < lines1.size()) {
                    if (lines2[j] == lines1[temp_i]) {
                        found_in_lines1 = true;
                        break;
                    }
                    temp_i++;
                }

                if (found_in_lines2 && !found_in_lines1) {
                    std::cout << "+ " << lines2[j] << std::endl;
                    j++;
                } else if (!found_in_lines2 && found_in_lines1) {
                    std::cout << "- " << lines1[i] << std::endl;
                    i++;
                } else {
                    // Both changed or both new/deleted, print both as changes
                    std::cout << "- " << lines1[i] << std::endl;
                    std::cout << "+ " << lines2[j] << std::endl;
                    i++;
                    j++;
                }
            }
        } else if (i < lines1.size()) {
            std::cout << "- " << lines1[i] << std::endl;
            i++;
        } else if (j < lines2.size()) {
            std::cout << "+ " << lines2[j] << std::endl;
            j++;
        }
    }
}

// Lists all branches and marks the current branch
void list_branches()
{
    std::string current_branch = "";
    std::ifstream head_file(".minigit/HEAD");
    std::string head_ref;
    std::getline(head_file, head_ref);
    head_file.close();

    if (head_ref.rfind("ref: refs/heads/", 0) == 0) {
        current_branch = head_ref.substr(16);
    }

    std::string refs_path = ".minigit/refs/heads";
    if (!fs::exists(refs_path)) {
        std::cout << "No branches found." << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(refs_path)) {
        std::string branch_name = entry.path().filename().string();
        if (branch_name == current_branch) {
            std::cout << "* " << branch_name << std::endl;
        } else {
            std::cout << "  " << branch_name << std::endl;
        }
    }
}

// Shows current branch and repository status
void status()
{
    std::ifstream head_file(".minigit/HEAD");
    std::string head_ref;
    std::getline(head_file, head_ref);
    head_file.close();

    if (head_ref.rfind("ref: refs/heads/", 0) == 0) {
        std::string current_branch = head_ref.substr(16);
        std::cout << "On branch " << current_branch << std::endl;
    } else {
        std::cout << "HEAD detached at " << head_ref.substr(0, 7) << std::endl;
    }

    // Check if there are staged files
    std::ifstream index_file(".minigit/index");
    std::string line;
    bool has_staged_files = false;
    while (std::getline(index_file, line)) {
        if (!line.empty()) {
            has_staged_files = true;
            break;
        }
    }
    index_file.close();

    if (has_staged_files) {
        std::cout << "\nChanges to be committed:" << std::endl;
        std::ifstream index_file_read(".minigit/index");
        while (std::getline(index_file_read, line)) {
            if (!line.empty()) {
                size_t space_pos = line.find(" ");
                if (space_pos != std::string::npos) {
                    std::string filename = line.substr(space_pos + 1);
                    std::cout << "  new file:   " << filename << std::endl;
                }
            }
        }
        index_file_read.close();
    } else {
        std::cout << "\nnothing to commit, working tree clean" << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: minigit <command> [args]\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "init")
    {
        init();
    }
    else if (command == "add")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: minigit add <filename>\n";
            return 1;
        }
        add(argv[2]);
    }
    else if (command == "commit")
    {
        if (argc < 3 || std::string(argv[2]) != "-m")
        {
            std::cerr << "Usage: minigit commit -m \"<message>\"\n";
            return 1;
        }
        std::string message = argv[3];
        commit(message);
    }
    else if (command == "log")
    {
        log();
    }
    else if (command == "branch")
    {
        if (argc < 3)
        {
            // No arguments provided - list branches
            list_branches();
        }
        else
        {
            // Branch name provided - create new branch
            branch(argv[2]);
        }
    }
    else if (command == "checkout")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: minigit checkout <branch-name> or <commit-hash>\n";
            return 1;
        }
        checkout(argv[2]);
    }
    else if (command == "merge")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: minigit merge <branch-name>\n";
            return 1;
        }
        merge(argv[2]);
    }
    else if (command == "diff")
    {
        if (argc < 4)
        {
            std::cerr << "Usage: minigit diff <file1> <file2>\n";
            return 1;
        }
        diff(argv[2], argv[3]);
    }
    else if (command == "status")
    {
        status();
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}


