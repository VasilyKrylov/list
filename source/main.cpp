#include <stdio.h>
#include <sys/stat.h>

#include "list.h"
#include "list_log.h"
#include "list_tests.h"
#include "debug.h"

int main()
{
    // list_t list;
    // LIST_CTOR (list, 10);

    int status = TestVerificator ();
    if (status == 0)
    {
        printf ("%s", "All verificator tests passed!\n");
    }
    else 
    {
        printf ("%s", "Not all verificator tests passed :(\n");
    }

    // ListDtor (&list);
}
