/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 10:05:47 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/13 13:29:30 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

pthread_mutex_t	*init_dongles(int number_of_coders)
{
	pthread_mutex_t	*dongles;
	int				i;

	dongles = malloc(sizeof(pthread_mutex_t) * number_of_coders);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < number_of_coders)
	{
		if (pthread_mutex_init(&dongles[i], NULL) != 0)
		{
			free_dongles(dongles, i);
			return (NULL);
		}
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
		return (0);
	data->dongles = init_dongles(data->config.number_of_coders);
	if (!data->dongles)
	{
		free(data->coders);
		return (0);
	}
	data->coders = init_coders(data);
	if (!data->coders)
	{
		free_dongles(data->dongles, data->config.number_of_coders);
		return (0);
	}
	return (1);
}
