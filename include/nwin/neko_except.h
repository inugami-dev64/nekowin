#ifndef NEKO_EXCEPT_H
#define NEKO_EXCEPT_H

#include <stdlib.h>
#include <stdio.h>
#define except(_cond, _msg) if(!_cond) { fprintf(stderr, "libneko: %s\n", _msg); abort(); }

#endif