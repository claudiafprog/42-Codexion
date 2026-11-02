/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 10:05:47 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/02 16:59:09 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_single_dongle(t_dongle	*dongles, int i)
{
	if (pthread_mutex_init(&dongles[i].dongle_lock, NULL) != 0)
	{
		free_dongles(dongles, i);
		return (0);
	}
	if (pthread_cond_init(&dongles[i].cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongles[i].dongle_lock);
		free_dongles(dongles, i);
		return (0);
	}
	dongles[i].last_release_time = 0;
	dongles[i].is_taken = 0;
	return (1);
}

t_dongle	*init_dongles(int number_of_coders)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle) * number_of_coders);
	if (!dongles)
		return (NULL);
	memset(dongles, 0, sizeof(t_dongle) * number_of_coders);
	i = 0;
	while (i < number_of_coders)
	{
		if (!init_single_dongle(dongles, i))
			return (NULL);
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
	memset(coders, 0, sizeof(t_coder) * data->config.number_of_coders);
	i = 0;
	while (i < data->config.number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].last_compile_time = 0;
		coders[i].config = &data->config;
		coders[i].left_dongle = &data->dongles[i];
		coders[i].right_dongle = &data->dongles[(i + 1)
			% data->config.number_of_coders];
		coders[i].waiting = 0;
		coders[i].node = init_waiting_line(&coders[i]);
		i++;
	}
	return (coders);
}

t_node	*init_waiting_line(t_coder	*coder)
{
	t_node	*new_node;

	new_node = malloc(sizeof(t_node));
	if (!new_node)
		return (NULL);
	memset(new_node, 0, sizeof(t_node));
	new_node->id = coder->id;
	new_node->arrival_time = get_time_in_ms();
	new_node->ticket = -1;
	if (coder->last_compile_time == 0)
		new_node->deadline = new_node->arrival_time
			+ coder->config->time_to_burnout;
	else
		new_node->deadline = coder->last_compile_time
			+ coder->config->time_to_burnout;
	pthread_cond_init(&new_node->cond, NULL);
	return (new_node);
}

int	init_all(t_data *data, int argc, char **argv)
{
	if (!clean_input(argc, argv, &data->config))
		return (0);
	data->config.next_ticket = 0;
	data->config.data = data;
	data->wait_queue.capacity = data->config.number_of_coders * 2;
	data->wait_queue.size = 0;
	data->wait_queue.nodes = malloc(sizeof(t_node *)
			* (data->config.number_of_coders * 2));
	if (!data->wait_queue.nodes)
		return (0);
	memset(data->wait_queue.nodes, 0, sizeof(t_node *)
		* data->config.number_of_coders * 2);
	data->dongles = init_dongles(data->config.number_of_coders);
	if (!data->dongles)
		return (0);
	pthread_mutex_init(&data->config.simulation_lock, NULL);
	pthread_mutex_init(&data->config.print_lock, NULL);
	pthread_mutex_init(&data->config.scheduler_mutex, NULL);
	data->coders = init_coders(data);
	if (!data->coders)
		return (init_destroy(data), 0);
	return (1);
}
