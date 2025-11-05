#ifndef K_LIST_TESTS_H
#define K_LIST_TESTS_H

#include "list.h"

int TestRandom1         (list_t *list);
int TestRealloc         (list_t *list);
int TestDelete          (list_t *list);
int Test4               (list_t *list);
int TestBeforeFunctions (list_t *list);
int TestLoop            (list_t *list);

int TestBadLoop     (list_t *list);
int TestBadEdge     (list_t *list);
int TestBadIdxes    (list_t *list);
int TestVerificator ();

#endif // K_LIST_TESTS_H