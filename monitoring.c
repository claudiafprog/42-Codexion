/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 10:21:20 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/16 11:03:13 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_simulation_over(t_coder	*coder)
{
	int	status;

	pthread_mutex_lock(&coder->config->simulation_lock);
	status = coder->config->simulation_over;
	pthread_mutex_unlock(&coder->config->simulation_lock);
	return (status);
}

void	finish_simulation(t_data	*data)
{
	int	i;

	pthread_mutex_lock(&data->config.simulation_lock);
	data->config.simulation_over = 1;
	pthread_mutex_unlock(&data->config.simulation_lock);
	i = 0;
	while (i < data->config.number_of_coders)
	{
		pthread_mutex_lock(&data->dongles[i].dongle_lock);
		pthread_cond_broadcast(&data->dongles[i].cond);
		pthread_mutex_unlock(&data->dongles[i].dongle_lock);
		i++;
	}
}

void	one_coder(t_coder	*coder)
{
	pthread_mutex_lock(&coder->left_dongle->dongle_lock);
	print_status(coder, "has taken a dongle");
	usleep(coder->config->time_to_burnout * 1000);
	pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
}
