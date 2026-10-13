/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/13 15:25:21 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int check_simulation_over(t_coder *coder)
{
    int status;

    pthread_mutex_lock(&coder->config->lock);
    status = coder->config->simulation_over;
    pthread_mutex_unlock(&coder->config->lock);
    return (status);
}

void *coder_routine(void *arg)
{
	t_coder	*coder;
	long	relative_time;

	coder = (t_coder *)arg;
	while (!check_simulation_over(coder))
	{
		pthread_mutex_lock(coder->left_dongle);
		printf("%ld %ld has taken a dongle.\n",
			get_time_in_ms() - coder->config->start_time, coder->id);
		pthread_mutex_lock(coder->right_dongle);
		printf("%ld %ld has taken a dongle.\n",
			get_time_in_ms() - coder->config->start_time, coder->id);
		printf("%ld %ld is compiling.\n",
            get_time_in_ms() - coder->config->start_time, coder->id);
		coder->last_compile_time = get_time_in_ms();
		usleep(coder->config->time_to_compile * 1000);
		pthread_mutex_unlock(coder->left_dongle);
        pthread_mutex_unlock(coder->right_dongle);

	}
	return (NULL);
}
