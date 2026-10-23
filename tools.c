/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:14:54 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/23 15:55:53 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_wait_queue(t_queue	*head)
{
	t_queue	*curr;
	t_queue	*next;

	curr = head;
	while (curr)
	{
		next = curr->next;
		pthread_cond_destroy(&curr->cond);
		free(curr);
		curr = next;
	}
}

void	free_dongles(t_dongle	*dongles, int count)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < count)
	{
		free_wait_queue(dongles[i].wait_queue);
		pthread_mutex_destroy(&dongles[i].dongle_lock);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
	free(dongles);
}

void	free_all(t_data *data, int count)
{
	if (data->coders)
		free(data->coders);
	free_dongles(data->dongles, count);
	pthread_mutex_destroy(&data->config.simulation_lock);
	pthread_mutex_destroy(&data->config.print_lock);
}

void	print_status(t_coder	*coder, char	*status)
{
	if (check_simulation_over(coder) && strcmp(status, "burned out") != 0)
		return ;
	pthread_mutex_lock(&coder->config->print_lock);
	printf("%ld %ld %s\n",
		get_time_in_ms() - coder->config->start_time, coder->id, status);
	pthread_mutex_unlock(&coder->config->print_lock);
}

void	one_coder(t_coder	*coder)
{
	pthread_mutex_lock(&coder->left_dongle->dongle_lock);
	print_status(coder, "has taken a dongle");
	usleep(coder->config->time_to_burnout * 1000);
	pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
}
