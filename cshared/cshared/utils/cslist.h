#ifndef CSHARED_UTILS_CSLIST_H_
#define CSHARED_UTILS_CSLIST_H_

#include <stdio.h>
#include <commons/collections/list.h>

int 	list_find_index(t_list*, bool(*condition)(void*));
double 	list_sum(t_list* self, double(*element_value)(void*));

#endif /* CSHARED_UTILS_CSLIST_H_ */
