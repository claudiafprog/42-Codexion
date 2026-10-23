/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/23 15:58:35 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	long	time_since_release;
	t_queue	*node;

	node = NULL;
	pthread_mutex_lock(&dongle->dongle_lock);
	while (!check_simulation_over(coder))
	{
		time_since_release = get_time_in_ms() - dongle->last_release_time;
		if (!dongle->is_taken && time_since_release
			>= coder->config->dongle_cooldown)
			break ;
		if (!node)
		{
			node = init_waiting_line(coder);
			dongle->wait_queue = chosen_scheduler(coder->config,
					dongle->wait_queue, node);
		}
		pthread_cond_wait(&dongle->cond, &dongle->dongle_lock);
	}
	if (check_simulation_over(coder))
	{
		pthread_mutex_unlock(&dongle->dongle_lock);
		return ;
	}
	dongle->is_taken = 1;
	pthread_mutex_unlock(&dongle->dongle_lock);
}

void	release_dongle(t_dongle *dongle)
{
	t_queue	*next_node;

	pthread_mutex_lock(&dongle->dongle_lock);
	dongle->is_taken = 0;
	dongle->last_release_time = get_time_in_ms();
	if (dongle->wait_queue)
	{
		next_node = dongle->wait_queue;
		dongle->wait_queue = dongle->wait_queue->next;
		pthread_cond_signal(&next_node->cond);
		pthread_cond_destroy(&next_node->cond);
		free(next_node);
	}
	pthread_mutex_unlock(&dongle->dongle_lock);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	pthread_mutex_lock(&coder->config->simulation_lock);
	coder->last_compile_time = get_time_in_ms();
	pthread_mutex_unlock(&coder->config->simulation_lock);
	if (coder->config->number_of_coders == 1)
		return (one_coder(coder), NULL);
	while (!check_simulation_over(coder))
	{
		take_dongle(coder, coder->left_dongle);
		print_status(coder, "has taken a dongle");
		take_dongle(coder, coder->right_dongle);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->config->simulation_lock);
		coder->last_compile_time = get_time_in_ms();
		pthread_mutex_unlock(&coder->config->simulation_lock);
		print_status(coder, "is compiling");
		usleep(coder->config->time_to_compile * 1000);
		release_dongle(coder->left_dongle);
		release_dongle(coder->right_dongle);
		print_status(coder, "is debugging");
		usleep(coder->config->time_to_debug * 1000);
		print_status(coder, "is refactoring");
		usleep(coder->config->time_to_refactor * 1000);
	}
	coder->compiles_count++;
	return (NULL);
}
