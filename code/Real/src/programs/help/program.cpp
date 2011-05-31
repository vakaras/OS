#include "stdio.h"
#define N 256


using namespace std;


int main() {

  stdout.write(
      "Naudojimas\n"\
      "==========\n\n"\
      "Persijungimas tarp ekranų:\n\n"\
      "+ F1 -- 1 ekranas (naudotojo)\n"\
      "+ F2 -- 2 ekranas (naudotojo)\n"\
      "+ F3 -- 3 ekranas (naudotojo)\n"\
      "+ F4 -- 4 ekranas (naudotojo)\n"\
      "+ F5 -- 5 ekranas (skerdimo)\n"\
      "+ F6 -- 6 ekranas (procesų sąrašas)\n\n"\
      "Naudotojo ekrane:\n\n"\
      "+ „tasks“ + <enter> -- parodo programų sąrašą\n"\
      "+ „run X“ + <enter> -- paleidžia X-ąją programą\n"\
      "+ <CTRL> -- ją nužudo\n\n"\
      "Skerdimo ekrane:\n\n"\
      "+ „kill X“ + <enter> -- nužudo X-ąjį procesą\n"
      );

  return 0;
  }
