/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:55:53 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/29 09:46:16 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_valid_number(char	*str)
{
	int	i;

	if (!str || str[0] == '\0')
		return (0);
	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

long	ft_atol(const char	*str)
{
	long	i;
	long	sign;
	long	result;

	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	sign = 1;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

int	read_input(int argc, char	**argv)
{
	int	i;
	int	n_arg;

	if (argc != 9)
		return (0);
	i = 1;
	n_arg = 8;
	while (i < n_arg)
	{
		if (!is_valid_number(argv[i]))
			return (0);
		i++;
	}
	if (strcmp(argv[n_arg], "fifo") != 0 && strcmp(argv[n_arg], "edf") != 0)
		return (0);
	return (1);
}

int	convert_input(int argc, char	**argv, t_config *config)
{
	(void)argc;
	config->number_of_coders = ft_atol(argv[1]);
	config->time_to_burnout = ft_atol(argv[2]);
	config->time_to_compile = ft_atol(argv[3]);
	config->time_to_debug = ft_atol(argv[4]);
	config->time_to_refactor = ft_atol(argv[5]);
	config->number_of_compiles_required = ft_atol(argv[6]);
	config->dongle_cooldown = ft_atol(argv[7]);
	config->scheduler = argv[8];
	config->simulation_over = 0;
	if ((config->number_of_coders <= 0 || config->number_of_coders > MY_INT_MAX)
		|| (config->time_to_burnout <= 0
			|| config->time_to_burnout > MY_INT_MAX)
		|| (config->time_to_compile <= 0
			|| config->time_to_compile > MY_INT_MAX)
		|| (config->time_to_debug <= 0 || config->time_to_debug > MY_INT_MAX)
		|| (config->time_to_refactor <= 0
			|| config->time_to_refactor > MY_INT_MAX)
		|| (config->number_of_compiles_required < 0
			|| config->number_of_compiles_required > MY_INT_MAX)
		|| (config->dongle_cooldown < 0
			|| config->dongle_cooldown > MY_INT_MAX))
		return (0);
	return (1);
}

int	clean_input(int argc, char	**argv, t_config *config)
{
	if (!read_input(argc, argv))
		return (0);
	if (!convert_input(argc, argv, config))
		return (0);
	return (1);
}
