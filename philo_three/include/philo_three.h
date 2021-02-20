/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:22 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/20 15:42:12 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_THREE_H
# define PHILO_THREE_H
# include <stddef.h>
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <sys/time.h>
# include <semaphore.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>

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
	unsigned		id;
	struct timeval	last_eat_time;
	unsigned		eat_count;
	unsigned		dining_complete;
	sem_t			*forks;
	sem_t			*print_status;
	sem_t			*check_starvation;
}				t_philo;

typedef struct	s_monitor
{
	t_args		*args;
	t_philo		*ph;
	pthread_t	thread;
	sem_t		*print_status;
	sem_t		*check_starvation;
}				t_monitor;

size_t			ft_strlen(const char *str);
unsigned		ft_strcpy(char *dst, const char *src);
int				check_args(int argc, char **argv, t_args *args);
void			ft_putstr_fd(const char *str, int fd);
unsigned		atou_error(const char *str, int *error);
unsigned		ft_utoa(unsigned n, char *buffer);
void			ft_print_status(unsigned ms, unsigned id, const char *status);
unsigned		get_time_diff(struct timeval *start, struct timeval *curr);
void			open_semaphores(sem_t **forks, t_monitor *mon, t_args *args);
void			create_philo_proc(sem_t *forks, t_monitor *mon, t_args *args,
																pid_t *philos);
void			*dine_philo(void *vars);
void			*monitor_self(void *vars);
int				is_starving(t_philo *ph, t_args *args);
int				is_dining_complete(t_philo *ph, t_args *args);
void			increment_eat_count(t_philo *ph);
void			eat_spaghetti(t_philo *ph);
void			philo_sleep(t_philo *ph);
void			unlink_semaphores(void);
char			*create_sem_name(const char *str, unsigned id);

#endif
