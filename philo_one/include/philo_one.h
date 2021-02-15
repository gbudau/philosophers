/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:22 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/15 16:25:49 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H
# include <stddef.h>
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <sys/time.h>

enum		e_bool
{
	FALSE,
	TRUE
};

typedef struct	s_args
{
	struct timeval	start_time;
	int				limit_times_to_eat;
	unsigned		n_philos;
	unsigned		time_to_die;
	unsigned		time_to_eat;
	unsigned		time_to_sleep;
	unsigned		n_times_to_eat;
}				t_args;

typedef struct	s_philo
{
	t_args			*args;
	pthread_t		thread;
	unsigned		id;
	struct timeval	last_eat_time;
	unsigned		eat_count;
	unsigned		dining_complete;
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;
	pthread_mutex_t	*print_status;
	pthread_mutex_t	*check_starvation;
	pthread_mutex_t *check_dining_complete;
}				t_philo;

typedef struct	s_monitor
{
	t_args			*args;
	t_philo			*ph;
	pthread_t		thread;
	pthread_mutex_t	*check_starvation;
	pthread_mutex_t	print_status;
	pthread_mutex_t	*check_dining_complete;
	unsigned		dining_complete_all;
}				t_monitor;

int			check_args(int argc, char **argv, t_args *args);
size_t		ft_strlen(const char *str);
void		ft_putstr_fd(const char *str, int fd);
unsigned	atou_error(const char *str, int *error);
unsigned	ft_utoa(unsigned n, char *buffer);
void		ft_print_status(unsigned ms, unsigned id, const char *status);

#endif
