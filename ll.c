#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define FINAL_NODE (SIZE_MAX)

typedef struct Stack {
  size_t * data;
  size_t size;
  size_t capacity;
  size_t top;
} Stack;

Stack stack_empty(size_t capacity) {
  return (Stack) {
    .data = calloc(capacity, sizeof(size_t)),
    .size = 0,
    .capacity = capacity,
    .top = 0,
  };
}

bool stack_is_empty(const Stack *stack) {
  return stack->size == 0;
}

size_t stack_peek(const Stack *stack) {
  assert(!stack_is_empty(stack));
  return stack->data[stack->top];
}

void stack_push(Stack *stack, size_t value) {
  assert(stack->size < stack->capacity);
  size_t new_top = stack->size;
  stack->data[new_top] = value;
  stack->top = new_top;
  stack->size += 1;
}

size_t stack_pop(Stack *stack) {
  size_t value = stack_peek(stack);
  stack->top -= 1;
  stack->size -= 1;
  return value;
}

void free_stack(Stack s) {
  free(s.data);
  s.data = NULL;
}

typedef struct LL {
  size_t root;
  size_t count;
  size_t capacity;
  int32_t *values;
  size_t *links;
  Stack available;
} LL;


LL ll_empty(size_t capacity) {
  Stack available = stack_empty(capacity);
  for (size_t i = 0; i < capacity; i++) {
    stack_push(&available, i);
  }
  return  (LL) {
    .root = FINAL_NODE,
    .count = 0,
    .capacity = capacity,
    .values = calloc(capacity, sizeof(int32_t)),
    .links = calloc(capacity, sizeof(size_t)),
    .available= available,
  };
}

void ll_free(LL list) {
  free(list.values);
  list.values = NULL;
  free(list.links);
  list.links = NULL;
  free_stack(list.available);
}

size_t ll_next_avaiable_index(LL *list) {
  return stack_pop(&list->available);
}

bool ll_is_empty(const LL list) {
  return list.count == 0;
}

void ll_insert_first(LL *list, int32_t value) {
  if (list->count < list->capacity) {
    size_t new_index = ll_next_avaiable_index(list);
    list->values[new_index] = value;
    list->links[new_index] = list->root;
    list->root = new_index;
    list->count += 1;
  }
}

void ll_insert_at(LL *list, int32_t value, size_t index) {
  assert(index >= 0 && index <= list->count);
  if (index == 0) {
    ll_insert_first(list, value);
    return;
  }

  if (list->count < list->capacity) {
    size_t nth_node = 0;
    size_t curr_index = list->root;
    while (nth_node < (index - 1)) {
      curr_index = list->links[curr_index];
      nth_node += 1;
    }
    size_t new_index = ll_next_avaiable_index(list);
    size_t curr_next = list->links[curr_index];
    list->links[curr_index] = new_index;
    list->values[new_index] = value;
    list->links[new_index] = curr_next;
    list->count += 1;
  }
}

void ll_insert_last(LL *list, int32_t value)
{
  ll_insert_at(list, value, list->count);
}

void ll_delete_first(LL *list) {
  size_t first = list->root;
  size_t next = list->links[first];
  list->root = next;
  stack_push(&list->available, first);
  list->count -= 1;
}

void ll_delete_at(LL *list, size_t index) {
  assert(index >= 0 && index < list->count);
  if (index == 0) {
    ll_delete_first(list);
  }

  size_t nth_node = 0;
  size_t curr_index = list->root;
  while (nth_node < (index - 1)) {
    curr_index = list->links[curr_index];
    nth_node += 1;
  }
  size_t node_to_delete = list->links[curr_index];
  size_t next_index = list->links[node_to_delete];
  list->links[curr_index] = next_index;
  stack_push(&list->available, node_to_delete);
  list->count -= 1;
}

void ll_delete_last(LL *list) {
  ll_delete_at(list, list->count - 1);
}

typedef struct NodeIter {
  LL list;
  size_t curr_node;
} NodeIter;


NodeIter ll_iter(LL list) {
  return (NodeIter) {
    .list = list,
    .curr_node = list.root,
  };
}

int32_t iter_next(NodeIter *iter) {
  int32_t value = iter->list.values[iter->curr_node];
  iter->curr_node = iter->list.links[iter->curr_node];
  return value;
}

bool iter_has_next(const NodeIter iter) {
  return iter.curr_node != FINAL_NODE;
}

void ll_dump(LL list) {
  NodeIter iter = ll_iter(list);
  while(iter_has_next(iter)) {
    int32_t value = iter_next(&iter);
    printf("%d\n", value);
  }
}

void ll_dump_debug(const LL list) {
  printf("capacity = %zu; count = %zu; root = %zu\n",
	 list.capacity, list.count, list.root);

  for (size_t i = 0; i < list.capacity; i++) {
    printf("[%zu] Value: %d, Link: %zu\n",
	   i,
	   list.values[i],
	   list.links[i]);
  }
  printf("\n\n");
}

int main(void) {
  printf("[BEGIN]\n");
  LL list = ll_empty(64);
  ll_insert_first(&list, 23);  // 23
  ll_insert_last(&list, 64);   // 23, 64
  ll_insert_last(&list, 42);   // 23, 64, 42
  ll_insert_first(&list, 16);  // 16, 23, 64, 42
  ll_insert_at(&list, 420, 2); // 16, 23, 420, 64, 42
  ll_delete_first(&list);      // 23, 420, 64, 42
  ll_dump(list);
  ll_dump_debug(list);
  printf("[DONE]\n");
  ll_free(list);
  return 0;
}
