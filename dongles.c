/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/02 21:46:10 by claudiaferrei    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	assign_dongles(t_coder	*coder, t_dongle	**first,
	t_dongle	**second)
{
	if (coder->left_dongle > coder->right_dongle)
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
	else
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
}

void	take_dongles(t_coder	*coder)
{
	t_dongle	*first;
	t_dongle	*second;

	assign_dongles(coder, &first, &second);
	pthread_mutex_lock(&first->dongle_lock);
	print_status(coder, "has taken a dongle");
	pthread_mutex_lock(&second->dongle_lock);
	print_status(coder, "has taken a dongle");
}

int	are_dongles_available(t_coder	*coder)
{
	long	present_moment;

	present_moment = get_time_in_ms();
	if (coder->left_dongle->is_taken || coder->right_dongle->is_taken)
		return (0);
	if (coder->config->dongle_cooldown > 0)
	{
		if ((present_moment - coder->left_dongle->last_release_time)
			< coder->config->dongle_cooldown)
			return (0);
		if ((present_moment - coder->right_dongle->last_release_time)
			< coder->config->dongle_cooldown)
			return (0);
	}
	return (1);
}

void	try_to_claim_dongles(t_coder	*coder)
{
	t_heap	*heap;

	heap = &coder->config->data->wait_queue;
	pthread_mutex_lock(&coder->config->scheduler_mutex);
	if (!coder->waiting)
	{
		coder->waiting = 1;
		coder->node->ticket = get_next_ticket(coder->config);
		if (coder->compiles_count == 0)
			coder->node->deadline = coder->config->start_time
				+ coder->config->time_to_burnout;
		else
			coder->node->deadline = coder->last_compile_time
				+ coder->config->time_to_burnout;
		heap_append_scheduled(heap, coder->node, coder->config);
	}
	pthread_mutex_unlock(&coder->config->scheduler_mutex);
	while (!check_simulation_over(coder))
	{
		pthread_mutex_lock(&coder->config->scheduler_mutex);
		if (heap->size > 0 && heap->nodes[0]->id == coder->id)
		{
			if (are_dongles_available(coder))
			{
				heap_prio_remove(heap, coder, coder->config);
				coder->waiting = 0;
				coder->left_dongle->is_taken = 1;
				coder->right_dongle->is_taken = 1;
				pthread_mutex_lock(&coder->config->simulation_lock);
				coder->last_compile_time = get_time_in_ms();
				coder->compiles_count++;
				pthread_mutex_unlock(&coder->config->simulation_lock);
				pthread_mutex_unlock(&coder->config->scheduler_mutex);
				take_dongles(coder);
				return ;
			}
		}
		pthread_mutex_unlock(&coder->config->scheduler_mutex);
		usleep(100);
	}
}

void	release_dongles(t_coder	*coder)
{
	long	present_moment;

	present_moment = get_time_in_ms();
	pthread_mutex_lock(&coder->config->scheduler_mutex);
	coder->left_dongle->is_taken = 0;
	coder->right_dongle->is_taken = 0;
	coder->left_dongle->last_release_time = present_moment;
	coder->right_dongle->last_release_time = present_moment;
	pthread_mutex_unlock(&coder->config->scheduler_mutex);
	pthread_mutex_unlock(&coder->right_dongle->dongle_lock);
	pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
}
