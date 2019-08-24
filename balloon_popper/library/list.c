#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

struct list {
    void **list_items;
    size_t size_capacity;
    size_t current_size;
    FreeFunc free;
};

List *list_init(size_t initial_size, FreeFunc freer) {
    List *list = malloc(sizeof(List));
    assert(list);
    if (initial_size > 0) {
        list->list_items = malloc(initial_size * sizeof(void *));
        assert(list->list_items);
    }

    list->size_capacity = initial_size;
    list->current_size = 0;
    list->free = freer;

    return list;
}

void list_free(List *list) {
    assert(list);

    if (list->current_size > 0) {
        for (size_t i = 0; i < list->current_size; i++) {
            (list->free)(list->list_items[i]);
        }

        free(list->list_items);
    }

    free(list);
}

size_t list_size(List *list) {
    assert(list);
    return list->current_size;
}

void *list_get(List *list, size_t index) {
    assert(list);
    assert(index < list->current_size);

    return list->list_items[index];
}

void list_remove(List *list, size_t index) {
    assert(list);
    assert(list->current_size != 0 && index < list->current_size);
    void *to_remove = list->list_items[index];

    for (size_t i = index; i < list->current_size-1; i++) {
        list->list_items[i] = list->list_items[i+1];
    }

    list->current_size--;
    (list->free)(to_remove);
}

void list_set(List *list, size_t index, void *value) {
    assert(list);
    assert(index >=0 && index < list->current_size);
    // Free previously set item
    list->free(list->list_items[index]);
    list->list_items[index] = value;
}

void list_add(List* list, void* value) {
    assert(list);
    size_t current_capacity = list->size_capacity;
    size_t current_size = list->current_size;

    // If adding the first item, then simply malloc enough space for 1
    if (current_capacity == 0) {
            list->list_items = malloc(sizeof(void*));
            assert(list->list_items);
            list->size_capacity++;
    }
    // If capacity is size, then we need to reallocate memory
    else if (current_capacity == current_size) {
        // Double capacity each time we need more space
            void **new = realloc(list->list_items, sizeof(void*) *
                    2 * current_capacity);
            list->list_items = new;
            list->size_capacity *= 2;
    }
    list->list_items[current_size] = value;
    list->current_size++;
}
