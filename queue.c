#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *pos, *safe;
    list_for_each_entry_safe (pos, safe, head, list) {
        list_del(&pos->list);
        q_release_element(pos);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return NULL;

    int len = (strlen(s) + 1) * sizeof(char);
    element_t *item = malloc(sizeof(element_t));
    item->value = malloc(len);
    memset(item->value, '\0', len);
    strncpy(item->value, s, strlen(s));

    list_add(&item->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return NULL;

    int len = (strlen(s) + 1) * sizeof(char);
    element_t *item = malloc(sizeof(element_t));
    item->value = malloc(len);
    memset(item->value, '\0', len);
    strncpy(item->value, s, strlen(s));

    list_add_tail(&item->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    element_t *item = list_entry(head->next, element_t, list);
    list_del(head->next);

    if (!sp)
        return item;

    memset(sp, '\0', bufsize);
    strncpy(sp, item->value, bufsize - 1);
    return item;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    element_t *item = list_entry(head->prev, element_t, list);
    list_del(head->prev);

    if (!sp)
        return item;

    memset(sp, '\0', bufsize);
    strncpy(sp, item->value, bufsize - 1);
    return item;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *slow = head->next, *fast = head->next;

    while ((fast != head) && (fast->next != head)) {
        slow = slow->next;
        fast = fast->next->next;
    }

    element_t *item = list_entry(slow, element_t, list);
    list_del(slow);
    q_release_element(item);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    element_t *toDelete, *item;
    struct list_head *pos, *safe;
    int flag = 0;
    toDelete = list_entry(head->next, element_t, list);
    pos = head->next->next;
    safe = pos->next;

    while (pos != head) {
        item = list_entry(pos, element_t, list);
        if (!strcmp(toDelete->value, item->value)) {
            list_del(&toDelete->list);
            q_release_element(toDelete);
            toDelete = item;
            flag = 1;
        } else if (flag) {
            list_del(&toDelete->list);
            q_release_element(toDelete);
            toDelete = item;
            flag = 0;
        } else {
            toDelete = item;
        }
        pos = safe;
        safe = pos->next;
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head))
        return;

    struct list_head *pos, *safe;

    list_for_each_safe (pos, safe, head) {
        pos->next = pos->prev;
        pos->prev = safe;
    }
    head->next = head->prev;
    head->prev = safe;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;
    if (list_empty(head))
        return;
    if (q_size(head) < k)
        return;

    LIST_HEAD(newList);
    LIST_HEAD(tmp);
    struct list_head *pos, *safe;

    int cnt = 1;
    list_for_each_safe (pos, safe, head) {
        if (cnt == k) {
            list_cut_position(&tmp, head, pos);
            q_reverse(&tmp);
            list_splice_tail(&tmp, &newList);
            cnt = 0;
        }
        cnt++;
    }
    list_splice(&newList, head);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *item = list_entry(head->prev, element_t, list);
    char *cmp = item->value;
    struct list_head *pos, *safe;

    // list_for_each_prev_safe(pos, safe, head)
    for (pos = head->prev->prev, safe = pos->prev; pos != head;
         pos = safe, safe = pos->prev) {
        item = list_entry(pos, element_t, list);
        if (strcmp(cmp, item->value) >= 0) {
            list_del(pos);
            q_release_element(item);
        } else {
            cmp = item->value;
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
