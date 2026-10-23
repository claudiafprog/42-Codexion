/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 10:21:20 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/23 15:58:13 by clferrei         ###   ########.fr       */
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

int	check_burnout(t_data	*data)
{
	int		i;
	long	time;

	i = 0;
	while (i < data->config.number_of_coders)
	{
		pthread_mutex_lock(&data->config.simulation_lock);
		time = get_time_in_ms() - data->coders[i].last_compile_time;
		pthread_mutex_unlock(&data->config.simulation_lock);
		if (time > data->config.time_to_burnout)
		{
			print_status(&data->coders[i], "burned out");
			finish_simulation(data);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_number_of_compilations(t_data	*data)
{
	int	i;
	int	all_compiled;

	i = 0;
	all_compiled = 1;
	if (data->config.number_of_compiles_required == 0)
		return (0);
	while (i < data->config.number_of_coders)
	{
		pthread_mutex_lock(&data->config.simulation_lock);
		if (data->coders[i].compiles_count
			< data->config.number_of_compiles_required)
			all_compiled = 0;
		pthread_mutex_unlock(&data->config.simulation_lock);
		i++;
		if (all_compiled)
		{
			finish_simulation(data);
			return (1);
		}
	}
	return (0);
}

void	simulation_monitor(t_data	*data)
{
	while (!check_simulation_over(data->coders))
	{
		if (check_burnout(data))
			return ;
		if (check_number_of_compilations(data))
			return ;
		usleep(1000);
	}
}
