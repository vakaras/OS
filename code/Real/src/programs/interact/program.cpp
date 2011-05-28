#include "stdio.h"
#define N 256


using namespace std;


int main() {

  //stdout.write("Koks tavo vardas?\n");
  
  //char name[N];
  //stdin.read_line(name, N);

  //stdout.write("Labas, ");
  //stdout.write(name);
  //stdout.write("!\n");

  while (true) {

    stdout.write("Choose action:\n");
    stdout.write("\t0. Write to file.\n");
    stdout.write("\t1. Read from file.\n");

    char action;
    stdin.read(&action);

    if (action == '0') {
      stdout.write("\nChosen action: Write to file.\n");
      stdout.write("Enter file name (digit):");
      char file_name;
      stdin.read(&file_name);
      if (!('0' <= file_name && file_name <= '9')) {
        stdout.write("\nWrong file name!\n");
        continue;
        }
      else {
        stdout.write("\nEnter line:\n");
        char buffer[N];
        stdin.read_line(buffer, N);
        StreamOut out(file_name - '0');
        out.write(buffer);
        stdout.write("Line was written to file.\n");
        }
      }
    else if (action == '1') {
      stdout.write("\nChosen action: Read from file.\n");
      stdout.write("Enter file name (digit):");
      char file_name;
      stdin.read(&file_name);
      if (!('0' <= file_name && file_name <= '9')) {
        stdout.write("\nWrong file name!\n");
        continue;
        }
      else {
        stdout.write("\nFile content:\n");
        char buffer[N];
        StreamIn in(file_name - '0');
        in.read(buffer, N);
        stdout.write(buffer);
        stdout.write("\nEnd of file.\n");
        }
      }

    }

  return 0;
  }
