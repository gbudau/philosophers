/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:22 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/24 18:28:04 by gbudau           ###   ########.fr       */
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

enum		e_semaphore_flags
{
	SEM_OPEN_FLAG = O_CREAT | O_EXCL,
	SEM_MODE_FLAG = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
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
	unsigned		is_dining_complete;
	sem_t			*forks;
	sem_t			*print_status;
	sem_t			*check_starvation;
	sem_t			*check_dining_complete;
	sem_t			*check_has_forks;
	sem_t			*dining_complete;
}				t_philo;

typedef struct	s_monitor
{
	t_args		*args;
	t_philo		*ph;
	pthread_t	thread;
	sem_t		*print_status;
	sem_t		*check_starvation;
	sem_t		*check_dining_complete;
	sem_t		*check_has_forks;
}				t_monitor;

typedef struct	s_monitor_dining_complete
{
	pid_t		*philos;
	t_args		*args;
	sem_t		**dining_complete;
}				t_monitor_dining_complete;

typedef struct	s_status_philo
{
	unsigned	id;
	sem_t		*print_status;
	sem_t		*dining_complete;
}				t_status_philo;

size_t			ft_strlen(const char *str);
unsigned		ft_strcpy(char *dst, const char *src);
int				check_args(int argc, char **argv, t_args *args);
void			ft_putstr_fd(const char *str, int fd);
unsigned		atou_error(const char *str, int *error);
unsigned		ft_utoa(unsigned n, char *buffer);
void			ft_print_status(unsigned ms, unsigned id, const char *status);
unsigned		get_time_diff(struct timeval *start, struct timeval *curr);
void			open_semaphores(sem_t **forks, t_args *args,
											t_monitor_dining_complete *mon_dc);
void			create_philo_proc(sem_t *forks, t_args *args, pid_t *philos,
											t_monitor_dining_complete *mon_dc);
void			*dine_philo(void *vars);
int				is_starving(t_philo *ph, t_args *args);
int				is_dining_complete(t_philo *ph);
void			increment_eat_count(t_philo *ph);
void			eat_spaghetti(t_philo *ph);
void			philo_sleep(t_philo *ph);
char			*create_sem_name(const char *str, unsigned id);
void			wait_all_philos(pid_t *philos, t_args *args);
void			create_and_detach_monitor_thread(
											t_monitor_dining_complete *mon_dc);
int				clean_all_philos(pid_t	*philos, unsigned count, int ret);
int				allocate_memory(pid_t **philos, t_args *args,
										t_monitor_dining_complete *mon_dc);
sem_t			*sem_open_unlink(const char *name, int value);
void			ft_sleep_ms(unsigned ms);

#endif
