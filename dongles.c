/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:46:11 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/03 12:48:50 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	assign_dongles(t_coder	*coder, t_dongle	**first,
	t_dongle	**second)
{
	if (coder->left_dongle > coder->right_dongle)
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
	else
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
}

void	take_dongles(t_coder	*coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (check_simulation_over(coder))
		return ;
	assign_dongles(coder, &first, &second);
	pthread_mutex_lock(&first->dongle_lock);
	if (check_simulation_over(coder))
	{
		pthread_mutex_unlock(&first->dongle_lock);
		return ;
	}
	print_status(coder, "has taken a dongle");
	pthread_mutex_lock(&second->dongle_lock);
	print_status(coder, "has taken a dongle");
}

void	release_dongles(t_coder	*coder)
{
	long	available_at;

	available_at = get_time_in_ms() + coder->config->dongle_cooldown;
	pthread_mutex_lock(&coder->config->scheduler_mutex);
	coder->left_dongle->is_taken = 0;
	coder->right_dongle->is_taken = 0;
	coder->left_dongle->last_release_time = available_at;
	coder->right_dongle->last_release_time = available_at;
	pthread_mutex_unlock(&coder->config->scheduler_mutex);
	pthread_mutex_unlock(&coder->right_dongle->dongle_lock);
	pthread_mutex_unlock(&coder->left_dongle->dongle_lock);
}
