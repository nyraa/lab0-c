#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *e = list_entry(node, element_t, list);
        list_del(node);
        q_release_element(e);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);

    // strdup returns NULL if allocation fails
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->next, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(head->next);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *pos;
    list_for_each (pos, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr, *safe;

    list_for_each_safe (curr, safe, head) {
        list_move(curr, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k < 2)
        return;
    if (list_is_singular(head))
        return;
    LIST_HEAD(dummy_head);
    struct list_head *head_from = head, *node = head, *tmp;
    int size = q_size(head);
    for (int i = 0, j = 0; i <= size; ++i, ++j) {
        if (j == k) {
            list_cut_position(&dummy_head, head_from, node);
            q_reverse(&dummy_head);
            tmp = head_from->next;
            list_splice(&dummy_head, head_from);
            head_from = tmp->prev;
            node = tmp->prev;
            j = 0;
        }
        node = node->next;
    }
}

/* Merge two sorted queue in to one queue, save to head1,  */
static void merge_two_lists(struct list_head *head1, struct list_head *head2)
{
    LIST_HEAD(dummy_head);
    while (!list_empty(head1) && !list_empty(head2)) {
        if (strcmp(list_first_entry(head1, element_t, list)->value,
                   list_first_entry(head2, element_t, list)->value) < 0) {
            list_move_tail(head1->next, &dummy_head);
        } else {
            list_move_tail(head2->next, &dummy_head);
        }
    }
    list_splice_tail_init(list_empty(head1) ? head2 : head1, &dummy_head);
    list_splice(&dummy_head, head1);
}

/* Merge sort */
static void merge_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    /* Use top-down merge sort */
    LIST_HEAD(left);
    struct list_head *slow = head, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_cut_position(&left, head, slow);
    merge_sort(&left);
    merge_sort(head);
    merge_two_lists(head, &left);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    merge_sort(head);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    int len = q_size(head);
    while (len > 1) {
        queue_contex_t *first_queue =
            (queue_contex_t *) list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second_queue = (queue_contex_t *) list_entry(
            first_queue->chain.next, queue_contex_t, chain);
        for (int i = 0; i < (len >> 1); ++i) {
            merge_two_lists(first_queue->q, second_queue->q);
            list_move_tail(&second_queue->chain, head);
            first_queue = (queue_contex_t *) list_entry(first_queue->chain.next,
                                                        queue_contex_t, chain);
            second_queue = (queue_contex_t *) list_entry(
                first_queue->chain.next, queue_contex_t, chain);
        }
        len =
            (len >> 1) + (len & 1); /* Leave tail odd th queue to next round */
    }
    struct list_head *first_list =
        list_first_entry(head, queue_contex_t, chain)->q;
    if (descend)
        q_reverse(first_list);
    return q_size(first_list);
}
