/* HTTP Header Parser
 * Copyright(c) 2016 y2c2 */

#include <stdlib.h>
#include "httpparse.h"
#include "test_req_hdr.h"
#include "test_res_hdr.h"

int main(void)
{
    /* Initialize memory management */
    hp_allocator_set_malloc(malloc);
    hp_allocator_set_free(free);

    test_req_hdr();
    test_res_hdr();

    return 0;
}

