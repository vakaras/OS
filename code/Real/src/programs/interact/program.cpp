#include "stdio.h"
#define N 256


using namespace std;


int main() {

  while (true) {

    stdout.write("Choose action:\n");
    stdout.write("\t0. Write to file.\n");
    stdout.write("\t1. Read from file.\n");

    char action;
    stdin.read(&action);

    if (action == '0') {
      stdout.write("\nChosen action: Write to file.\n");
      stdout.write("Enter file name (digit):");
      u64int file_name;
      stdin.read(&file_name);
      stdout.write("\nFile name: ");
      stdout.write(file_name);
      stdout.write("\nEnter line:\n");
      char buffer[N];
      stdin.read_line(buffer, N);
      StreamOut out(file_name);
      out.write(buffer);
      stdout.write("Line was written to file.\n");
      }
    else if (action == '1') {
      stdout.write("\nChosen action: Read from file.\n");
      stdout.write("Enter file name (digit):");
      u64int file_name;
      stdin.read(&file_name);
      stdout.write("\nFile name: ");
      stdout.write(file_name);
      stdout.write("\nFile content:\n");
      char buffer[N];
      StreamIn in(file_name);
      in.read(buffer, N);
      stdout.write(buffer);
      stdout.write("\nEnd of file.\n");
      }

    }

  return 0;
  }
