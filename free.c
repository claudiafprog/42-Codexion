/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:14:54 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/14 13:50:41 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_dongles(pthread_mutex_t	*dongles, int count)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&dongles[i]);
		i++;
	}
	free(dongles);
}

void	free_all(t_data *data, int count)
{
	free(data->coders);
	free_dongles(data->dongles, count);
	pthread_mutex_destroy(&data->config.lock);
	pthread_mutex_destroy(&data->config.print_lock);
}
