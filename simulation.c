/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/19 11:10:24 by claudiaferrei    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder	*coder, char	*status)
{
	if (check_simulation_over(coder) && strcmp(status, "burned out") != 0)
		return ;
	pthread_mutex_lock(&coder->config->print_lock);
	printf("%ld %ld %s\n",
		get_time_in_ms() - coder->config->start_time, coder->id, status);
	pthread_mutex_unlock(&coder->config->print_lock);
}

void	simulation_monitor(t_data	*data)
{
	int		i;
	long	time;
	int		all_compiled;

	while (1)
	{
		i = 0;
		all_compiled = 1;
		while (i < data->config.number_of_coders)
		{
			pthread_mutex_lock(&data->config.simulation_lock);
			time = get_time_in_ms() - data->coders[i].last_compile_time;
			pthread_mutex_unlock(&data->config.simulation_lock);
			if (time > data->config.time_to_burnout)
			{
				finish_simulation(data);
				print_status(&data->coders[i], "burned out");
				return ;
			}
			if (data->config.number_of_compiles_required > 0
				&& data->coders[i].compiles_count
				< data->config.number_of_compiles_required)
				all_compiled = 0;
			i++;
		}
		if (data->config.number_of_compiles_required > 0
			&& all_compiled)
		{
			finish_simulation(data);
			return ;
		}
		usleep(1000);
	}
}

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	long	time_since_release;

	pthread_mutex_lock(&dongle->dongle_lock);
	while (!check_simulation_over(coder))
	{
		time_since_release = get_time_in_ms() - dongle->last_release_time;
		if (!dongle->is_taken && time_since_release
			>= coder->config->dongle_cooldown)
			break ;
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
	pthread_mutex_lock(&dongle->dongle_lock);
	dongle->is_taken = 0;
	dongle->last_release_time = get_time_in_ms();
	pthread_cond_broadcast(&dongle->cond);
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
