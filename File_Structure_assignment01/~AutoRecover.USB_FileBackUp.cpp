#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, char* argv[]){
	if (argc != 2) {
		cout << "arg ERR";
	}

	string fPath_front = argv[0];
	string fPath_back = argv[1];

	fstream file_front;
	fstream file_back;

	file_front.open(fPath_front, fstream::in | fstream::out);

}