/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:44:38 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/13 15:25:23 by clferrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

/* parsig */
typedef struct s_config
{
	long			number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	long			number_of_compiles_required;
	long			dongle_cooldown;
	char			*scheduler;
	long			start_time;
	bool			simulation_over;
	pthread_mutex_t	lock;
}	t_config;

int		read_input(int argc, char	**argv, t_config *config);
void	convert_input(int argc, char	**argv, t_config *config);
int		clean_input(int argc, char	**argv, t_config *config);
/* time */
long	get_time_in_ms(void);
/* init */
typedef struct s_coder
{
	long			id;
	long			last_compile_time;
	pthread_t		thread_id;
	pthread_mutex_t	*left_dongle;
	pthread_mutex_t	*right_dongle;
	t_config		*config;
}	t_coder;

typedef struct s_data
{
	t_config		*config;
	t_coder			*coders;
	pthread_mutex_t	*dongles;
}	t_data;
int		init_all(t_data	*data, int argc, char	**argv);
/* free */
void	free_dongles(pthread_mutex_t	*dongles, int count);
#endif
