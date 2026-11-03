/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 19:34:42 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/03 12:06:55 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_fifo_priority(t_coder	*coder, t_coder	*left, t_coder	*right)
{
	if (left->waiting && are_dongles_available(left)
		&& left->node->ticket != -1
		&& left->node->ticket < coder->node->ticket)
		return (0);
	if (right->waiting && are_dongles_available(right)
		&& right->node->ticket != -1
		&& right->node->ticket < coder->node->ticket)
		return (0);
	return (1);
}

static int	check_edf_priority(t_coder	*coder, t_coder	*left, t_coder	*right)
{
	if (left->waiting && are_dongles_available(left)
		&& left->node->deadline < coder->node->deadline)
		return (0);
	if (right->waiting && are_dongles_available(right)
		&& right->node->deadline < coder->node->deadline)
		return (0);
	return (1);
}

int	has_local_priority(t_coder	*coder)
{
	t_coder	*left;
	t_coder	*right;
	t_data	*data;
	int		index;
	int		coders;

	index = coder->id - 1;
	coders = coder->config->number_of_coders;
	data = coder->config->data;
	left = &data->coders[(index - 1 + coders) % coders];
	right = &data->coders[(index + 1) % coders];
	if (strcmp(coder->config->scheduler, "fifo") == 0)
		return (check_fifo_priority(coder, left, right));
	else if (strcmp(coder->config->scheduler, "edf") == 0)
		return (check_edf_priority(coder, left, right));
	return (1);
}

int	is_front_of_queue(t_coder	*coder)
{
	t_heap	*heap;

	heap = &coder->config->data->wait_queue;
	if (heap->size > 0 && heap->nodes[0]->id == coder->id)
		return (1);
	return (0);
}

void	*coder_routine(void	*arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->config->number_of_coders == 1)
		return (one_coder(coder), NULL);
	while (!check_simulation_over(coder))
	{
		try_to_claim_dongles(coder);
		if (check_simulation_over(coder))
			break ;
		print_status(coder, "is compiling");
		usleep(coder->config->time_to_compile * 1000);
		release_dongles(coder);
		print_status(coder, "is debugging");
		usleep(coder->config->time_to_debug * 1000);
		print_status(coder, "is refactoring");
		usleep(coder->config->time_to_refactor * 1000);
	}
	return (NULL);
}
