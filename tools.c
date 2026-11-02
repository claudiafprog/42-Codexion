/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:14:54 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/02 17:02:11 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder	*coder, char	*status)
{
	pthread_mutex_lock(&coder->config->print_lock);
	if (!coder->config->simulation_over || strcmp(status, "burned out") == 0)
		printf("%ld %ld %s\n",
			get_time_in_ms() - coder->config->start_time, coder->id, status);
	pthread_mutex_unlock(&coder->config->print_lock);
}

void	one_coder(t_coder	*coder)
{
	pthread_mutex_lock(&coder->left_dongle->dongle_lock);
	print_status(coder, "has taken a dongle");
	usleep(coder->config->time_to_burnout * 1000);
	pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
}

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}
