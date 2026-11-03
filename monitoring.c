/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 10:21:20 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/03 12:24:33 by clferrei         ###   ########.fr       */
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
	int	j;

	pthread_mutex_lock(&data->config.simulation_lock);
	data->config.simulation_over = 1;
	pthread_mutex_unlock(&data->config.simulation_lock);
	pthread_mutex_lock(&data->config.scheduler_mutex);
	j = 0;
	while (j < data->wait_queue.size)
	{
		if (data->wait_queue.nodes[j])
			pthread_cond_signal(&data->wait_queue.nodes[j]->cond);
		j++;
	}
	pthread_mutex_unlock(&data->config.scheduler_mutex);
}

int	check_burnout(t_data	*data)
{
	int		i;
	long	present_moment;
	long	deadline;

	i = 0;
	present_moment = get_time_in_ms();
	while (i < data->config.number_of_coders)
	{
		pthread_mutex_lock(&data->config.simulation_lock);
		deadline = data->coders[i].last_compile_time
			+ data->config.time_to_burnout;
		if (present_moment >= deadline
			&& !data->coders[i].left_dongle->is_taken)
		{
			pthread_mutex_unlock(&data->config.simulation_lock);
			finish_simulation(data);
			print_status(&data->coders[i], "burned out");
			return (1);
		}
		pthread_mutex_unlock(&data->config.simulation_lock);
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
	}
	if (all_compiled)
	{
		finish_simulation(data);
		return (1);
	}
	return (0);
}

void	*simulation_monitor(void	*arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (1)
	{
		if (check_simulation_over(data->coders))
			break ;
		if (check_burnout(data))
			break ;
		if (check_number_of_compilations(data))
			break ;
		usleep(10);
	}
	return (NULL);
}
