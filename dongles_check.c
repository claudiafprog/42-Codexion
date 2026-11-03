/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_check.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 09:46:25 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/03 12:38:30 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	are_dongles_available(t_coder	*coder)
{
	long	present_moment;

	present_moment = get_time_in_ms();
	if (coder->left_dongle->is_taken || coder->right_dongle->is_taken)
		return (0);
	if (coder->config->dongle_cooldown > 0)
	{
		if (present_moment < coder->left_dongle->last_release_time)
			return (0);
		if (present_moment < coder->right_dongle->last_release_time)
			return (0);
	}
	return (1);
}

void	make_coder_wait(t_coder	*coder)
{
	t_heap	*heap;

	heap = &coder->config->data->wait_queue;
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

void	atomic_take_dongles(t_coder	*coder)
{
	t_heap	*heap;

	heap = &coder->config->data->wait_queue;
	if (coder->waiting)
	{
		heap_prio_remove(heap, coder, coder->config);
		coder->waiting = 0;
	}
	coder->left_dongle->is_taken = 1;
	coder->right_dongle->is_taken = 1;
	pthread_mutex_lock(&coder->config->simulation_lock);
	coder->last_compile_time = get_time_in_ms();
	coder->compiles_count++;
	pthread_mutex_unlock(&coder->config->simulation_lock);
	pthread_mutex_unlock(&coder->config->scheduler_mutex);
	take_dongles(coder);
}

void	try_to_claim_dongles(t_coder	*coder)
{
	pthread_mutex_lock(&coder->config->scheduler_mutex);
	if (!coder->waiting)
		make_coder_wait(coder);
	pthread_mutex_unlock(&coder->config->scheduler_mutex);
	while (1)
	{
		if (check_simulation_over(coder))
			return ;
		pthread_mutex_lock(&coder->config->scheduler_mutex);
		if (check_simulation_over(coder))
		{
			pthread_mutex_unlock(&coder->config->scheduler_mutex);
			return ;
		}
		if (are_dongles_available(coder) && has_local_priority(coder))
		{
			atomic_take_dongles(coder);
			return ;
		}
		pthread_mutex_unlock(&coder->config->scheduler_mutex);
		usleep(1);
	}
}
