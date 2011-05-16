#ifndef RESOURCES_H
#define RESOURCES_H 1

#include "types.h"

#define MAX_RESOURCES 0x1000

#define RESOURCE_TYPE_NONE 0            // Neegzistuojantis resursas.


struct Resource {

  // Visiems resursams bendri laukai.
  u64int type;                          // Resurso tipas – kiekvienam tipui
                                        // yra priskirta po unikalų numerį.
  bool reusable;                        // Ar resursas yra perpanaudojamas, 
                                        // ar jis po panaudojimo turėtų būti
                                        // sunaikintas.
  bool free;                            // Ar resursas yra užimtas ar 
                                        // laisvas? (Užimti gali būti tik
                                        // tie, kur reusable == true.)
  
  // Individualaus resurso informacija.
  u64int value[10];

  // Metodai.

  Resource() {
    this->type = RESOURCE_TYPE_NONE;
    this->reusable = false;
    this->free = false;
    };

  };


class ResourceManager {

private:

  // Atributai.

  Resource resource[MAX_RESOURCES];

  u64int last_used;                     // Paskutinio užimto resurso id.

  /**
   * Gražina indeksą pirmojo neužimto įrašo apie resursą.
   */
  u64int get_free_slot() {

    for (int i = 0; i < MAX_RESOURCES; i++) {
      if (this->resource[i].type == RESOURCE_TYPE_NONE) {
        return i;
        }
      }

    // FIXME: Turėtų mesti klaidą.

    return MAX_RESOURCES;
    }
  

public:

  // Metodai.

  };

#endif
