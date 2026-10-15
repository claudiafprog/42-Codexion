/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 10:05:47 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/15 12:15:40 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	*init_dongles(int number_of_coders)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle) * number_of_coders);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < number_of_coders)
	{
		if (pthread_mutex_init(&dongles[i].dongle_lock, NULL) != 0)
		{
			free_dongles(dongles, i);
			return (NULL);
		}
		if (pthread_cond_init(&dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&dongles[i].dongle_lock);
			free_dongles(dongles, i);
			return (NULL);
		}
		dongles[i].last_release_time = 0;
		dongles[i].is_taken = 0;
		i++;
	}
	return (dongles);
}

t_coder	*init_coders(t_data	*data)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * data->config.number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < data->config.number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].last_compile_time = 0;
		coders[i].config = &data->config;
		coders[i].left_dongle = &data->dongles[i];
		coders[i].right_dongle = &data->dongles[(i + 1)
			% data->config.number_of_coders];
		i++;
	}
	return (coders);
}

int	init_all(t_data *data, int argc, char **argv)
{
	if (!clean_input(argc, argv, &data->config))
		return (printf("Error: Insert valid input arguments.\n"), 0);
	data->dongles = init_dongles(data->config.number_of_coders);
	if (!data->dongles)
		return (0);
	pthread_mutex_init(&data->config.simulation_lock, NULL);
	pthread_mutex_init(&data->config.print_lock, NULL);
	data->coders = init_coders(data);
	if (!data->coders)
	{
		free_dongles(data->dongles, data->config.number_of_coders);
		pthread_mutex_destroy(&data->config.simulation_lock);
		pthread_mutex_destroy(&data->config.print_lock);
		return (0);
	}
	return (1);
}
