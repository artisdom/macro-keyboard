#ifndef KEYMAP_H
#define KEYMAP_H


#define LAYOUT( \
    K32,   K00, K01, K02, \
    K31,   K10, K11, K12, \
    K30,   K20, K21, K22  \
  ) { \
    { K00, K01, K02 }, \
    { K10, K11, K12 }, \
    { K20, K21, K22 }, \
    { K30, K31, K32 }  \
}


#endif