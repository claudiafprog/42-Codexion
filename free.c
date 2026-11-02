/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 16:55:57 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/02 17:00:01 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_dongles(t_dongle	*dongles, int count)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&dongles[i].dongle_lock);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
	free(dongles);
}

void	free_all(t_data	*data, int count)
{
	int	i;

	i = 0;
	if (data->coders)
	{
		while (i < count)
		{
			if (data->coders[i].node)
			{
				pthread_cond_destroy(&data->coders[i].node->cond);
				free(data->coders[i].node);
				data->coders[i].node = NULL;
			}
			i++;
		}
		free(data->coders);
	}
	free_dongles(data->dongles, count);
	if (data->wait_queue.nodes)
		free(data->wait_queue.nodes);
	pthread_mutex_destroy(&data->config.simulation_lock);
	pthread_mutex_destroy(&data->config.print_lock);
	pthread_mutex_destroy(&data->config.scheduler_mutex);
}

void	init_destroy(t_data	*data)
{
	free(data->wait_queue.nodes);
	free_dongles(data->dongles, data->config.number_of_coders);
	pthread_mutex_destroy(&data->config.simulation_lock);
	pthread_mutex_destroy(&data->config.print_lock);
	pthread_mutex_destroy(&data->config.scheduler_mutex);
}
