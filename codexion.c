/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 14:59:21 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/13 14:43:12 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	config;

	config.start_time = get_time_in_ms();
	if (!clean_input(argc, argv, &config))
	{
		printf("Error: Insert valid input arguments.\n");
		return (1);
	}
	printf("Number of coders: %ld\n", config.number_of_coders);
	printf("Time to burnout: %ld\n", config.time_to_burnout);
	printf("Time to compile: %ld\n", config.time_to_compile);
	printf("Scheduler: %s\n", config.scheduler);
	printf("Time in ms is: %ld\n", config.start_time);
	return (0);
}
