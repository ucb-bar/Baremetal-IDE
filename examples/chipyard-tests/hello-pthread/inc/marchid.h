#ifndef __MARCHID_H
#define __MARCHID_H

const char *get_machine_architecture(size_t marchid) {
  switch (marchid) {
  case 1:
    return "rocket";
  case 2:
    return "sonicboom";
  case 5:
    return "spike";
  default:
    return "unknown";
  }
}

#endif  // __MARCHID_H
