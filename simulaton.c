/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulaton.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/14 14:16:00 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_simulation_over(t_coder *coder)
{
	int	status;

	pthread_mutex_lock(&coder->config->lock);
	status = coder->config->simulation_over;
	pthread_mutex_unlock(&coder->config->lock);
	return (status);
}

void	print_status(t_coder	*coder, char	*status)
{
	pthread_mutex_lock(&coder->config->print_lock);
	printf("%ld %ld %s\n",
		get_time_in_ms() - coder->config->start_time, coder->id, status);
	pthread_mutex_unlock(&coder->config->print_lock);
}

void	*coder_routine(void	*arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	coder->last_compile_time = get_time_in_ms();
	if (coder->config->number_of_coders == 1)
	{
		pthread_mutex_lock(coder->left_dongle);
		print_status(coder, "has taken a dongle");
		usleep(coder->config->time_to_burnout * 1000);
		pthread_mutex_unlock(coder->left_dongle);
		return (NULL);
	}
	while (!check_simulation_over(coder))
	{
		pthread_mutex_lock(coder->left_dongle);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(coder->right_dongle);
		print_status(coder, "has taken a dongle");
		coder->last_compile_time = get_time_in_ms();
		print_status(coder, "is compiling");
		usleep(coder->config->time_to_compile * 1000);
		pthread_mutex_unlock(coder->left_dongle);
		pthread_mutex_unlock(coder->right_dongle);
		print_status(coder, "is debugging");
		usleep(coder->config->time_to_debug * 1000);
		print_status(coder, "is refactoring");
		usleep(coder->config->time_to_refactor * 1000);
	}
	return (NULL);
}

void	simulation_monitor(t_data	*data)
{
	int		i;
	long	time;

	while (1)
	{
		i = 0;
		while (i < data->config.number_of_coders)
		{
			time = get_time_in_ms() - data->coders[i].last_compile_time;
			if (time > data->config.time_to_burnout)
			{
				pthread_mutex_lock(&data->config.lock);
				data->config.simulation_over = 1;
				pthread_mutex_unlock(&data->config.lock);
				print_status(&data->coders[i], "burned out");
				return ;
			}
			i++;
		}
		usleep(1000);
	}
}
