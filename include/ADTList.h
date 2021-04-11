////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file ADTList.h
/// @author Constantinos Georgiou
/// @brief Interface for List abastract data type.
///        Operations included:
///          insertion, removal, look up, serial traversal, accessing at given index.
/// @version 0.1
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common_types.h"  // CompareFunc, DestroyFunc

#define LIST_BOF (ListNode)0  // Defines the "virtual" beginning of the list.
#define LIST_EOF (ListNode)0  // Defines the "virtual" end of the list.

typedef struct list* List;
typedef struct list_node* ListNode;

/// @brief Creates and returns a new list.
///
/// @param destroy_value If destroy_value != NULL, then each time an item is removed,
///                      destroy(value) is called.
///
/// @return Newly created list.
///
List list_create(DestroyFunc destroy_value);

/// @brief Frees all the allocated memory of the given list.
///
///
void list_destroy(List);

/// @brief Changes the function called on each element removal/overwrite to given destroy.
///
/// @param list Defines a list.
/// @param destroy Defines a destroy function.
///
/// @return Previous destroy function.
///
DestroyFunc list_set_destroy_value(List list, DestroyFunc destroy);

/// @brief Returns the number of elements in given list.
///
/// @return The number of elements in given list.
///
int list_size(List);

/// @brief Inserts a new node with given value after given node, or at the beginning
///        if node == LIST_BOF.
///
/// @param list Defines a list.
/// @param node Defines a list node.
/// @param value Defines a value to be inserted.
///
/// @return Pointer to newly inserted node, or NULL if an error occurred.
///
ListNode list_insert_next(List list, ListNode node, void* value);

// Αφαιρεί τον __επόμενο__ κόμβο από τον node, ή τον πρώτο κόμβο αν node == LIST_BOF.

/// @brief Removes the next node after given node, or first node if node == LIST_BOF.
///
/// @param list Defines a list.
/// @param node Defines a list node.
///
/// @return Pointer to given node, or NULL if an error occurred.
///
ListNode list_remove_next(List list, ListNode node);

/// @brief Removes given node from given list.
///
/// @param list Defines a list.
/// @param node Defines a list node.
///
/// @return Pointer to next node of newly removed node, or NULL if an error occurred.
///
void list_remove(List list, ListNode node);

/// @brief Appends to_append list, to given list. After operation to_append list can not be used.
///        list == to_append causes undefined behaviour.
///
/// @param list Defines a list.
/// @param to_append Defines a list to be appended.
///
/// @return Pointer to the beginning of the appened list, or NULL if an error occured.
///
void list_append(List list, List to_append);

/// @brief Finds and returns the first list node with value equivalent to value, (based on compare
///        function).
///
/// @param list Defines a list.
/// @param value Defines desired value.
/// @param compare Defines a compare function.
///
/// @return The list node of given value, or LIST_EOF if value was not found.
///
ListNode list_find_node(List list, void* value, CompareFunc compare);

/// @brief Finds and returns the first value equivalent to given value, (based on compare function).
///
/// @param list Defines a list.
/// @param value Defines a desired value.
/// @param compare Defines a compare function.
///
/// @return The desired value, or NULL if value was not found.
///
void* list_find(List list, void* value, CompareFunc compare);

/// @brief Returns the value of given list node.
///
/// @return Content of given node.
///
void* list_node_value(List, ListNode);

////////////    TRAVERSAL    ///////////////////////////////////////////////////////////////////////

/// @brief Returns first list node, or LIST_BOF if list is empty.
///
///
ListNode list_first(List);

/// @brief Returns last list node, or LIST_EOF if list is empty.
///
///
ListNode list_last(List);

/// @brief Returns the next node of given node, or LIST_EOF if given node is the last node.
///
///
ListNode list_next(List, ListNode);

/// @brief Returns the value of given position in list.
///
/// @param position Defines the position in the list.
///
/// @return Value of given position, or NULL if error.
///
void* list_get_at(List, int position);

////////////    END OF TRAVERSAL    ////////////////////////////////////////////////////////////////
