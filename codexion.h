/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:44:38 by clferrei          #+#    #+#             */
/*   Updated: 2026/10/16 11:04:53 by clferrei         ###   ########.fr       */
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

/* system structs */
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
	long			simulation_over;
	pthread_mutex_t	simulation_lock;
	pthread_mutex_t	print_lock;
}	t_config;

typedef struct s_dongle
{
	pthread_mutex_t	dongle_lock;
	pthread_cond_t	cond;
	long			last_release_time;
	long			is_taken;
}	t_dongle;

typedef struct s_coder
{
	long			id;
	long			last_compile_time;
	long			compiles_count;
	pthread_t		thread_id;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_config		*config;
}	t_coder;

typedef struct s_data
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_data;

/* parsing */
int		clean_input(int argc, char	**argv, t_config *config);
/* time */
long	get_time_in_ms(void);
/* init */
int		init_all(t_data	*data, int argc, char	**argv);
/* free */
void	free_dongles(t_dongle	*dongles, int count);
void	free_all(t_data *data, int count);
/* monitoring */
int		check_simulation_over(t_coder *coder);
void	finish_simulation(t_data	*data);
void	one_coder(t_coder	*coder);
/* simulation */
void	print_status(t_coder	*coder, char	*status);
void	*coder_routine(void *arg);
void	simulation_monitor(t_data	*data);
#endif
