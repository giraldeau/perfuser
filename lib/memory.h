/*
 * memory.h
 *
 *  Created on: Jan 15, 2015
 *      Author: francis
 */

#ifndef MEMORY_H_
#define MEMORY_H_

/**
 * FREE:
 * @ptr: pointer holding address to be freed
 *
 * Free the memory stored in 'ptr' and update to point
 * to NULL.
 */
#define FREE(ptr)                               \
  do {                                          \
    free(ptr);                                  \
    (ptr) = NULL;                               \
  } while(0)


#endif /* MEMORY_H_ */
