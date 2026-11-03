/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 14:59:21 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/03 12:29:59 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coders_time(t_data	*data)
{
	int	i;

	data->config.start_time = get_time_in_ms();
	i = 0;
	while (i < data->config.number_of_coders)
	{
		data->coders[i].last_compile_time = data->config.start_time;
		if (data->coders[i].node)
			data->coders[i].node->deadline = data->config.start_time
				+ data->config.time_to_burnout;
		i++;
	}
}

static void	create_threads(t_data	*data)
{
	int	i;

	i = 0;
	while (i < data->config.number_of_coders)
	{
		pthread_create(&data->coders[i].thread_id, NULL,
			coder_routine, &data->coders[i]);
		i++;
	}
}

static void	join_threads(t_data	*data)
{
	int	i;

	i = 0;
	while (i < data->config.number_of_coders)
	{
		pthread_join(data->coders[i].thread_id, NULL);
		i++;
	}
}

int	main(int argc, char	**argv)
{
	t_data	data;

	if (!init_all(&data, argc, argv))
		return (fprintf(stderr, "Error: Initialization failed.\n"), 1);
	init_coders_time(&data);
	create_threads(&data);
	simulation_monitor(&data);
	join_threads(&data);
	usleep(10000);
	free_all(&data, data.config.number_of_coders);
	return (0);
}
