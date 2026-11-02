/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clferrei <clferrei@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:44:38 by clferrei          #+#    #+#             */
/*   Updated: 2026/11/02 17:00:54 by clferrei         ###   ########.fr       */
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

# define MY_INT_MAX 2147483647

/* system structs */
typedef struct s_data	t_data;

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
	long			next_ticket;
	t_data			*data;
	pthread_mutex_t	simulation_lock;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	scheduler_mutex;
}	t_config;

typedef struct s_node
{
	long			id;
	long			arrival_time;
	long			deadline;
	long			ticket;
	pthread_cond_t	cond;
}	t_node;

typedef struct s_heap
{
	t_node			**nodes;
	int				size;
	int				capacity;
}	t_heap;

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
	int				waiting;
	t_node			*node;
}	t_coder;

typedef struct s_data
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	t_heap			wait_queue;
}	t_data;

/* parsing */
int		clean_input(int argc, char	**argv, t_config *config);
/* scheduler / time */
long	get_time_in_ms(void);
long    get_next_ticket(t_config    *config);
int		heap_append_scheduled(t_heap	*heap, t_node	*new_node, t_config	*config);
t_node	*heap_pop(t_heap	*heap, t_config	*config);
void	heap_prio_remove(t_heap	*heap, t_coder	*coder, t_config	*config);
/* init */
t_node	*init_waiting_line(t_coder	*coder);
int		init_all(t_data	*data, int argc, char	**argv);
/* free */
void	free_dongles(t_dongle	*dongles, int count);
void	free_all(t_data *data, int count);
void	init_destroy(t_data	*data);
/* tools */
void	print_status(t_coder	*coder, char	*status);
void	one_coder(t_coder	*coder);
/* monitoring */
int		check_simulation_over(t_coder *coder);
void	finish_simulation(t_data	*data);
/* simulation */
int		has_local_priority(t_coder	*coder);
void	*coder_routine(void *arg);
void	*simulation_monitor(void	*arg);
#endif
