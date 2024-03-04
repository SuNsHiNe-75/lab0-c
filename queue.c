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
    struct list_head *q_head = malloc(sizeof(*q_head));
    if (!q_head) {
        free(q_head);
        return NULL;
    }

    INIT_LIST_HEAD(q_head);
    return q_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = strdup(s);
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
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
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

    element_t *rmv_element = list_first_entry(head, element_t, list);
    if (!rmv_element) {
        return NULL;
    }

    if (sp != NULL) {
        strncpy(sp, rmv_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del(&rmv_element->list);

    return rmv_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *rmv_element = list_last_entry(head, element_t, list);
    if (!rmv_element) {
        return NULL;
    }

    if (sp != NULL) {
        strncpy(sp, rmv_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del(&rmv_element->list);

    return rmv_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int cnt = 0;
    struct list_head *cur;
    list_for_each (cur, head) {
        cnt++;
    }
    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    int leng = q_size(head);
    int cnt = -1;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        cnt++;
        if (cnt == (leng / 2)) {
            list_del(&entry->list);
            free(entry->value);
            free(entry);
            break;
        }
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    element_t *entry = NULL, *safe = NULL;
    bool dup = false;

    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head &&
            strcmp(entry->value, safe->value) == 0) {
            list_del(&entry->list);
            q_release_element(entry);
            dup = true;
        } else if (dup) {
            list_del(&entry->list);
            q_release_element(entry);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *pre, *cur;
    for (pre = head->next, cur = pre->next; pre != head && cur != head;
         pre = pre->next, cur = pre->next) {
        list_move(pre, cur);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k == 0)
        return;

    struct list_head *cur_tail = head->next;
    int rev_times = q_size(head) / k;

    LIST_HEAD(tmp);
    LIST_HEAD(result);

    for (int i = 0; i < rev_times; ++i) {
        for (int j = 0; j < k; ++j)
            cur_tail = cur_tail->next;
        list_cut_position(&tmp, head, cur_tail->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &result);
    }
    list_splice_init(&result, head);
}

void merge2list(struct list_head *left_head,
                struct list_head *right_head,
                struct list_head *head,
                bool descend)
{
    while (!list_empty(left_head) && !list_empty(right_head)) {
        element_t *left_entry = list_entry(left_head->next, element_t, list);
        element_t *right_entry = list_entry(right_head->next, element_t, list);

        if (!descend) {
            if (strcmp(left_entry->value, right_entry->value) <= 0)
                list_move_tail(left_head->next, head);
            else
                list_move_tail(right_head->next, head);
        } else {
            if (strcmp(left_entry->value, right_entry->value) >= 0)
                list_move_tail(left_head->next, head);
            else
                list_move_tail(right_head->next, head);
        }
    }

    if (list_empty(left_head))
        list_splice_tail_init(right_head, head);
    else
        list_splice_tail_init(left_head, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *slow = head, *fast = head;

    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (!(fast == head || fast->next == head));

    LIST_HEAD(left_head);
    LIST_HEAD(right_head);
    list_splice_tail_init(head, &right_head);
    list_cut_position(&left_head, &right_head, slow);

    q_sort(&left_head, descend);
    q_sort(&right_head, descend);
    merge2list(&left_head, &right_head, head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    struct list_head *cur, *safe;
    char *s = list_entry(head->prev, element_t, list)->value;
    for (cur = (head)->prev, safe = cur->prev; cur != (head);
         cur = safe, safe = cur->prev) {
        element_t *tmp = list_entry(cur, element_t, list);
        if (cur != head->prev) {
            if (strcmp(s, tmp->value) > 0) {
                s = tmp->value;
            } else {
                list_del(&tmp->list);
                q_release_element(tmp);
            }
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    struct list_head *cur, *safe;
    char *s = list_entry(head->prev, element_t, list)->value;
    for (cur = (head)->prev, safe = cur->prev; cur != (head);
         cur = safe, safe = cur->prev) {
        element_t *tmp = list_entry(cur, element_t, list);
        if (cur != head->prev) {
            if (strcmp(s, tmp->value) < 0) {
                s = tmp->value;
            } else {
                list_del(&tmp->list);
                q_release_element(tmp);
            }
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
