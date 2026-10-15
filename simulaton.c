/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulaton.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/15 15:50:46 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_simulation_over(t_coder *coder)
{
	int	status;

	pthread_mutex_lock(&coder->config->simulation_lock);
	status = coder->config->simulation_over;
	pthread_mutex_unlock(&coder->config->simulation_lock);
	return (status);
}

void	print_status(t_coder	*coder, char	*status)
{
	pthread_mutex_lock(&coder->config->print_lock);
	printf("%ld %ld %s\n",
		get_time_in_ms() - coder->config->start_time, coder->id, status);
	pthread_mutex_unlock(&coder->config->print_lock);
}

void	simulation_monitor(t_data	*data)
{
	int		i;
	int		j;
	long	time;

	while (1)
	{
		i = 0;
		while (i < data->config.number_of_coders)
		{
			time = get_time_in_ms() - data->coders[i].last_compile_time;
			if (time > data->config.time_to_burnout)
			{
				pthread_mutex_lock(&data->config.simulation_lock);
				data->config.simulation_over = 1;
				pthread_mutex_unlock(&data->config.simulation_lock);
				j = 0;
				while (j < data->config.number_of_coders)
				{
					pthread_mutex_lock(&data->dongles[j].dongle_lock);
                    pthread_cond_broadcast(&data->dongles[j].cond);
                    pthread_mutex_unlock(&data->dongles[j].dongle_lock);
                    j++;
                }
				print_status(&data->coders[i], "burned out");
				return ;
			}
			i++;
		}
		usleep(1000);
	}
}

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	while (!check_simulation_over(coder) && (dongle->is_taken
		|| (get_time_in_ms() - dongle->last_release_time
			< coder->config->dongle_cooldown)))
		pthread_cond_wait(&dongle->cond, &dongle->dongle_lock);
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
	coder->last_compile_time = get_time_in_ms();
	if (coder->config->number_of_coders == 1)
	{
		pthread_mutex_lock(&coder->left_dongle->dongle_lock);
		print_status(coder, "has taken a dongle");
		usleep(coder->config->time_to_burnout * 1000);
		pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
		return (NULL);
	}
	while (!check_simulation_over(coder))
	{
		take_dongle(coder, coder->left_dongle);
		print_status(coder, "has taken a dongle");
		take_dongle(coder, coder->right_dongle);
		print_status(coder, "has taken a dongle");
		coder->last_compile_time = get_time_in_ms();
		print_status(coder, "is compiling");
		usleep(coder->config->time_to_compile * 1000);
		release_dongle(coder->left_dongle);
		release_dongle(coder->right_dongle);
		print_status(coder, "is debugging");
		usleep(coder->config->time_to_debug * 1000);
		print_status(coder, "is refactoring");
		usleep(coder->config->time_to_refactor * 1000);
	}
	return (NULL);
}

