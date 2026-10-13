/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 13:14:54 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/13 13:29:50 by clferrei         ###   ########.fr       */
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
