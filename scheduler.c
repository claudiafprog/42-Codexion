/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:25:26 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/02 17:02:18 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_next_ticket(t_config	*config)
{
	long	ticket;

	ticket = config->next_ticket;
	config->next_ticket++;
	return (ticket);
}

int	is_higher_priority(t_config *config, t_node *a, t_node *b)
{
	if (strcmp(config->scheduler, "edf") == 0)
	{
		if (a->deadline < b->deadline)
			return (1);
		else if (a->deadline == b->deadline)
			if (a->ticket < b->ticket)
				return (1);
	}
	else if (strcmp(config->scheduler, "fifo") == 0)
		if (a->ticket > b->ticket)
			return (1);
	return (0);
}

static void	heapify_up(t_heap *heap, int i, t_config *config)
{
	int		father_i;
	t_node	*temp;

	while (i > 0)
	{
		father_i = (i - 1) / 2;
		if (is_higher_priority(config, heap->nodes[i], heap->nodes[father_i]))
		{
			temp = heap->nodes[i];
			heap->nodes[i] = heap->nodes[father_i];
			heap->nodes[father_i] = temp;
			i = father_i;
		}
		else
			break ;
	}
}

static void	heapify_down(t_heap *heap, int i, t_config *config)
{
	int		left;
	int		right;
	int		smallest;
	t_node	*temp;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size && is_higher_priority(
				config, heap->nodes[left], heap->nodes[smallest]))
			smallest = left;
		if (right < heap->size && is_higher_priority(
				config, heap->nodes[right], heap->nodes[smallest]))
			smallest = right;
		if (smallest == i)
			break ;
		temp = heap->nodes[i];
		heap->nodes[i] = heap->nodes[smallest];
		heap->nodes[smallest] = temp;
		i = smallest;
	}
}

int	heap_append_scheduled(t_heap	*heap, t_node	*new_node, t_config	*config)
{
	int		i;
	int		father_i;
	t_node	*temp;

	if (heap->size >= heap->capacity)
		return (0);
	heap->nodes[heap->size] = new_node;
	i = heap->size;
	heap->size++;
	while (i > 0)
	{
		father_i = (i - 1) / 2;
		if (is_higher_priority(config, heap->nodes[i], heap->nodes[father_i]))
		{
			temp = heap->nodes[i];
			heap->nodes[i] = heap->nodes[father_i];
			heap->nodes[father_i] = temp;
			i = father_i;
		}
		else
			break ;
	}
	return (1);
}

t_node  *heap_pop(t_heap    *heap, t_config *config)
{
    t_node  *prio;

    if (heap->size == 0)
        return (NULL);
    prio = heap->nodes[0];
    heap->size--;
    if (heap->size > 0)
    {
        heap->nodes[0] = heap->nodes[heap->size];
        heapify_down(heap, 0, config);
    }
    return (prio);
}

void	heap_prio_remove(t_heap *heap, t_coder *coder, t_config *config)
{
	int	i;

	i = 0;
	while (i < heap->size && heap->nodes[i]->id != coder->id)
		i++;
	if (i == heap->size)
		return ;

	heap->size--;
	if (i < heap->size)
	{
		heap->nodes[i] = heap->nodes[heap->size];
		heapify_up(heap, i, config);
		heapify_down(heap, i, config);
	}
}
