#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <inttypes.h>
#include <malloc.h>

#include "assignment4.h"

// --- Problem 1: Compare-and-swap Spinlock  ---

/**
 * Initialize a newly allocated cas_lock_t to the unlocked state. After
 * allocating a cas_lock_t it must be initialized before calling
 * cas_lock_acquire() and cas_lock_release() with that lock is legal.
 *
 * This function performs no allocations itself, and it never fails.
 * Hence, no special function needs to be called before freeing the lock later.
 *
 * This function may not be thread-safe, and only one thread should call it.
 *
 * Arguments:
 *  - lock: a pointer to a newly allocated cas_lock_t whose fields are to be
 *          initialized; passing any other pointer is illegal.
 */
void cas_lock_init(cas_lock_t* lock) {
  bool expected = false;
  atomic_compare_exchange_strong(&lock->locked, &expected, false);
}

/**
 * Spins until ownership of the lock is acquired; this ensures that only thread
 * has ownership of the lock at a time. It is illegal to call this function if
 *  - the lock has never been initialized (see cas_lock_init()),
 *  - or this thread already owns the passed lock.
 *
 * This function never fails.
 *
 * Arguments:
 *  - lock: a pointer to an initalized cas_lock_t not currently owned by the
 *          calling thread; passing any other pointer is illegal.
 */
void cas_lock_acquire(cas_lock_t* lock) {
  bool expected = false;
  bool success = false;
  while(!success) {
    success = atomic_compare_exchange_strong(&lock->locked, &expected, true);
    expected = false;
  }
}

/**
 * Releases ownership of the lock. It is illegal to call this function
 * if this thread does not own the passed lock.
 *
 * This function never fails.
 *
 * Arguments:
 * - lock: a pointer to a cas_lock_t currently owned by the calling
 *         thread; passing any other pointer is illegal.
 */
void cas_lock_release(cas_lock_t* lock) {
  bool expected = true;
  atomic_compare_exchange_strong(&lock->locked, &expected, false);
}

// --- Problem 2: Ticket Spinlock  ---

/**
 * Initialize a newly allocated ticket_lock_t to the unlocked state. After
 * allocating a ticket_lock_t it must be initialized before calling
 * ticket_lock_acquire() and ticket_lock_release() with that lock is legal.
 *
 * This function performs no allocations itself, and it never fails.
 * Hence, no special function needs to be called before freeing the lock later.
 *
 * This function may not be thread-safe, and only one thread should call it.
 *
 * Arguments:
 *  - lock: a pointer to a newly allocated ticket_lock_t whose fields are to be
 *          initialized; passing any other pointer is illegal.
 */
void ticket_lock_init(ticket_lock_t* lock) {
  int zero = 0;
  int counter = 0;
  atomic_store(&lock->ticket, zero);
  atomic_store(&lock->turn, counter);
}

/**
 * Spins until ownership of the lock is acquired; this ensures that only thread
 * has ownership of the lock at a time. It is illegal to call this function if
 *  - the lock has never been initialized (see ticket_lock_init()),
 *  - or this thread already owns the passed lock.
 *
 * This function never fails.
 *
 * Arguments:
 *  - lock: a pointer to an initalized ticket_lock_t not currently owned by the
 *          calling thread; passing any other pointer is illegal.
 */
void ticket_lock_acquire(ticket_lock_t* lock) {
  int me;
  me = atomic_fetch_add(&lock->ticket, 1);
  while (me != atomic_load(&lock->turn));
}

/**
 * Releases ownership of the lock. It is illegal to call this function
 * if this thread does not own the passed lock.
 *
 * This function never fails.
 *
 * Arguments:
 * - lock: a pointer to a ticket_lock_t currently owned by the calling
 *         thread; passing any other pointer is illegal.
 */
void ticket_lock_release(ticket_lock_t* lock) {
  atomic_store(&lock->turn, atomic_load(&lock->turn)+1);
}

// --- Problem 3: Linked List ---

/**
 * Initialize a newly allocated list_t to the empty list state. After
 * allocating a list_t it must be initialized before calling
 * list_insert() and list_find() with that list is legal.
 *
 * This function performs no allocations itself, and it never fails.
 * Lists can never be freed once a call to list_insert() is made, since
 * deallocation/removal of elements isn't supported.
 *
 * This function may not be thread-safe, and only one thread should call it.
 *
 * Arguments:
 *  - list: a pointer to a newly allocated list_t whose fields are to be
 *          initialized.
 */
void list_init(list_t* list) {
  list->head = NULL;
  cas_lock_init(&list->lock);
}

/**
 * Insert key into list. After list_insert(list, key) returns, a call to
 * list_find(list, key) will henceforth be guaranteed to return true.
 *
 * This function is thread-safe; multiple threads can call list_insert() and
 * list_find() concurrently and safely.
 *
 * This function must perform memory allocation which can fail. Memory
 * allocation failures log a message and cause the current process to exit.
 *
 * Arguments:
 *  - list: a pointer to an initialized list_t; passing any other pointer
 *          is illegal.
 *  - key: a value which is guaranteed to be found by future calls to
 *         list_find() on the same list once this call returns.
 */
void list_insert(list_t* list, int key) {
  cas_lock_acquire(&list->lock);
  node_t *n = malloc(sizeof(node_t));
  n->key=key;
  n->next=list->head;
  list->head=n;
  cas_lock_release(&list->lock);
}

/**
 * Find key in list. Returns true if key exists in list, otherwise returns
 * false.
 *
 * This function is thread-safe; multiple threads can call list_insert() and
 * list_find() concurrently and safely.
 *
 * This function never fails.
 *
 * Arguments:
 *  - list: a pointer to an initialized list_t; passing any other pointer
 *          is illegal.
 *  - key: the value to search for in list; if key exists (due to a prior
 *         list_insert()) the function returns true, otherwise it returns false.
 */
bool list_find(list_t* list, int key) {
  cas_lock_acquire(&list->lock);
  node_t *tmp = list->head;
  while (tmp) {
    if (tmp->key == key) {
      cas_lock_release(&list->lock);
      return true;
    }
    tmp = tmp->next;
  }
  cas_lock_release(&list->lock);
  return false;
}

