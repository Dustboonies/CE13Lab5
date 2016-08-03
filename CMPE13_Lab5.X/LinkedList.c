/**
 * @file
 * This file provided a doubly-linked list implementation for storing strings (NULL-terminated
 * character arrays.
 * The list implementation relies on a chain metaphor: a list is merely a sequence of links
 * (ListItems) and there is no separate construct to represent the entire list, each ListItem in it
 * does that implicitly.
 * ListItems can store pointers to strings, but the strings themselves must be stored somewhere else.
 * This list supports NULL pointers as well.
 */

//Standard Libraries
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "LinkedList.h"



/**
 * This is the struct that will hold an individual list item. This is a doubly-linked list and
 * so there is no need to have a separate list struct that holds all of the individual list items
 * as they're already chained together. Note that the data is a (void *), which means that it can
 * hold any type of pointer, even pointers to multi-dimensional arrays. This also means that any
 * data stored in a list item must first be allocated.
 */

/**
 * This function starts a new linked list. Given an allocated pointer to data it will return a
 * pointer for a malloc()ed ListItem struct. If malloc() fails for any reason, then this function
 * returns NULL otherwise it should return a pointer to this new list item. data can be NULL.
 *
 * @param data The data to be stored in the first ListItem in this new list. Can be any valid 
 *             pointer value.
 * @return A pointer to the malloc()'d ListItem. May be NULL if an error occured.
 */
ListItem *LinkedListNew(char *data)
{

    ListItem *temp = malloc(sizeof (ListItem));
    temp->data = data;
    if (temp == NULL) {
        return NULL;
    }
    return temp;
}

/**
 * This function will remove a list item from the linked list and free() the memory that the
 * ListItem struct was using. It doesn't, however, free() the data pointer and instead returns it
 * so that the calling code can manage it.  If passed a pointer to NULL, LinkedListRemove() should
 * return NULL to signal an error.
 *
 * @param item The ListItem to remove from the list.
 * @return The data pointer from the removed item. May be NULL.
 */
char *LinkedListRemove(ListItem *item)
{
    ListItem *temp = item;
    char *data = temp->data;
    if (temp == NULL) {
        return NULL;
    }
    if (temp->nextItem == NULL && temp->previousItem == NULL) {
        free(temp);
    } else if (temp->nextItem == NULL) {
        temp->previousItem->nextItem = NULL;
        free(temp);
    } else if (temp->previousItem == NULL) {
        temp->nextItem->previousItem = NULL;
        free(temp);
    } else {
        temp->nextItem->previousItem = temp->previousItem;
        temp->previousItem->nextItem = temp->nextItem;
        free(temp);
    }
    return data;

}

/**
 * This function returns the total size of the linked list. This means that even if it is passed a
 * ListItem that is not at the head of the list, it should still return the total number of
 * ListItems in the list. A NULL argument will result in 0 being returned.
 *
 * @param list An item in the list to be sized.
 * @return The number of ListItems in the list (0 if `list` was NULL).
 */
int LinkedListSize(ListItem *list)
{
    ListItem *temp = LinkedListGetFirst(list);
    int count = 0;
    while (temp != NULL) {
        count++;
        temp = temp->nextItem;
    }
    return count;
}

/**
 * This function returns the head of a list given some element in the list. If it is passed NULL,
 * it will just return NULL. If given the head of the list it will just return the pointer to the
 * head anyways for consistency.
 *
 * @param list An element in a list.
 * @return The first element in the list. Or NULL if provided an invalid list.
 */
ListItem *LinkedListGetFirst(ListItem *list)
{
    ListItem *first;
    first = list;
    while (first->previousItem != NULL) {
        first = first->previousItem;
    }
    return first;
}

/**
 * This function allocates a new ListItem containing data and inserts it into the list directly
 * after item. It rearranges the pointers of other elements in the list to make this happen. If
 * passed a NULL item, InsertAfter() should still create a new ListItem, just with no previousItem.
 * It returns NULL if it can't malloc() a new ListItem, otherwise it returns a pointer to the new
 * item. The data parameter is also allowed to be NULL.
 *
 * @param item The ListItem that will be before the newly-created ListItem.
 * @param data The data the new ListItem will point to.
 * @return A pointer to the newly-malloc()'d ListItem.
 */
ListItem *LinkedListCreateAfter(ListItem *item, char *data)
{
    ListItem *new = LinkedListNew(data);
    if (new == NULL) {
        return NULL;
    }
    if (item->nextItem == NULL) {
        new->nextItem = NULL;
        new->previousItem = item;
        item->nextItem = new;
    } else {
        new->previousItem = item;
        new->nextItem = item->nextItem;
        item->nextItem->previousItem = new;
        item->nextItem = new;
    }
    return new;
}

/**
 * LinkedListSwapData() switches the data pointers of the two provided ListItems. This is most
 * useful when trying to reorder ListItems but when you want to preserve their location. It is used
 * within LinkedListSort() for swapping items, but probably isn't too useful otherwise. This
 * function should return STANDARD_ERROR if either arguments are NULL, otherwise it should return
 * SUCCESS. If one or both of the data pointers are NULL in the given ListItems, it still does
 * perform the swap and returns SUCCESS.
 *
 * @param firstItem One of the items whose data will be swapped.
 * @param secondItem Another item whose data will be swapped.
 * @return SUCCESS if the swap worked or STANDARD_ERROR if it failed.
 */
int LinkedListSwapData(ListItem *firstItem, ListItem *secondItem)
{
    if (firstItem != NULL || secondItem != NULL) {
        char *temp = NULL;
        temp = firstItem->data;
        firstItem->data = secondItem->data;
        secondItem->data = temp;
        return SUCCESS;
    } else {
        return STANDARD_ERROR;
    }
}

/**
 * LinkedListSort() performs a selection sort on list to sort the elements into decending order. It
 * makes no guarantees of the addresses of the list items after sorting, so any ListItem referenced
 * before a call to LinkedListSort() and after may contain different data as only the data pointers
 * for the ListItems in the list are swapped. This function sorts the strings in ascending order
 * first by size (with NULL data pointers counting as 0-length strings) and then alphabetically
 * ascending order. So the list [dog, cat, duck, goat, NULL] will be sorted to [NULL, cat, dog,
 * duck, goat]. LinkedListSort() returns SUCCESS if sorting was possible. If passed a NULL pointer
 * for either argument, it will do nothing and return STANDARD_ERROR.
 *
 * @param list Any element in the list to sort.
 * @return SUCCESS if successful or STANDARD_ERROR is passed NULL pointers.
 */
int LinkedListSort(ListItem *list)
{
    if (list == NULL) {
        return STANDARD_ERROR;
    }
    ListItem *select,*cmp;
    for(select = list; select->nextItem !=NULL; select = select->nextItem){
        for(cmp = select->nextItem; cmp != NULL; cmp = cmp->nextItem){
            if (select->data == NULL){
                break;
            } else if (cmp->data == NULL){
                LinkedListSwapData(select,cmp);
            } else if (strlen(select->data)>strlen(cmp->data)){
                LinkedListSwapData(select,cmp);
            } else if (strlen(select->data) == strlen(cmp->data)){
                if(strcmp(cmp->data,select->data)<0){
                    LinkedListSwapData(select,cmp);
                }
            }
        }
    }
//    int f1, f2;
//    ListItem *temp, *current;
//    current = list;
//    int size = LinkedListSize(current);
//    current = LinkedListGetFirst(current);
//    for (f1 = 0; f1 < size - 1; f1++) {
//        temp = current;
//        for (f2 = f1 + 1; f2 < size; f2++) {
//            if (current->data == NULL) {
//                LinkedListSwapData(current, temp);
//            } else if (temp->data == NULL) {
//                break;
//            } else if (strlen(current->data) < strlen(temp->data)) {
//                LinkedListSwapData(current, temp);
//            } else if (strlen(current->data) == strlen(temp->data)) {
//                if (strcmp(current->data, temp->data) < 0) {
//                    LinkedListSwapData(current, temp);
//                }
//            }
//            current = current->nextItem;
//        }
//        current = temp;
//        current = current->nextItem;
//    }
    return SUCCESS;
}

/**
 * LinkedListPrint() prints out the complete list to stdout. This function prints out the given
 * list, starting at the head if the provided pointer is not the head of the list, like "[STRING1,
 * STRING2, ... ]" If LinkedListPrint() is called with a NULL list it does nothing, returning
 * STANDARD_ERROR. If passed a valid pointer, prints the list and returns SUCCESS.
 *
 * @param list Any element in the list to print.
 * @return SUCCESS or STANDARD_ERROR if passed NULL pointers.
 */
int LinkedListPrint(ListItem *list)
{
    if (list == NULL) {
        return STANDARD_ERROR;
    }
    list = LinkedListGetFirst(list);
    printf("{");
    while (list != NULL) {
        if (list->data == NULL) {
            printf("-(null)-");
        } else {
            printf("-%s-", list->data);
        }
        list = list->nextItem;
    }
    printf("}\n");
    return SUCCESS;
}
