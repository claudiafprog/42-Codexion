/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 14:59:21 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/14 14:21:25 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_data	data;
	int		i;

	if (!init_all(&data, argc, argv))
		return (fprintf(stderr, "Error: Initialization failed.\n"), 1);
	data.config.start_time = get_time_in_ms();
	i = 0;
	while (i < data.config.number_of_coders)
	{
		data.coders[i].last_compile_time = data.config.start_time;
		i++;
	}
	i = 0;
	while (i < data.config.number_of_coders)
	{
		pthread_create(
			&data.coders[i].thread_id, NULL, coder_routine, &data.coders[i]);
		i++;
	}
	simulation_monitor(&data);
	i = 0;
	while (i < data.config.number_of_coders)
	{
		pthread_join(data.coders[i].thread_id, NULL);
		i++;
	}
	free_all(&data, data.config.number_of_coders);
	return (0);
}
