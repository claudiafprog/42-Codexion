/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:25:26 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/20 15:07:51 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

t_queue	*fifo_queue(t_queue	*head, t_queue	*new_node)
{
	t_queue	*current;

	if (!head)
		return (new_node);
	current = head;
	while (current->next)
		current = current->next;
	current->next = new_node;
	return (head);
}

t_queue	*edf_queue(t_queue	*head, t_queue	*new_node)
{
	t_queue	*current;

	if (!head || new_node->deadline < head->deadline)
	{
		new_node->next = head;
		return (new_node);
	}
	current = head;
	while (current->next
		&& (current->next->deadline < new_node->deadline
			|| (current->next->deadline == new_node->deadline
				&& current->next->id < new_node->id)))
		current = current->next;
	new_node->next = current->next;
	current->next = new_node;
	return (head);
}

t_queue	*chosen_scheduler(t_config	*config, t_queue	*head,
	t_queue	*new_node)
{
	if (strcmp(config->scheduler, "fifo") == 0)
		return (fifo_queue(head, new_node));
	else if (strcmp(config->scheduler, "edf") == 0)
		return (edf_queue(head, new_node));
	return (head);
}
