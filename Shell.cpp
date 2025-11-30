#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <list>
#include <deque>
#include<stack>
#include <stdio.h>
#include <conio.h>
#include <algorithm> 
#include <iterator>
#include <windows.h>
using namespace std;


string lastCommand = "";

void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
class file {
public:
	string name;
	string data;
	int charCount;
	int lineCount;
	bool lock;
	int pwd;
	double avgCharPerLine;
	time_t creationTime;
	string owner;
	bool readOnly;
	int priority;
	int timeToPrint;
	string fileType;
	string path;
	file(string n = "NULL", string o = "NULL", bool r = false, int p = 0, int t = 0, string fT = "NULL", string pth = "", time_t cT = time(0), string d = "") {
		name = n;
		owner = o;
		readOnly = r;
		priority = p;
		timeToPrint = t;
		fileType = fT;
		path = pth;
		creationTime = cT;
		data = d;
		lock = false;
	}

	void print() {
		setColor(1);
		cout << "Name: " << name << endl;
		cout << "Owner: " << owner << endl;
		cout << "Read Only: " << readOnly << endl;
		cout << "Priority: " << priority << endl;
		cout << "Time to Print: " << timeToPrint << endl;
		cout << "File Type: " << fileType << endl;
		cout << "Path: " << path << endl;
	}
	int FileSize() {
		return data.length();
	}
	bool getlock() {
		return lock;
	}
	void setData(const string& d) {
		data = d;
	}
	void togglePWD() {
		pwd = !pwd;
	}
	void setPWD(bool val) {
		pwd = val;
	}
	bool matchPWD(int password) {
		return pwd == password;
	}
};

class directory {
public:
	friend class Tree;
	string name;
	time_t creationTime;
	string owner;
	bool hidden;
	vector<directory*> subFolders;
	vector<file*> files;
	directory* parent;
	string path;
	directory(string n = "NULL", string p = "", directory* par = nullptr, vector<file*> fs = vector<file*>(), vector<directory*> FS = vector<directory*>()) {
		name = n;
		files = fs;
		subFolders = FS;
		parent = par;
		path = p;
		hidden = false;
	}
	bool removeFile(const string& name) {
		for (int i = 0; i < files.size(); i++) {
			if (files[i]->name == name) {
				// Swap with the last element and pop back
				swap(files[i], files[files.size() - 1]);
				files.pop_back();
				return true; // Return true for successful removal
			}
		}

		// If the loop completes without finding the file, return false
		return false;
	}
	bool removeFolder(const string& name) {
		for (int i = 0; i < subFolders.size(); i++) {
			if (subFolders[i]->name == name) {
				// Swap with the last element and pop back
				swap(subFolders[i], subFolders[subFolders.size() - 1]);
				subFolders.pop_back();
				return true; // Return true for successful removal
			}
		}

		// If the loop completes without finding the folder, return false
		return false;
	}


	void addFile(file* f) {
		files.push_back(f);
	}
	void addFolder(directory* f) {
		subFolders.push_back(f);
	}
	void emptyDirectory() {
		files.resize(0);
		subFolders.resize(0);
	}
	file* findFile(string name) {
		for (int i = 0; i < files.size(); i++) {
			if (files[i]->name == name) {
				return files[i];
			}
		}
		return nullptr;
	}
	directory* findFolder(string name) {
		for (int i = 0; i < subFolders.size(); i++) {
			if (subFolders[i]->name == name) {
				return subFolders[i];
			}
		}
		return nullptr;
	}
	// Modify the print function for dir
	void print(directory* current, const string& filterExtension = "") {
		// Display the directory structure
		setColor(4);
		cout << "Directory of " << current->path << "\\" << current->name << "\\" << endl << endl;

		for (int i = 0; i < current->subFolders.size(); i++) {
			cout << "                          <DIR>      " << current->subFolders[i]->name << endl;
		}

		for (int i = 0; i < current->files.size(); i++) {
			// Extract the file extension from the file name
			size_t dotPos = current->files[i]->name.find_last_of('.');
			string fileExtension = (dotPos != string::npos) ? current->files[i]->name.substr(dotPos + 1) : "";

			// Check if the file matches the filter extension or if no filter is applied
			if (filterExtension.empty() || caseInsensitiveStringCompare(fileExtension, filterExtension)) {
				cout << "                           " << current->files[i]->name << endl;
			}
		}
	}
	// Helper function to compare strings case-insensitively
	bool caseInsensitiveStringCompare(const string& str1, const string& str2) {
		return std::equal(str1.begin(), str1.end(), str2.begin(), str2.end(), [](char ch1, char ch2) {
			return tolower(ch1) == tolower(ch2);
			});
	}
	void format(const string& formatExtension) {
		if (formatExtension == "*" || formatExtension == "*.*") {
			// If * or *.* is provided, delete all files
			emptyDirectory();
		}
		else if (!formatExtension.empty()) {
			// Delete files with the specified extension (case-sensitive)
			auto it = std::remove_if(files.begin(), files.end(),
				[&](const auto& file) {
					return file->fileType == formatExtension;
				});

			// Erase the files that match the specified extension
			files.erase(it, files.end());
		}
		// If formatExtension is empty, do nothing
	}



};

class Tree {
public:
	directory* root;
	directory* current;

	Tree() {
		current = root = new directory("V");
	}

	void setPath(directory* f) {
		string path = current->name;
		directory* parent = new directory();
		while (parent != nullptr) {
			path = parent->name + "/" + path;
			parent = parent->parent;
		}
		f->path = path;
	}
	void print(string prompt = "\\>") {
		setColor(14);
		// Check if the current command is "tree"
		if (lastCommand == "tree") {
			// Display the directory structure
			displayDirectoryStructure(current, "");
			lastCommand = "";
		}
		else {
			if (current->name == "V") {
				cout << "V:" << prompt;
			}
			else {
				cout << "V:" << current->path.substr(2, current->path.length()) << "\\" << current->name << prompt;
			}
		}
	}
	bool isFolder(string name) {
		for (int i = 0; i < current->subFolders.size(); i++) {
			if (current->subFolders[i]->name == name) {
				return true;
			}
		}
		return false;
	}
	void displayDirectoryStructure(directory* folder, const string& indent) {
		cout << endl << indent << "+---" << folder->name << "\\";

		for (size_t i = 0; i < folder->subFolders.size(); ++i) {
			displayDirectoryStructure(folder->subFolders[i], indent + "|   ");
		}

		for (size_t i = 0; i < folder->files.size(); ++i) {
			cout << endl << indent + "|   " << folder->files[i]->name;
		}
		cout << endl;
	}
	bool isFile(string name) {
		for (int i = 0; i < current->files.size(); i++) {
			if (current->files[i]->name == name) {
				return true;
			}
		}
		return false;
	}
	void insert(directory* f) {
		current->subFolders.push_back(f);
	}
	file* findFile(string name) {
		for (int i = 0; i < current->files.size(); i++) {
			if (current->files[i]->name == name) {
				return current->files[i];
			}
		}
		return nullptr;
	}
	directory* findFolder(string name) {
		for (int i = 0; i < current->subFolders.size(); i++) {
			if (current->subFolders[i]->name == name) {
				return current->subFolders[i];
			}
		}
		return nullptr;
	}
	void listitems() {
		cout << "Folders: " << endl;
		for (int i = 0; i < current->subFolders.size(); i++) {
			cout << current->subFolders[i]->name << endl;
		}
		cout << "Files: " << endl;
		for (int i = 0; i < current->files.size(); i++) {
			cout << current->files[i]->name << "." << current->files[i]->fileType <<endl;
		}
	}
};

class Command {
private:
	string inLowerCase(string s) {
		for (int i = 0; i < s.length(); i++) {
			s[i] = tolower(s[i]);
		}
		return s;
	}
	string buffer = "";
	bool prompt = false;
	bool f_out = false;
	bool out = true;
	vector<string> commands;
public:
	Tree tree;
	string User = "Hammad";
	ofstream SaveTree(ofstream writer) {
		saveDirectory(tree.root, writer);
		writer.close();
		return writer;
	}
	ifstream loadTree(ifstream reader) {
		tree.root = loadDirectory(nullptr, reader);
		tree.current = tree.root;
		reader.close();
		return reader;
	}
	Command() {

	}
	void print() {
		setColor(12);
		cout << "\t\t\t\t\t\t" << "Rana Hammad Ahmad : BSCS23192" << endl;
		tree.print();
	}
	void run() {
		print();
		loadTree(ifstream("tree.txt"));
		bool run = false;
		while (!run) {
			run = executeCommand();
		}
	}
	directory* findFolder(string name) {
		for (int i = 0; i < tree.current->subFolders.size(); i++) {
			if (tree.current->subFolders[i]->name == name) {
				return tree.current->subFolders[i];
			}
		}
		return nullptr;
	}
	bool executeCommand() {
		string command;
		
		if (commands.empty()) {

			getline(cin, command);
			if (command.find('|')!=-1) {
				while (command.find('|') != -1) {
					commands.push_back(command.substr(0, command.find('|')-1));
					command = command.substr(command.find('|') + 1, command.length());
				}
				commands.push_back(command.substr(1, command.length()-1));
			}
			else {
				commands.push_back(command);
			}

		}
		/*else {
			
		}*/
		command = commands[0];
		commands.erase(commands.begin());
		string opr = command.substr(0, command.find(' '));
		opr = inLowerCase(opr);
		if (opr == "mkdir") {
			tree.insert(new directory(command.substr(6, command.length()), tree.current->path + "\\" + tree.current->name, tree.current));
		}
		else if (opr == "touch") {
			// Extract file name and extension from the command
			string fileName = command.substr(6, command.length());
			if (fileName.empty()) {
				cout << "Error: File name cannot be empty." << endl;
				return false;
			}
			else if (tree.isFile(fileName)) {
				cout << "Error: File '" << fileName << "' already exists." << endl;
				return false;
			}
			size_t dotPos = fileName.find('.');
			if (dotPos != string::npos) {
				string fileBaseName = fileName.substr(0, dotPos);
				string fileExtension = fileName.substr(dotPos + 1);

				file* f = new file(fileBaseName, User);
				f->fileType = fileExtension;
				tree.current->files.push_back(f);

				cout << "File '" << fileName << "' created successfully." << endl;
			}
			else {
				file* f = new file(fileName, User);
				tree.current->files.push_back(f);

				cout << "File '" << fileName << "' created successfully." << endl;
			}
		}
		else if (opr == "del") {
			string fileName = command.substr(4, command.length());
			if (tree.current->removeFile(fileName)) {
				cout << "File '" << fileName << "' deleted successfully." << endl;
			}
			else {
				cout << "File '" << fileName << "' not found." << endl;
			}
		}
		else if (opr == "cd") {
			string folderPath = command.substr(3, command.length());
			if (tree.isFolder(folderPath))
				tree.current = tree.findFolder(folderPath);
			else {
				cout << "The system cannot find the path specified." << endl;
			}
		}
		else if (opr == "cd..") {
			if (tree.current->parent != nullptr)
				tree.current = tree.current->parent;
		}
		else if (opr == "cd\\") {
			if (tree.current->parent != nullptr)
				tree.current = tree.root;
		}
		else if (opr == "dir" || opr == "cd.") {
			size_t pos = command.find(' ');
			if (pos != string::npos) {
				string filterExtension = command.substr(pos + 1); // Extract extension after '*'
				tree.current->print(tree.current, filterExtension);
			}
			else {
				tree.current->print(tree.current);
			}
		}
		else if (opr == "tree") {
			lastCommand = "tree";
			tree.print();
		}
		else if (opr == "ls") {
			tree.listitems();
		}
		else if (opr == "fork") {
			string programName = command.substr(5, command.length());
			system(programName.c_str());


		}
		else if (opr == "execvp") {
			string programName = command.substr(7, command.length());
			system(programName.c_str());
		}
		else if (opr == "clear") {
			system("cls");
			//print();
		}
		else if (opr == "find") {
			string name = command.substr(5, command.length());
			file* f = tree.findFile(name);

			if (f != nullptr) {
				cout << "File found: " << f->data << endl;
			}
			else {
				cout << "File not found." << endl;
			}
		}
		else if (opr == "attrib") {
			string name = command.substr(7, command.length());
			file* f = tree.findFile(name);

			if (f != nullptr) {
				cout << "File attributes for " << f->data << ":" << endl;
				f->print();
			}
			else {
				cout << "File not found." << endl;
			}
		}
		else if (opr == "format") {
			size_t pos = command.find(' ');
			if (pos != string::npos) {
				string formatExtension = command.substr(pos + 1); // Extract extension after '*'
				tree.current->format(formatExtension);
				cout << "Operation completed: Files with extension " << formatExtension << " deleted." << endl;
			}
			else {
				// If no extension is provided, empty the directory
				tree.current->format("");
				cout << "Operation completed: Directory formatted." << endl;
			}
		}
		else if (opr == "rmdir") {
			if (tree.current == tree.root) {
				cout << "At the root directory...." << endl;
			}
			else {
				directory* current = tree.current;
				tree.current = tree.current->parent;
				string folderName = current->name;

				if (tree.current->removeFolder(folderName)) {
					cout << "Folder '" << folderName << "' removed successfully." << endl;
				}
				else {
					cout << "Error: Folder '" << folderName << "' not found or not empty." << endl;
				}
			}
		}

		else if (opr == "prompt") {
			prompt = !prompt;  // Toggle prompt value
		}
		else if (opr == "rename") {
			command = command.substr(opr.length() + 1, command.length());
			string oldName = command.substr(0, command.find(' '));
			string newName = command.substr(command.find(' ') + 1, command.length());
			file* f = tree.current->findFile(oldName);

			if (f != nullptr) {
				f->name = newName;
				cout << "File or folder renamed successfully." << endl;
			}
			else {
				cout << "Error: File or folder '" << oldName << "' not found." << endl;
			}
		}
		else if (opr == "lockfile"){
			string name = command.substr(9, command.length());
			string test = name.substr(0, name.find('.'));
			file* f = tree.findFile(test);
			if (f == nullptr) {
				cout << "File not found." << endl;
			}
			else {
				f->lock = true;
				cout << "Enter password to lock the file: ";
				cin >> f->pwd;
				cout << "Operation completed: File locked." << endl;
			}
		}
		else if (opr == "edit") {
			string pwd;
			string name = command.substr(5, command.length());
			string test = name.substr(0,name.find('.'));
			file* f = tree.findFile(test);
			if (f == nullptr) {
				cout << "File not found." << endl;
			}
			else {
				if (f->getlock()) {
					cout << "Enter password to unlock the file: ";
					cin >> pwd;
					if (!f->matchPWD(stoi(pwd))) {
						cout << "Incorrect password. Access denied." << endl;
						tree.print();
						return false;
					}
				}
				//cin.ignore(); // Clear the newline character from the input buffer
				string data=f->data;
				cout << "Enter new data for the file (press Enter when done):\n";
				cout << data << endl;
				string newData;
				getline(cin, newData);
				f->setData(data+" "+newData);
				cout << "Operation completed: File edited." << endl;
			}
		}
		else if (opr == "copy") {
			command = command.substr(opr.length() + 3, command.length());
			int index = command.find(' ');
			string source = command.substr(0, index);
			string destination = command.substr(index + 3, command.length());
			file* fileToCopy;
			directory* current = tree.root;
			string f;

			// Find and navigate to the source folder
			while (source.length() > 0) {
				f = source.substr(0, source.find('\\'));
				source = source.substr(f.length() + 1, source.length());

				if (source.length() != 0)
					current = current->findFolder(f);

				if (current == nullptr) {
					cout << "The system cannot find the path specified." << endl;
					return true;
				}
			}

			// Find the file to copy
			fileToCopy = current->findFile(f);
			current = tree.root;

			// Find and navigate to the destination folder
			while (destination.length() > 0) {
				f = destination.substr(0, destination.find('\\'));
				destination = destination.substr(f.length() + 1, destination.length());

				current = current->findFolder(f);

				if (current == nullptr) {
					cout << "The system cannot find the path specified." << endl;
					return true;
				}
			}

			// Copy the file to the destination folder
			current->addFile(new file(fileToCopy->name));
		}
		else if (opr == "mov") {
			command = command.substr(opr.length() + 3, command.length());
			int index = command.find(' ');
			string source = command.substr(0, command.find(' '));
			string destination = command.substr(source.length() + 3, command.length());
			file* fileToMove;
			directory* current = tree.root;
			string f;
			while (source.length() > 0) {
				f = source.substr(0, source.find('\\'));
				source = source.substr(f.length() + 1, source.length());
				if (source.length() != 0)
					current = current->findFolder(f);
				if (current == nullptr) {
					cout << "The system cannot find the path specified." << endl;
					return true;
				}
			}
			fileToMove = current->findFile(f);
			current->removeFile(fileToMove->name);
			current = tree.root;
			while (destination.length() > 0) {
				f = destination.substr(0, destination.find('\\'));
				destination = destination.substr(f.length() + 1, destination.length());
				current = current->findFolder(f);
				if (current == nullptr) {
					cout << "The system cannot find the path specified." << endl;
					return true;
				}
			}
			current->addFile(fileToMove);
		}
		//ver
		else if (opr == "ver") {
			cout << "Microsoft Windows [Version 10.0.19041.746]" << endl;
		}
		else if (opr == "exit") {
			setColor(10); // Set color to green (you can change this value)
			cout << "Thank you for using My DOS. Have a great day!" << endl;
			setColor(7);  // Reset color to default
			SaveTree(ofstream("tree.txt"));
			return true;
		}

		else if (opr == "convert") {
			// Extract old and new file types from the command
			size_t pos = command.find(' ');
			if (pos != string::npos) {
				string oldType = command.substr(pos + 1, command.find(' ', pos + 1) - pos - 1);
				string newType = command.substr(command.find(' ', pos + 1) + 1);

				// Convert files with the old type to the new type
				bool conversionSuccess = false;
				for (int i = 0; i < tree.current->files.size(); i++) {
					if (tree.current->files[i]->fileType == oldType) {
						tree.current->files[i]->fileType = newType;
						conversionSuccess = true;
					}
				}

				// Display acknowledgment message
				if (conversionSuccess) {
					cout << "File type converted successfully." << endl;
				}
				else {
					cout << "No files with the specified old type found." << endl;
				}
			}
		}
		else if (opr == "grep") {
			vector<string> tokens = tokenize(command.substr(opr.length() + 1, command.length()));
			string text = tokens[0];
			tokens.erase(tokens.begin());
			if (tokens.empty()) {
				int pos, oldpos = 0;
				string copybuffer = buffer;

				if (copybuffer != "") {
					while (copybuffer.find(text) != string::npos) {
						pos = copybuffer.find(text);
						//cout << "Text found at position: " << pos+oldpos << endl;
						buffer += "Text found at position: " + to_string(pos + oldpos) + "\n";
						copybuffer = copybuffer.substr(pos + text.length(), copybuffer.length());
						oldpos += pos + text.length();
					}
				}
				else {
					cout << "No Result" << endl;
				}
			}
			else if(tokens[0]=="<" || tokens[0] == "<<" || tokens[0]==">") {
				if (tokens.size() < 2) {
					cout << "Invalid command format." << endl;
					return false;
				}
				if (tokens[0] == "<") {
					string filename = tokens[1];
					file* f = tree.findFile(filename);
					int pos, oldpos = 0;
					if (f != nullptr) {
						while (f->data.find(text) != string::npos) {
							pos = f->data.find(text);
							//cout << "Text found at position: " << pos+oldpos << endl;
							buffer += "Text found at position: " + to_string(pos + oldpos) + "\n";
							f->data = f->data.substr(pos + text.length(), f->data.length());
							oldpos += pos + text.length();
						}
					}
					else {
						cout << "File not found" << endl;
					}
				}
				else if (tokens[0] == ">>") {
					string filename = tokens[1];
					ofstream writer(filename.c_str(), ios::app);
					writer << buffer;
					writer.close();
					buffer = "";
				}
				else if (tokens[0] == "<<") {
					string filename = tokens[1];
					ifstream reader(filename.c_str());
					string fileContent((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
					reader.close();
					int pos, oldpos = 0;
					while (fileContent.find(text) != string::npos) {
						pos = fileContent.find(text);
						//cout << "Text found at position: " << pos+oldpos << endl;
						buffer += "Text found at position: " + to_string(pos + oldpos) + "\n";
						fileContent = fileContent.substr(pos + text.length(), fileContent.length());
						oldpos += pos + text.length();
					}
				}

			}
			else {
				string name = tokens[0];
				string test = name.substr(0, name.find('.'));
				file* f = tree.findFile(test);
				int pos, oldpos = 0;
				if (f != nullptr) {
					while (f->data.find(text) != string::npos) {
						pos = f->data.find(text);
						//cout << "Text found at position: " << pos+oldpos << endl;
							buffer += "Text found at position: " + to_string(pos + oldpos) + "\n";
						f->data = f->data.substr(pos + text.length(), f->data.length());
						oldpos += pos + text.length();
					}
				}
				else {
					cout << "File not found" << endl;
				}
			}
		}
		else if (opr == "wc") {
			command = command.substr(opr.length() + 1, command.length());
			string name = command.substr(0, command.find(' '));
			string test = name.substr(0, name.find('.'));
			file* f = tree.findFile(test);
			//file* f = tree.findFile(name);
			if (f != nullptr) {
				cout << f->FileSize() << endl;
			}
			else {
				cout << "File not found" << endl;
			}
		}
		else if (opr == "findstr") {
			command = command.substr(opr.length() + 1, command.length());
			string text = command.substr(0, command.find(' '));
			bool found = false;  // Flag to track if any matching files are found

			for (int i = 0; i < tree.current->files.size(); i++) {
				if (tree.current->files[i]->data.find(text) != string::npos) {
					cout << tree.current->files[i]->name << endl;
					found = true;
				}
			}

			if (!found) {
				cout << "No matching files found." << endl;
			}
		}
		else if (opr == "print") {
			command = command.substr(opr.length() + 1, command.length());
			string name = command.substr(0, command.find(' '));
			file* f = tree.findFile(name);
			if (f != nullptr) {
				f->priority += 1;  // Increment priority
				cout << "File " << name << " added to print queue with priority " << f->priority << endl;
			}
			else {
				cout << "File not found" << endl;
			}
		}

		else if (opr == "pprint") {
			command = command.substr(opr.length() + 1, command.length());
			string name = command.substr(0, command.find(' '));
			file* f = tree.findFile(name);
			if (f != nullptr) {
				f->priority += 1;  // Increment priority
				cout << "File " << name << " added to priority print queue with priority " << f->priority << endl;
			}
			else {
				cout << "File not found" << endl;
			}
		}
		else if (opr == "pqueue") {
			cout << "Priority based print queue" << endl;
			for (int i = 0; i < tree.current->files.size(); i++) {
				cout << tree.current->files[i]->name << " " << tree.current->files[i]->priority << " " << tree.current->files[i]->timeToPrint << endl;
			}
		}

		else if (opr == "queue") {
			cout << "Print queue" << endl;
			for (int i = 0; i < tree.current->files.size(); i++) {
				cout << tree.current->files[i]->name << " " << tree.current->files[i]->timeToPrint << endl;
			}
		}
		else if (opr == "save") {
			command = command.substr(opr.length() + 1, command.length());
			string name = command.substr(0, command.find(' '));
			file* f = tree.findFile(name);

			if (f != nullptr) {
				// Display the current content of the file
				cout << "Current content of file " << name << ":\n";
				cout << f->data << endl;

				// Ask the user if they want to make modifications
				cout << "Do you want to make modifications? (Y/N): ";
				char choice;
				cin >> choice;

				cin.ignore(); // Clear the newline character from the input buffer

				if (toupper(choice) == 'Y') {
					// Get the new content from the user
					cout << "Enter the new content (press Ctrl+Z followed by Enter to save):" << endl;
					stringstream buffer;
					buffer << cin.rdbuf();  // Read input until Ctrl+Z is pressed

					// Update the file content
					f->data = buffer.str();
					cout << "File " << name << " saved" << endl;
				}
				else {
					cout << "File not modified" << endl;
				}
			}
			else {
				cout << "File not found" << endl;
			}
		}
		else if (opr == "pwd") {
			cout << tree.current->path << endl;
		}
		//else if (opr == "loadtree") {
		//	command = command.substr(opr.length() + 1, command.length());
		//	string name = command.substr(0, command.find(' '));

		//	ifstream rdr(name.c_str());

		//	if (!rdr.is_open()) {
		//		cerr << "Error: Unable to open file " << name << endl;
		//		// Continue with the execution or handle the error as needed
		//	}
		//	else {
		//		//loadTree(rdr);
		//		rdr.close();
		//		cout << "Success..." << endl;
		//	}
		//}
		else if (opr == "help") {
			cout << "ATTRIB Displays file (name provided as input) attributes." << endl;
			cout << "CD Displays the name of or changes the current directory. See details below." << endl;
			cout << "CD. Prints working directory (i.e. your current directory in your tree structure)" << endl;
			cout << "CD.. Change directory to previous directory" << endl;
			cout << "CD\\ Changes directory to root directory (i.e. V:\\&gt;)" << endl;
			cout << "CONVERT Asks two types and converts extension of all files of one type to another type." << endl;
			cout << "COPY Copies one file in the current directory to another location (directory). See details below." << endl;
			cout << "CREATE Creates a file with the name provided and allows the user to enter contents of the file." << endl;
			cout << "DEL Delete a file whose name is provided in input." << endl;
			cout << "DIR Displays a list of files and subdirectories in a directory." << endl;
			cout << "EDIT Opens a file (loads into a linked list) and allows the user to edit and save the contents of" << endl;
			cout << "the file. See bullet 5 below" << endl;
			cout << "EXIT Quits the program" << endl;
			cout << "FIND Searches for a file in your current virtual directory whose name is provided as input." << endl;
			cout << "FINDF Searches for a text string in the currently open file or the file whose name is provided as" << endl;
			cout << "FINDSTR Searches for strings in all files of your current virtual directory, prints names of files with" << endl;
			cout << "the string" << endl;
			cout << "FORMAT Formats the current virtual directory i.e. empties the current directory and all" << endl;
			cout << "HELP Provides Help information for all these commands." << endl;
			cout << "LOADTREE Load a given tree in your tree structure. Given tree is in a file named tree.txt" << endl;
			cout << "MKDIR Creates a virtual directory." << endl;
			cout << "MOVE Moves one file (whose name is provided as input) from one directory to another" << endl;
			cout << "PPRINT Adds a text file to the priority based print queue, and displays the current priority queue." << endl;
			cout << "PROMPT Changes the Windows command prompt (for example from V:\\&gt; to V$)." << endl;
			cout << "PRINT Adds a text file to the print queue, and displays the current queue." << endl;
			cout << "PQUEUE Shows current state of the priority based print queue, with time left for each element" << endl;
			cout << "PWD Prints working directory" << endl;
			cout << "QUEUE Shows current state of the print queue, with time left for each element" << endl;
			cout << "RENAME Renames a file whose current and new name is provided as input." << endl;
			cout << "RMDIR Removes a directory along with its contents. Consider setting the respective sibling links" << endl;
			cout << "SAVE Saves the currently open file to disk." << endl;
			cout << "TREE Displays the complete directory structure." << endl;
			}
		else if (opr == "") {
			// Do nothing for empty command
			return false;
		}
		else {
			cout << "Invalid command..." << endl;
			tree.print();
			return false;
		}
		if (opr != "cls") {
			if (prompt) {
				tree.print("$")         ;
			}
			else if (commands.empty()) {
				if (!f_out) {
					cout << buffer;
					buffer = "";
				}
				tree.print();
				SaveTree(ofstream("tree.txt"));
			}
		}
		
		return false;
	}
	/*bool DecideOutputMode() {
		if (f_out) {
			out = false;
		}
		else {
			out = true;
		}
	}*/
	vector<string> tokenize(const string& command) {
		vector<string> tokens;
		stringstream ss(command);
		string token;

		while (ss >> token) {  
			tokens.push_back(token);
		}

		return tokens;
	}
	void SaveTree(ofstream& wtr) {
		if (!wtr.is_open()) {
			cerr << "Error: Unable to open file stream for saving tree." << endl;
			return;
		}

		try {
			saveDirectory(tree.root, wtr);
		}
		catch (const exception& e) {
			cerr << "Error during tree saving: " << e.what() << endl;
		}
	}
	void saveDirectory(directory* dir, ofstream& wtr) {
		if (dir == nullptr) {
			return;
		}

		// Write directory information
		wtr << dir->name << endl;

		// Write files in the directory
		for (size_t i = 0; i < dir->files.size(); ++i) {
			wtr << "F" << endl;
			wtr << dir->files[i]->name << endl;
		}

		// Recursively write subdirectories
		for (size_t i = 0; i < dir->subFolders.size(); ++i) {
			wtr << "D" << endl;
			saveDirectory(dir->subFolders[i], wtr);
		}

		// Indicate going back to parent directory
		wtr << "P" << endl;
	}
	 directory* loadDirectory(directory* parent, ifstream& rdr) {
		if (!rdr.is_open()) {
			cerr << "Error: Unable to open file stream for loading directory." << endl;
			return nullptr;
		}

		string line;
		if (!getline(rdr, line)) {
			return nullptr; // No more data to read
		}

		directory* dir = new directory(line, parent ? parent->path + "\\" + parent->name : "", parent);

		while (getline(rdr, line)) {
			if (line == "F") {
				getline(rdr, line);
				file* f = new file(line);
				dir->files.push_back(f);
			}
			else if (line == "D") {
				directory* subDir = loadDirectory(dir, rdr);
				if (subDir != nullptr) {
					dir->subFolders.push_back(subDir);
				}
			}
			else if (line == "P") {
				break; // Go back to parent directory
			}
		}

		return dir;
	}
	void decideOutputRedirection(const string& command) {
		// Check for output redirection operators
		size_t outPos = command.find('>');
		size_t appendPos = command.find(">>");

		if (appendPos != string::npos) {
			// Handle append redirection
			string cmd = command.substr(0, appendPos - 1);
			string filename = command.substr(appendPos + 2);
			f_out = true;
			out = false;
			// Redirect output to the specified file in append mode
			ofstream outFile(filename.c_str(), ios::app);
			if (outFile.is_open()) {
				// Execute the command and redirect output
				// executeCommand(cmd, outFile);
				outFile.close();
			}
			else {
				cerr << "Error: Unable to open file " << filename << " for appending." << endl;
			}
		}
		else if (outPos != string::npos) {
			// Handle overwrite redirection
			string cmd = command.substr(0, outPos - 1);
			string filename = command.substr(outPos + 1);
			f_out = true;
			out = false;
			// Redirect output to the specified file in overwrite mode
			ofstream outFile(filename.c_str());
			if (outFile.is_open()) {
				// Execute the command and redirect output
				// executeCommand(cmd, outFile);
				outFile.close();
			}
			else {
				cerr << "Error: Unable to open file " << filename << " for writing." << endl;
			}
		}
		else {
			f_out = false;
			out = true;
		}
	}
	string decideInputRedirection(const string& command) {
		// Check for input redirection operator
		size_t inPos = command.find('<');

		if (inPos != string::npos) {
			// Handle input redirection
			string cmd = command.substr(0, inPos - 1);
			string filename = command.substr(inPos + 1);
			// Redirect input from the specified file
			ifstream inFile(filename.c_str());
			if (inFile.is_open()) {
				// Execute the command with redirected input
				// executeCommand(cmd, inFile);
				inFile.close();
			}
			else {
				cerr << "Error: Unable to open file " << filename << " for reading." << endl;
			}
			return cmd;
		}
		else {
			// No input redirection, return the original command
			return command;
		}
	}
	void loadTree(ifstream& rdr) {
		if (!rdr.is_open()) {
			cerr << "Error: Unable to open file stream for loading tree." << endl;
			return;
		}

		try {
			string line;
			getline(rdr, line);

			// Assume the first line should contain the root folder information
			tree.root = new directory(line);
			tree.current = tree.root;

			while (getline(rdr, line)) {
				if (line == "F") {
					getline(rdr, line);
					file* f = new file(line);
					tree.current->files.push_back(f);
				}
				else if (line == "D") {
					getline(rdr, line);
					directory* f = new directory(line, "", tree.current);
					tree.current->subFolders.push_back(f);
					tree.current = f;
				}
				else if (line == "P") {
					if (tree.current->parent) {
						tree.current = tree.current->parent;
					}
					else {
						cerr << "Error: Trying to go up, but current folder has no parent." << endl;
						// Handle the error or exit the loop
						break;
					}
				}
			}
		}
		catch (const exception& e) {
			cerr << "Error during tree loading: " << e.what() << endl;
		}
	}
};


int main() {
	//write code to test shell
	Tree tree;
	Command cmd;
	cmd.run();
	system("color 09");
	return 0;
}