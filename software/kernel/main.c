//
// Copyright 2016 Jeff Bush
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "asm.h"
#include "kernel_heap.h"
#include "libc.h"
#include "slab.h"
#include "vm.h"

struct linked_node
{
    struct linked_node *next;
    int value;
};

MAKE_SLAB(node_slab, struct linked_node);

// XXX need a global variable for _end to be emitted correctly.
int foo;

void kernel_main()
{
    int i;
    int j;
    struct linked_node *node;
    struct linked_node *list = 0;
    struct vm_translation_map *new_map;

    vm_init();

    kprintf("Hello kernel land\n");

    // Map a page, then read and write to it
    for (j = 1; j < 30; j++)
    {
        for (i = 0; i < j; i++)
        {
            node = (struct linked_node*) slab_alloc(&node_slab);
            node->next = list;
            list = node;
            node->value = j;
        }


        for (i = 0; i < j - 1; i++)
        {
            node = list;
            list = list->next;
            slab_free(&node_slab, node);
        }
    }

    for (node = list; node; node = node->next)
        kprintf("%d ", node->value);

    kprintf("\n");

    new_map = new_translation_map();
    destroy_translation_map(new_map);

    // Start other threads
    *((volatile unsigned int*) 0xffff0100) = 0xffffffff;

//    *((unsigned int*) 1) = 1; // Cause fault
    for (;;)
        ;
}

void thread_n_main()
{
    kprintf("%c", __builtin_nyuzi_read_control_reg(0) + 'a');
    for (;;)
        ;
}

