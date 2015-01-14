/*-------------------------
* Library System by Group 7
* -------------------------
*
* Description: A program that can manage the data of students and books in the library. 
*
* LICENSE
* This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

// Header files *******************************************************************************************************
#include <iostream>							// Standard include file
//#include <cstdlib>						// Disabled - For system("cls"), etc.
#include <string>							// Standard string header file
#include <Windows.h>						// To use the windows API for the console program. (Create directories, sleep, etc)
#include <fstream>							// To save all data to the files
#include <tchar.h>							// Use to create TCHAR class, which used in windows API
#include <sstream>							// Easy conversion from string to int
#include <time.h>							// Random seed time
#include <iomanip>							// Manipulator of cout/cin
#include <vector>							// Used in create menu
#include <algorithm>						// Basic bubble sort
//#include "PressAnyKey.h"					// Cause multiple key pressed bug. Custom function was created below to replace this.

// Definitions ********************************************************************************************************
// Program definitions
#define BUFSIZE MAX_PATH*2					// For directory check/create, used to create TCHAR array to store some characters
#define MAX_BOOK_PER_STUDENTS 2				// Limit to n books per students to borrow
#define VERSION 1.01						// Internal program version
#define nbsp cout << '\n';					// Shortcut to create newline, instead of typing full commands.

// Namepsace **********************************************************************************************************
// Default namespace in this code
using namespace std;

// Global variables / declarations ************************************************************************************
// GetStdHandle is use to make the program able to use built-in Windows specific functions.
// In other words, it is to extend the capability of this program.
HANDLE cBuffer = GetStdHandle(STD_OUTPUT_HANDLE);
unsigned int main_menu_selection = 0, student_menu_selection = 0, book_menu_selection = 0, index_page = 0, current_page = 1, list_index;	// To save the main menu position
bool esc_pressed = false;


// Prototypes *********************************************************************************************************
void MakeDirectories();													// Create directories 
void MainMenu();														// Menu function
void StudentMenu();														// Menu function
void BookMenu();														// Menu function
void ClearScreen();														// Clear the screen as system("cls")
string transbool(bool);													// Translate from true/false to Yes/No as string
void IssueBook();														// Function for the student to borrow book
void ReturnBook();														//     "     "  return the book
void AddNewStudent();													// Function that handle storing student info
void GetStudentInfo(bool menu = true, int id = -1);						// Find saved file and print out the info from it
void DeleteStudent();													// Find saved file and delete it
void ShowAllStudents();													// Find all related files and print out its data
void AddNewBook();														// For book,
void DeleteBook();														// almost same,
void GetBookInfo(bool menu = true, int id = -1);						// like,
void ShowAllBooks();													// student funcs :)
void SetPos(int, int);													// Set the cursor position in the console program
void AnimateString(int, int, string);									// Animate each character from the string and print it to the screen.
void setcolor(WORD w = 15);												// Set the color of the console text or background.
void Intro();															// A series of animatestring to show the intro of the program
void HideScrollBar();													// Hide the scrollbar of the console by resizing it to the minimum size of console.
void CreateMenu(vector<string> &menu_item, int x, int y, unsigned int &index, unsigned int *current_index = NULL);	// Create a menu from the string given. Current index use pointer to check for null that will be used in the functions.
void SetCursorSize(bool visible = true, DWORD size = 0);				// Cursor size in the console can be set using this function
void SetMenuTitle(string s = "Main Menu");								// Set the title of the program when run. Same as system("title ...") but this use windows API
void GetPos(COORD);														// Get the current cursor in the console
void print_tips(string s, int type = 0);								// A simplified version to print the status of the program
void PressAnyKey(string);												// A duplicate of press any key to continue using basic functions.
string xcin(unsigned int max_char, bool alphanumeric = true);			// Custom std::cin function, which able to customize the 
int string2int(string);													// Convert string to integer
void ClearLine(int, int);												// Clear the line in the console

// Classes ************************************************************************************************************
// Almost same as students class
class books {
private:
	int id = -1, borrowed_by = -1;										// Both will int type data. the borrowed_by also stored the student ID
	string title = "ID NOT EXISTS OR INVALID ", author = "AUTHOR NOT EXISTS OR INVALID ";	// Book title and author
	string location = "NOT EXISTS";
	bool available = true;												// A variable that store the availability status of the book.

public:
	books() {}															// Empty destructor and constructor
	~books() {}

	void setbooklocation(string s) {
		location = s;
	}

	string getbooklocation() {
		return location;
	}

	void reset() {														// Reset all the variables in the class to its default value.
		id = -1;
		borrowed_by = -1;
		title = "ID NOT EXISTS OR INVALID ";
		author = "AUTHOR NOT EXISTS OR INVALID";
		available = true;
	}

	string id2book(int this_id = -1) {									// Get the book title based on book id.
		if (this_id != -1 || id != -1) {
			int temp_id = id;
			string temp_title;
			if (load(this_id) != 1){
				temp_title = title;
				load(temp_id);
				return temp_title;
			}
			else {
				load(temp_id);
				return "-1";
			}
		}
		else return "-";
	}

	void setborrower(int i) {														// Set the id of a student that borrow certain book;
		borrowed_by = i;
	}

	void setauthor(string s) {														// Set the author of a book.
		author = s;
	}

	void toggleAvailable() {														// TOggle (available/unavailable) of a book
		if (available == true) available = false;
		else available = true;
	}

	void setAvailable(bool x) {														// Force set the availability of book.
		available = x;
	}

	bool isAvailable() {															// Check if book available to borrow.
		if (id == -1) return false;
		else return available;
	}

	void setid(int this_id) {														// Set the id of the book
		id = this_id;
	}

	int getid() {																	// Get the id of a book
		return id;
	}

	void settitle(string this_title) {												// Set the book title
		title = this_title;
	}

	string gettitle() {																// Get the title of a book.
		return title;
	}

	string getauthor() {															// Get the book author
		return author;	
	}

	int getborrower() {																// Get the borrower id of a book
		return borrowed_by;
	}

	int load(int this_id = -1) {
		if (this_id == -1) this_id = id;										// Set default id if no
		ifstream file("Data\\Books\\" + to_string(this_id));
		if (file.is_open()){
			string temp[6];
			for (int i = 0; i < 6; i++)	getline(file, temp[i], '|');
			istringstream stemp(temp[0]);										// Use to convert string to int
			stemp >> id;														// Convert string to int
			title = temp[1];
			if (temp[2] == "1") available = true;
			else if (temp[2] == "0") available = false;
			author = temp[3];
			istringstream stemp2(temp[4]);										// Use to convert string to int
			stemp2 >> borrowed_by;														// Convert string to int
			location = temp[5];
			file.close();
			return 0;
		}
		else {
			id = -1;
			title = "NOT FOUND";
			return 1;
		}
	}

	int rem(int this_id = -1) {													// Remove book info from database
		string filename = "Data\\Books\\" + to_string(this_id);
		const char *cfilename = filename.c_str();
		return remove(cfilename);
	}

	bool isExist(int this_id = -1) {											// Check if book exists based on given book id or current book id
		if (this_id == -1) this_id = id;
		ifstream pfile("Data\\Books\\" + to_string(this_id));
		if (pfile.is_open()) {
			pfile.close();
			return true;
		}
		pfile.close();
		return false;
	}

	int save() {
		int ret_value = isExist(id);
		ofstream file("Data\\Books\\" + to_string(id), ofstream::trunc);											// Open the file specifically for each student
		file << id << '|' << title.c_str() << '|' << available << '|' << author.c_str() << '|' << borrowed_by << '|' << location;		// Write all the data into the file
		file.close();
		return ret_value;
	}
};

// Declare as global book object 
books book;													// book declared first before students because the students uses the book object inside students class

// Basic class students to store all the data
class students {
private:
	int id = -1, age = -1;															// Student ID,
	int no_of_books_borrowed = 0;													// How many books student borrow
	int book_id_borrowed[MAX_BOOK_PER_STUDENTS];									// ID of the books that student borrow
	string name = "ID NOT EXISTS OR INVALID";										// Student name
	string program = "none";

public:
	students() 	{}
	~students() {}

	string id2name(int this_id = -1) {												// Return the name of a student based on given student id
		if (this_id != -1 || (this_id > -1 && id != 1)) {							// If the Id is legit, then continue
			int temp_id = id;														// Store id to temporary variable
			string temp_name;
			if (load(this_id) != 1){												// Load the data from given id
				temp_name = name;													// save the current name to temporary variable for use in return
				load(temp_id);														// Load back the current book id to avoid data error/mixed up
				return temp_name;
			}
			else {
				load(temp_id);
				return "-1";
			}
		}
		else return "-";
	}

	int getage() {																	// Get the age of a student
		return age;
	}

	string getprogram() {															// Get student program
		return program;
	}

	void setage(int i) {															// Set the age
		age = i;														
	}

	void setprogram(string s) {														// Set program
		program = s;
	}

	bool return_book(int book_id = -1) {
		if (book_id == -1) book_id = id;
		bool deleted = false;														// As one of the method to rearrange the array (sort)
		for (int i = 0; i < MAX_BOOK_PER_STUDENTS; i++) {
			if (deleted == true) {													// Same as above	
				if (!((i + 1) > MAX_BOOK_PER_STUDENTS)) book_id_borrowed[i - 1] = book_id_borrowed[i];	// If the "i" is not the last (e.g: 1 is the last number in this example), it will run this command. This command is use to rearrage the array by swapping values
				book_id_borrowed[i] = 0;											// Set current array to 0
			}

			if (book_id_borrowed[i] == book_id) {									// Delete the data in the array
				book_id_borrowed[i] = 0;											// Set null (0) to the array
				no_of_books_borrowed--;
				deleted = true;
			}
		}
		return deleted;
	}

	void resetData() {																// Reset all the properties in this class to its initial value
		no_of_books_borrowed = 0;
		for (int i = 0; i< MAX_BOOK_PER_STUDENTS; i++){
			book_id_borrowed[i] = 0;
		}
	}

	bool checkBook(int book_id = -1) {												// Check the whether student borrow the book or not
		if (book_id == -1) book_id = id;
		if (no_of_books_borrowed > 0) {												// If student borrow book more than 0, then continue
			for (int i = 0; i < MAX_BOOK_PER_STUDENTS; i++) {
				if (book_id_borrowed[i] == book_id) return true;					// Check the status of the book based on book id
			}
			return false;
		}
		else return false;
	}

	bool issue(int book_id = -1) {													// Let the student borrow a book
		if (!(no_of_books_borrowed + 1 > MAX_BOOK_PER_STUDENTS)) {
			if (book_id == -1) book_id = id;										
			if (book_id != -1 && book_id > 0) {										// Check the book if the book id valid
				book_id_borrowed[no_of_books_borrowed] = book_id;					// Set the array of the book id
				no_of_books_borrowed++;												
				return true;
			}
			else return false;
		}
		else return false;
	}

	int get_no_of_book() {															// Get the number of book student borrow
		return no_of_books_borrowed;
	}

	string get_borrowed_book_list() {												// Print out the IDs of book borrowed
		if (no_of_books_borrowed > 0) {
			string temp = "";
			for (int i = 0; i < no_of_books_borrowed; i++){
				temp += " " + to_string(i+1) + ". " + book.id2book(book_id_borrowed[i]) + " (ID: " + to_string(book_id_borrowed[i]) + ")\n";
			}
			return temp;
		}
		else return " -";
	}

	void setname(string this_name) {												// Set the name of student
		name = this_name;
	}

	string getname() {																// Get the name
		return name;
	}

	void setid(int this_id) {														// ID
		id = this_id;
	}

	int getid() {																	// Get the student id
		return id;
	}

	bool isExist(int this_id = -1) {												// Check if student data exists or not
		if (this_id == -1) this_id = id;
		ifstream dfile("data\\students\\" + to_string(this_id));					// Open the file and check the file
		if (dfile.is_open()) {
			dfile.close();
			return true;
		}
		else {
			dfile.close();
			return false;
		}
	}

	int load(int this_id = -1) {													// Read student data from the file based on its ID
		if (this_id == -1) this_id = id;											// Set default id if no
		ifstream file("data\\students\\" + to_string(this_id));
		if (file.is_open()){
			string temp[7];
			//string a;
			for (int i = 0; i < 7; i++) {
				getline(file, temp[i], '|');										// Get the first data in the file
			}
			for (int i = 0; i < 2+MAX_BOOK_PER_STUDENTS; i++) {
				istringstream stemp(temp[1+i]);											// Use to convert string to int
				if (i == 0) stemp >> id;												// Convert string to int
				if (i == 1) stemp >> no_of_books_borrowed;
				if (i > 1) stemp >> book_id_borrowed[i - MAX_BOOK_PER_STUDENTS];
			}
			istringstream stemp(temp[5]);
			stemp >> age;
			program = temp[6];
			name = temp[0];
			file.close();
			return 0;
		}
		else {
			id = -1;
			name = "NOT FOUND";
			no_of_books_borrowed = 0;
			file.close();
			return 1;
		}
		return 0;
	}

	int rem(int this_id = -1) {																// Remove file according to student ID
		string filename = "Data\\Students\\" + to_string(this_id);
		const char *cfilename = filename.c_str();											// CHange the filename to const char pointer
		return remove(cfilename);
	}

	int save() {																			// Save all the information in a file named as its ID
		bool a = isExist(id);
		ofstream file("Data\\Students\\" + to_string(id), ofstream::trunc);
		file << name.c_str() << '|' << id << '|' << no_of_books_borrowed;
		for (int i = 0; i < MAX_BOOK_PER_STUDENTS; i++) {
			file << '|' << book_id_borrowed[i];
		}
		file << '|' << age << '|' << program;
		file.close();
		return a;
	}
};
// Declare as global student object. So that other functions can use this objects without passing its value one by one
students student;

// Functions **********************************************************************************************************
// Main function
int main() {
	SetConsoleTitle(L"Library System");										// Set the console title
	HideScrollBar();														// Hide scroll bar of the console program
	SetCursorSize(false);													// Hide the blinking cursor from the console
	Intro();																// The welcome screen
	MakeDirectories();														// Check and make directories if not exists
	MainMenu();																// Go to the main menu
	return 0;
}

// Main menu-related functions ***************************************************************************************
void IssueBook() {
	int student_id, book_id;
	ClearScreen();
	SetMenuTitle("Issue a book");
	SetPos(0, 4);
	cout << " Enter book ID: ";
	print_tips("Enter the book ID. Press ESC to go back.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	book_id = string2int(xcin(10, false));								// Using custom cin, get the string of the id and convert it to int.
	if (esc_pressed) { esc_pressed = false; MainMenu(); }
	nbsp;																// For new line in the console. Shortcut ;)
	setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);

	if (book.load(book_id) != 1) {										// If the book valid
		if (book.isAvailable()) {										// book is available
			setcolor();
			cout << " Enter student ID: ";
			print_tips("Enter the student ID.");
			setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			student_id = string2int(xcin(10,false));					// Get the input from the user
			if (esc_pressed) { esc_pressed = false; MainMenu(); }		// If user press ESC during the input, return to main menu
			nbsp;
			setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);
			if (student.load(student_id) != 1) {						// Load student info based on id
				if (student.issue(book_id)) {							// If student able to issue a book
					student.save();										// Save the data to file
					book.toggleAvailable();								// Toggle book to unavaiable
					book.setborrower(student_id);						// Set the borrower id with the student id
					book.save();										// Save book to the database
					setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					cout << " Status: OK\n\n";
				}
				else cout << " Status: Max book limit reach. Return book before issue another\n\n";
			}
			else cout << " Status: Student data not exist\n\n";
		}
		else cout << " Status: Book not available\n\n";
	}
	else cout << " Status: Book not exist\n\n";
	setcolor();
	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	MainMenu();
}

void ReturnBook() {
	int student_id, book_id;
	ClearScreen();
	SetMenuTitle("Return a book");
	SetPos(0, 4);
	cout << " Enter student ID: ";
	print_tips("Enter the student ID. Press ESC to go back.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	student_id = string2int(xcin(10, false));
	if (esc_pressed) { esc_pressed = false; MainMenu(); }
	nbsp;
	setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);

	if (student.load(student_id) != 1) {
		if (student.get_no_of_book() > 0) {
			setcolor();
			cout << "\n Book issued: \n" << student.get_borrowed_book_list() << "\n Enter book ID to return: ";
			print_tips("Enter the book ID.");
			setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			book_id = string2int(xcin(10, false));
			if (esc_pressed) { esc_pressed = false; MainMenu(); }
			nbsp;
			setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);
			if (student.checkBook(book_id)) {
				if (student.return_book(book_id)) {
					student.save();
					if (book.load(book_id) != -1) {
						book.toggleAvailable();
						book.setborrower(-1);
						book.save();
						setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
						cout << " Status: OK\n\n";
					}
					else cout << " Status: Error accessing book database\n\n";
				}
				else cout << " Status: Unable to continue. This book is not found in student data\n\n";
			}
			else cout << " Status: Invalid book ID or student does not issued it\n\n";
		}
		else cout << " Status: This student does not borrow any books\n\n";
	}
	else cout << " Status: Unable to load student data\n\n";
	setcolor();
	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	MainMenu();
}

void MainMenu() {
	unsigned int temp_index;
	vector<string> menu_item = { "Issue a book", "Return a book", "Students Management", "Book Management", "", "Exit"};			// Create a dynamic menu. Built from scratch :)

	ClearScreen();
	print_tips("(C) 2014 Group 7 - Use the arrow keys (Up/Down) to move. ENTER to select.");
	SetMenuTitle();

	do {									
		CreateMenu(menu_item, 1, 4, main_menu_selection, &temp_index);
		if (main_menu_selection == 0) IssueBook();
		else if (main_menu_selection == 1) ReturnBook();
		else if (main_menu_selection == 2) StudentMenu();
		else if (main_menu_selection == 3) BookMenu();
		else if (main_menu_selection == 5) break;
		else if (main_menu_selection == 270000) {
			int msg;
			msg = MessageBox(0, L"Are you sure to exit this program?", L"LibSys", MB_YESNO | MB_ICONQUESTION);
			if (msg == IDYES) break;
			else {
				main_menu_selection = temp_index;
				continue;
			}
		}
		else {
			main_menu_selection = temp_index;						// Prevent menu from resetting to first option
			continue;
		}
	} while (1);
	exit(0);
}

// Student-related functions *****************************************************************************************
// Add or edit student data file
void AddNewStudent() {
	int student_id, age, mb;
	string student_name, prog;
	ClearScreen();
	SetMenuTitle("Students Management Menu - Add/Edit Student");
	SetPos(0, 4);
	cout << " Student ID\t: ";
	print_tips("Please enter the Student ID in numbers. Press ESC to go back");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	student_id = string2int(xcin(10, false));
	if (esc_pressed) { esc_pressed = false; StudentMenu(); }
	nbsp;
	setcolor();

	if (student.isExist(student_id)) {
		student.load(student_id);
		print_tips("Student data already exist in the database.", 2);
		if (student.get_no_of_book() < 1) {
			mb = MessageBox(NULL, L"Student data already exist.\nDo you want to replace?", L"LibSys", MB_YESNO | MB_ICONEXCLAMATION);		// SHow the messagebox
			if (mb == IDNO) AddNewStudent();
		}
		else {
			mb = MessageBox(NULL, L"Student data already exist. To edit, student must return all books first.", L"LibSys", MB_OK | MB_ICONINFORMATION);
			AddNewStudent();
		}
	}

	cout << " Name\t\t: ";
	print_tips("Please enter the student's name.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	student_name = xcin(60);
	if (esc_pressed) { esc_pressed = false; StudentMenu(); }
	nbsp;
	setcolor();
	cout << " Age\t\t: ";
	print_tips("Please enter the student's age.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	age = string2int(xcin(3, false));
	if (esc_pressed) { esc_pressed = false; StudentMenu(); }
	nbsp;
	setcolor();
	cout << " Program\t: ";
	print_tips("Please enter the student's program.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	prog = xcin(60);
	if (esc_pressed) { esc_pressed = false; StudentMenu(); }
	nbsp;

	setcolor();

	mb = MessageBox(NULL, L"Save book info in the database?", L"LibSys", MB_YESNO | MB_ICONINFORMATION);
	if (mb == IDNO) StudentMenu();

	student.resetData();												// Reset and save all students data to the database
	student.setid(student_id);
	student.setname(student_name);
	student.setage(age);
	student.setprogram(prog);

	if (!student.save()) print_tips("Student data successfully added to the database.");
	else print_tips("Old student data already exist and was replaced by the new one.", 2);

	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	StudentMenu();
}

void GetStudentInfo(bool menu, int id) {
	int student_id;
	ClearScreen();
	SetMenuTitle("Students Management Menu - Get Student Info");
	print_tips("Enter the student ID. Press ESC to go back.");
	if (id == -1) {
		SetPos(0, 4);
		cout << " Enter student ID: ";
		setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		student_id = string2int(xcin(10, false));
		if (esc_pressed) { esc_pressed = false; StudentMenu(); }
		nbsp;
		setcolor();
	}
	else student_id = id;

	student.load(student_id);
	if (!student.isExist(student_id)) MessageBox(NULL, L"Student data not exist!", L"LibSys", MB_OK | MB_ICONASTERISK);
	else {
		ClearScreen();
		student.load(student_id);
		SetMenuTitle("Students Management Menu - Get Student Info");
		SetPos(0, 4);
		cout << " Student ID\t\t: " << student.getid() << "\n Student Name\t\t: " << student.getname() << "\n Age\t\t\t: " << student.getage() << "\n Program\t\t: " << student.getprogram() << "\n Books issued\t\t: " << student.get_no_of_book() << "\n\n Books issued: \n" << student.get_borrowed_book_list();
	}
	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	if (menu == true) StudentMenu();
	else ShowAllStudents();
}

void DeleteStudent() {
	int student_id, err, mb;
	ClearScreen();

	SetMenuTitle("Students Management Menu - Delete Student");
	SetPos(0, 4);
	cout << " Enter Student ID: ";
	print_tips("Enter the student ID. Press ESC to go back.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	student_id = string2int(xcin(10, false));
	if (esc_pressed) { esc_pressed = false; StudentMenu(); }
	nbsp;
	setcolor();

	if (student.isExist(student_id)) {
		student.load(student_id);
		if (student.get_no_of_book() < 1) {
			mb = MessageBox(NULL, L"Student data already exist.\nDo you want to delete?", L"LibSys", MB_YESNO | MB_ICONEXCLAMATION);
			if (mb == IDNO) StudentMenu();
		}
		else {
			mb = MessageBox(NULL, L"Student issued some books. To delete, student must return all books first.", L"LibSys", MB_OK | MB_ICONINFORMATION);
			StudentMenu();
		}
	}

	err = student.rem(student_id);											// If fail to save, the program will print error. 
	if (err == 0) {
		setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		cout << " Status: Delete completed\n\n";
	}
	else {
		setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << " Status: Data not exist or invalid input. Please try again.\n\n";
	}
	setcolor();
	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	StudentMenu();
}

void ShowAllStudents() {
	unsigned int temp_index;
	vector<int> student_ids;
	vector<string> str_student_ids, menu_item;
	WIN32_FIND_DATA FindFileData;												// A find data class, to store find items
	HANDLE hFind;																// handle to find 
	TCHAR filename[] = TEXT("Data\\Students\\*.*");

	ClearScreen();
	SetMenuTitle("Students Management Menu - Show All Students");
	SetPos(0, 4);
	print_tips("Use arrow keys to move or ENTER to get details. Press ESC to go back.");

	hFind = FindFirstFile(filename, &FindFileData);								// Start the find 
	if (hFind == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("INVALID HANDLE\nThis program will skip this function."), TEXT("LibSys"), MB_OK | MB_ICONSTOP);
		StudentMenu();
	}

	// Find the next file. tccmp is to compare two wide char (unicode) format.
	// If the function found valid file (not valid data, this require further check and prolonged the codes)
	// it will use the file name as id and put it in a vector
	// The vector will used further to determine the size and students data
	while (FindNextFile(hFind, &FindFileData)) if (_tccmp(FindFileData.cFileName, TEXT("..")) || _tccmp(FindFileData.cFileName, TEXT("."))) student_ids.push_back(_ttoi(FindFileData.cFileName));
	
	if (student_ids.size() > 0) {
		sort(student_ids.begin(), student_ids.end());									// BUbble sort the ids of students
		unsigned int mod = student_ids.size() % 17, max_page;							// Get the maximum page by calculating the size
		if (mod != 0) max_page = (int)(ceil(student_ids.size() / 17)) + 1;
		else max_page = student_ids.size() / 17;

		while (1) {																		// Loop until a valid exit called. Else, run the loop to redraw the list
			ClearLine(4, 17);															// Clear the specific line in the console
			if (!str_student_ids.size()){
				for (unsigned int i = index_page; i < index_page + 17; i++) {
					if (i < student_ids.size()) {
						student.load(student_ids[i]);									// Load the studend id based on valid student id list (students_ids)
						str_student_ids.push_back(to_string(i + 1) + ".\t" + student.getname() + " [ID: " + to_string(student_ids[i]) + "]"); // Format nicely the data to be shown to menu_item vector. This will be used to create menu
					}
				}
			}
			SetPos(1, 22);
			setcolor(7);
			printf_s("Page: %i/%i", current_page, max_page);					// Print page number
			setcolor();
			CreateMenu(str_student_ids, 1, 4, list_index, &temp_index);						// Create menu based on the menu_item vector

			// Below this is a logic operator to control the key press when user pressed
			if (list_index == 432100) {											// left key
				if (current_page > 1) {
					index_page -= 17;
					current_page--;
					str_student_ids.clear();
					continue;
				}
				else list_index = temp_index;
			}
			else if (list_index == 678900) {									// right key
				if (current_page < max_page) {
					index_page += 17;
					current_page++;
					str_student_ids.clear();
					continue;
				}
				else list_index = temp_index;
			}
			else if (list_index == 270000) {									// esc key
				current_page = 1;
				index_page = 0;
				list_index = 0;
				StudentMenu();
			}
			else {																// Enter key
				GetStudentInfo(false, student_ids[17 * (current_page - 1) + list_index]);
			}
		}
	}
	else {
		SetPos(1, 4);
		cout << "Empty.";
		SetPos(1, 24);
		PressAnyKey("Press any key to go back...");
		StudentMenu();
	}
}

void StudentMenu() {
	unsigned int temp_index;
	vector<string> menu_item = { "Add / Edit a student","Delete a student","Get student info","Show all students","","Return to Main menu" };

	ClearScreen();
	SetMenuTitle("Students Management Menu");

	do {
		CreateMenu(menu_item, 1, 4, student_menu_selection, &temp_index);
		if (student_menu_selection == 0) AddNewStudent();
		else if (student_menu_selection == 1) DeleteStudent();
		else if (student_menu_selection == 2) GetStudentInfo();
		else if (student_menu_selection == 3) {
			index_page = 0;
			current_page = 1;
			list_index = 0;
			ShowAllStudents();
		}
		else if (student_menu_selection == 5) {
			student_menu_selection = 0;
			MainMenu();
		}
		else if (student_menu_selection == 270000) break;
		else {
			student_menu_selection = temp_index;
			continue;
		}
	} while (1);
	MainMenu();
}

// Since the book functions almost similar with students, so comments are not written below. Just the variables, text,
// menu are changed plus few addition that will be commented.
// Book-related functions ********************************************************************************************
void AddNewBook() {
	int book_id, mb;
	string book_title, book_author, book_location;

	book.reset();															// reset the book object to its default properties values

	ClearScreen();
	SetMenuTitle("Books Management Menu - Add/Edit a book");
	SetPos(0, 4);

	cout << " Book ID\t: ";
	print_tips("Please enter the Book ID in numbers. Press ESC to go back.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	book_id = string2int(xcin(10,false));
	if (esc_pressed) { esc_pressed = false; BookMenu(); }
	nbsp;
	setcolor();

	if (book.isExist(book_id)) {
		book.load(book_id);
		print_tips("Book data already exist in the database.", 2);
		if (book.isAvailable()) {
			mb = MessageBox(NULL, L"Book already exist.\nDo you want to replace?", L"LibSys", MB_YESNO | MB_ICONEXCLAMATION);
			if (mb == IDNO) AddNewBook();
		}
		else {
			MessageBox(NULL, L"Book data already exist. To edit, the book issued must be returned.", L"LibSys", MB_OK | MB_ICONINFORMATION);
			AddNewBook();
		}
	}

	cout << " Book Title\t: ";
	print_tips("Please enter the title.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	book_title = xcin(60);
	if (esc_pressed) { esc_pressed = false; BookMenu(); }
	nbsp;
	setcolor();
	cout << " Book Author\t: ";
	print_tips("Please enter the book author.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	book_author = xcin(60);
	if (esc_pressed) { esc_pressed = false; BookMenu(); }
	nbsp;
	setcolor();
	cout << " Book Location\t: ";
	print_tips("Please enter the location of the book.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	book_location = xcin(60);
	if (esc_pressed) { esc_pressed = false; BookMenu(); }
	nbsp;
	setcolor();

	// Copy all the book information to the book class in order to prepare to save.
	book.setid(book_id);
	book.settitle(book_title);
	book.setauthor(book_author);
	book.setbooklocation(book_location);
	book.setAvailable(true);

	mb = MessageBox(NULL, L"Save book info in the database?", L"LibSys", MB_YESNO | MB_ICONINFORMATION);
	if (mb == IDNO) BookMenu();
	if (!book.save()) print_tips("Book data successfully added to the database.");
	else print_tips("Old book data already exist and was replaced by the new one.",2);
	SetPos(1, 24);
	PressAnyKey("Press any key to go back...");
	BookMenu();
}

void DeleteBook() {
	int book_id, err, mb;
	ClearScreen();
	SetMenuTitle("Books Management Menu - Delete Book");
	SetPos(0, 4);
	cout << " Enter book ID: ";
	print_tips("Enter the book ID. Press ESC to go back.");
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	book_id = string2int(xcin(10,false));
	if (esc_pressed) { esc_pressed = false; BookMenu(); }
	nbsp;

	if (book.isExist(book_id)) {
		book.load(book_id);
		//print_tips("Book data already exist in the database.", 2);
		if (book.isAvailable()) {
			mb = MessageBox(NULL, L"Book already exist.\nAre you sure to delete this?", L"LibSys", MB_YESNO | MB_ICONEXCLAMATION);
			if (mb == IDNO) BookMenu();
		}
		else {
			MessageBox(NULL, L"Someone issued this book. To delete, this book must be returned first.", L"LibSys", MB_OK | MB_ICONINFORMATION);
			BookMenu();
		}
	}

	err = book.rem(book_id);
	if (err == 0) cout << " Status: Delete completed\n\n";
	else {
		setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		MessageBox(NULL, L"Book not exist!", L"LibSys", MB_OK | MB_ICONASTERISK);
		cout << " Status: Data not exist or invalid input. Please try again.\n\n";
	}
	setcolor();
	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	BookMenu();
}

// Print out the information in the stored files at the screen
void GetBookInfo(bool menu, int id) {
	int book_id;
	ClearScreen();
	SetMenuTitle("Books Management Menu - Get Book Info");
	print_tips("Enter the book ID. Press ESC to go back.");
	if (id == -1) {
		SetPos(0, 4);
		cout << " Enter book ID: ";
		setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		book_id = string2int(xcin(10, false));
		if (esc_pressed) { esc_pressed = false; BookMenu(); }
		nbsp;
		setcolor();
	}
	else book_id = id;

	book.load(book_id);

	if (!book.isExist(book_id)) MessageBox(NULL, L"Book not exist!", L"LibSys", MB_OK | MB_ICONASTERISK);
	else {
		ClearScreen();
		SetMenuTitle("Books Management Menu - Get Book Info");
		SetPos(0, 4);
		cout << " Book ID\t: " << book.getid() << "\n Book Name\t: " << book.gettitle() << "\n Book Author\t: " << book.getauthor() << "\n Location\t: " << book.getbooklocation() << "\n Available\t: " << transbool(book.isAvailable()) << "\n Issued by\t: " << student.id2name(book.getborrower()) << " (ID: " << book.getborrower() << ")\n\n";
	}
	SetPos(0, 24);
	PressAnyKey(" Press any key to go back...");
	if (menu == true) BookMenu();
	else ShowAllBooks();
}

// Using Windows built-in function to search for files that store students and books information
void ShowAllBooks() {
	unsigned int temp_index;
	vector<int> book_ids;
	vector<string> str_book_ids;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	TCHAR filename[] = TEXT("Data\\Books\\*.*");

	ClearScreen();
	SetMenuTitle("Books Management Menu - Show All Books");
	SetPos(0, 4);
	print_tips("Use arrow keys to move or ENTER to get details. Press ESC to go back.");

	hFind = FindFirstFile(filename, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("INVALID HANDLE\nThis program will skip this function."), TEXT("LibSys"), MB_OK | MB_ICONSTOP);
		StudentMenu();
	}

	while (FindNextFile(hFind, &FindFileData)) if (_tccmp(FindFileData.cFileName, TEXT("..")) || _tccmp(FindFileData.cFileName, TEXT("."))) book_ids.push_back(_ttoi(FindFileData.cFileName));
	
	if (book_ids.size() > 0){
		sort(book_ids.begin(), book_ids.end());
		unsigned int mod = book_ids.size() % 17, max_page;
		if (mod != 0) max_page = (int)(ceil(book_ids.size() / 17)) + 1;
		else max_page = book_ids.size() / 17;

		while (1) {
			ClearLine(4, 17);
			if (!str_book_ids.size()){
				for (unsigned int i = index_page; i < index_page + 17; i++) {
					if (i < book_ids.size()) {
						book.load(book_ids[i]);
						str_book_ids.push_back(to_string(i + 1) + ".\t" + book.gettitle() + " [ID: " + to_string(book_ids[i]) + ", Available: " + transbool(book.isAvailable()) + "]");
					}
				}
			}
			SetPos(1, 22);
			setcolor(7);
			printf_s("Page: %i/%i", current_page, max_page);
			setcolor();
			CreateMenu(str_book_ids, 1, 4, list_index, &temp_index);
			if (list_index == 432100) {											// left key
				if (current_page > 1) {
					index_page -= 17;
					current_page--;
					str_book_ids.clear();
					continue;
				}
				else list_index = temp_index;
			}
			else if (list_index == 678900) {									// right key
				if (current_page < max_page) {
					index_page += 17;
					current_page++;
					str_book_ids.clear();
					continue;
				}
				else list_index = temp_index;
			}
			else if (list_index == 270000) {									// esc key
				current_page = 1;
				index_page = 0;
				list_index = 0;
				BookMenu();
			}
			else {																// Enter key
				GetBookInfo(false, book_ids[17 * (current_page - 1) + list_index]);
			}
		}
	}
	else {
		SetPos(1, 4);
		cout << "Empty.";
		SetPos(1, 24);
		PressAnyKey("Press any key to go back...");
		BookMenu();
	}
}

// Book menu
void BookMenu() {
	unsigned int temp_index;
	vector<string> book_menu_item = { "Add / Edit a book","Delete a book","Get book info","Show all books","","Return to Main menu"};

	ClearScreen();
	SetMenuTitle("Books Management Menu");

	do {
		CreateMenu(book_menu_item, 1, 4, book_menu_selection, &temp_index);
		if (book_menu_selection == 0) AddNewBook();
		else if (book_menu_selection == 1) DeleteBook();
		else if (book_menu_selection == 2) GetBookInfo();
		else if (book_menu_selection == 3) ShowAllBooks();
		else if (book_menu_selection == 5) {
			book_menu_selection = 0;
			MainMenu();
		}
		else if (book_menu_selection == 270000) break;
		else {
			book_menu_selection = temp_index;
			continue;
		}
	} while (1);
	MainMenu();
}


// Other functions **************************************************************************************************
// Create directories Data, Students and Books. Using windows built-in function.
// Taken from MSDN CreateDirectory example page with modifications
void MakeDirectories() {
	TCHAR Buffer[BUFSIZE];
	DWORD dwRet;

	dwRet = GetCurrentDirectory(BUFSIZE, Buffer);
	if (dwRet == 0)
	{
		printf("GetCurrentDirectory failed (%d)\n", GetLastError());
		exit(113);																		// Exit program with code 113 - Fail to get current directory
	}
	if (dwRet > BUFSIZE)
	{
		printf("Buffer too small; need %d characters\n", dwRet);
		exit(114);																		// Exit program with code 114 - Number of dimension of TCHAR array exceeded, require more.
	}

	// Create directories/folders acccording to its name
	_tcscat_s(Buffer, TEXT("\\Data"));													// Combine the current path of program (e.g: c:\test + \data = c:\test\data)
	CreateDirectory(Buffer, NULL);														// Create the directory if not exits. Else, it will skip.
	if (!SetCurrentDirectory(Buffer)) {													// Try to access created folder, if cannot the program will exit. 
		MessageBox(0, TEXT("Unable to access Data folder. This program will exit."), TEXT("LibSys"), MB_OK | MB_ICONEXCLAMATION);			// Show messagebox to inform the error.
		exit(110);																		// Exit program with code 110 - Data folder no access
	}

	_tcscat_s(Buffer, TEXT("\\Students"));
	CreateDirectory(Buffer, NULL);
	if (!SetCurrentDirectory(Buffer)) {
		MessageBox(0, TEXT("Unable to access Students folder. This program will exit."), TEXT("LibSys"), MB_OK | MB_ICONEXCLAMATION);
		exit(111);																		// Exit program with code 111 - Students folder no access
	}

	_tcscat_s(Buffer, TEXT("\\..\\Books"));
	CreateDirectory(Buffer, NULL);
	if (!SetCurrentDirectory(Buffer)) {
		MessageBox(0, TEXT("Unable to access Books folder. This program will exit."), TEXT("LibSys"), MB_OK | MB_ICONEXCLAMATION);
		exit(112);																		// Exit program with code 110 - Books folder no access
	}

	_tcscat_s(Buffer, TEXT("\\..\\..\\"));
	if (!SetCurrentDirectory(Buffer)) {
		MessageBox(0, TEXT("Unable to change directory to default. This program will exit."), TEXT("LibSys"), MB_OK | MB_ICONEXCLAMATION);
		exit(115);																		// Exit program with code 115 - Unable to change to default directory
	}
}

// ClearScreen function. Same as system("cls") but this function is more secure. Taken from http://www.cplusplus.com/articles/4z18T05o/
void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void SetPos(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(cBuffer, pos);								// Move the console cursor to desired points
}

void AnimateString(int x, int y, string s) {
	int nrand;
	srand((unsigned int)time(NULL));
	const char *def = s.c_str();
	for (unsigned int i = 0; i < s.length(); i++) {
		for (unsigned int o = 0; o < 3; o++) {
			SetPos(x + i, y);
			nrand = rand() % 60 + 65;
			cout << char(nrand);
			Sleep(10);
		}
		SetPos(x + i, y);
		cout << *(def + i);
	}
}

void setcolor(WORD w) { SetConsoleTextAttribute(cBuffer, w); return; }

void Intro() {
	setcolor();
	setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	AnimateString(25, 9, "Welcome To Library System");
	setcolor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	AnimateString(33, 10, "by group 7");
	Sleep(1000);
	setcolor();
	AnimateString(51, 24, "Press any key to continue...");
	PressAnyKey("");
	ClearScreen();
}

void HideScrollBar() {
	COORD cSize = { 80, 25 };
	int Status = SetConsoleScreenBufferSize(cBuffer, cSize);			// Change the console cursor size
}

// A custom built menu functions to create dynamic menu that can be select using arrow keys with additional key supported.
void CreateMenu(vector<string> &menu_item, int x, int y, unsigned int &index, unsigned int *current_index) {
	int key;
	if (index > menu_item.size() || index < 0) {
		index = 0;
		if (menu_item[index] == "") {
			do {
				index++;
			} while (menu_item[index] == "");
		}
	}

	// loop forever until a specific menu item selected...
	while (1) {
		// Redrawing the menu
		for (unsigned int i = 0; i < menu_item.size(); i++) {
			if (i == index) {
				SetPos(x, y + i);
				setcolor(BACKGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				cout << menu_item[i];
				setcolor();
			}
			else {
				SetPos(x, y + i);
				cout << menu_item[i];
			}
		}

		key = _gettch();
		switch (key) {
		case 80:										// DOWN
			if (index < menu_item.size() - 1) index++;
			else if (index == menu_item.size() - 1) index = 0;
			if (menu_item[index] == "") {
				do {
					index++;
					if (index > menu_item.size() - 1) index = 0;
				} while (menu_item[index] == "");
			}
			break;

		case 72:										// UP
			if (index > 0) index--;
			else if (index == 0) index = menu_item.size() - 1;
			if (menu_item[index] == "") {
				do {
					index--;
					if (index > menu_item.size() - 1) index = menu_item.size() - 1;			//since index is unsigned int, it will not be below 0. but if we force it to below 0, it will show random large number
				} while (menu_item[index] == "");
			}
			break;

		case 13:
			if (current_index) *current_index = index;
			return;
			//break;

		case 77:														// right
			if (current_index) *current_index = index;
			index = 678900;
			return;
			//break;

		case 75:														//left
			if (current_index) *current_index = index;
			index = 432100;
			return;
			//break;

		case 27:														// ESC
			index = 270000;
			return;
			//break;

		default:
			break;
		}
	}
}

void SetCursorSize(bool visible, DWORD size)
{
	if (size == 0) size = 20;												// default cursor size Changing to numbers from 1 to 20, decreases cursor width
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = size;
	SetConsoleCursorInfo(cBuffer, &lpCursor);
}

void SetMenuTitle(string s) {
	const unsigned int size = s.size();
	setcolor(FOREGROUND_BLUE | FOREGROUND_GREEN);
	cout << char(201) << setfill(char(205)) << setw(size + 3) << char(187) << '\n';
	cout << char(186) << setfill(' ') << setw(size + 3) << char(186) << '\n';
	cout << char(211) << setfill(char(196)) << setw(size + 3) << char(208) << setw(80 - size - 3) << '\n';
	SetPos(2, 1);
	setcolor();
	cout << s;
}

// Using pointers to put the information about the cursor position
void GetPos(COORD *pos) {
	CONSOLE_SCREEN_BUFFER_INFO cInfo;
	GetConsoleScreenBufferInfo(cBuffer, &cInfo);
	if (pos) {
		pos->X = cInfo.dwCursorPosition.X;
		pos->Y = cInfo.dwCursorPosition.Y;
	}
	else return;
}

void print_tips(string s, int type) {
	COORD temp_pos;
	GetPos(&temp_pos);
	SetPos(0, 23);
	cout << setfill(' ') << setw(79) << " ";									// Clear the status screen
	SetPos(1, 23);
	if (type == 0) {															// Info - Green
		setcolor(FOREGROUND_GREEN);
	}
	else if (type == 1) {														// Error - RED
		setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (type == 2) {														// Warning - YELLOW
		setcolor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}

	printf_s("%s", s);
	setcolor();
	SetPos(temp_pos.X, temp_pos.Y);
}

// Super simplified version of PressAnyKey()
void PressAnyKey(string s) {
	cout << s;
	_gettch();
}

// Custom built function to get the string input from the user.
// This function eliminates the problem when cin.sync() is not used.
// So the program will be more stable and less bugs to be handled.
string xcin(unsigned int max_char, bool alphanumeric) {
	int key;
	vector<char> vkey;											// Using vector to store pressed key.

	esc_pressed = false;										// As a precaution measurement to set the esc_pressed to false
	while (1) {
		key = _gettch();
		if (key >= 32 && key <= 126){
			if (vkey.size() < max_char) {						// limit the character inputs
				if (alphanumeric || (!alphanumeric && (key >= 48 && key <= 57))) {			// Allow only specific characters to accept
					printf_s("%c", key);
					vkey.push_back(char(key));
				}
			}
		}
		else if (key == 13) {													// user press enter
			if (vkey.size() > 0){
				vkey.push_back('\0');												// end the vectors of characters with null so that it becomes valid vectors,
				vkey.clear();														// clear the array
				return string(vkey.data());											// convert the vectors of characters to string
			}
			else continue;
		}
		else if (key == 8) {													// Backspace function
			if (vkey.size() > 0) {
				printf_s("%c %c", char(8), char(8));
				vkey.pop_back();
			}
		}
		else if (key == 27) {													// user pressed esc
			esc_pressed = true;
			return "-1";
		}
	}
}

int string2int(string s) {
	int i;
	istringstream ss(s);														// using istringstream to easily convert string to integer
	ss >> i;																	// convert and copy the data to variable i
	ss.clear();																	// clear the variable ss
	return i;
}

void ClearLine(int start_y, int n_y) {
	SetPos(0, start_y);
	for (int i = 0; i <= n_y; i++) cout << setfill(' ') << setw(79) << '\n';	// Clear specific numbers of characters at specific y axis.
}

string transbool(bool this_bool) {
	if (this_bool == true) return "Yes";
	else if (this_bool == false) return "No";
	else return false;
}