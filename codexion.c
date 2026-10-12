/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 14:59:21 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/12 15:33:08 by claudiaferrei    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char	**argv)
{
	t_config	config;

	if (!read_input(argc, argv))
	{
		printf("Error: Insert valid input arguments.\n");
		return (1);
	}
	convert_input(argc, argv, &config);
	if (!boundaries_validation(&config))
	{
		printf("Error: Insert valid input arguments.\n");
        return (1);
	}
	printf("Number of coders: %ld\n", config.number_of_coders);
	printf("Time to burnout: %ld\n", config.time_to_burnout);
	printf("Time to compile: %ld\n", config.time_to_compile);
	printf("Scheduler: %s\n", config.scheduler);
	return (0);
}
