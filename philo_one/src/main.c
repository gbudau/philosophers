/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/14 14:57:24 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"
#include <stdio.h>

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
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;
	int				*stop_simulation;
}				t_philo;

typedef struct	s_monitor
{
	pthread_t	thread;
	t_args		*args;
	t_philo		*ph;
	int			stop_simulation;
}				t_monitor;

int check_args_n(int argc, char *str)
{
	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Usage: ", STDERR_FILENO);
		ft_putstr_fd(str, STDERR_FILENO);
		ft_putstr_fd("number_of_philosophers time_to_die "
				"time_to_eat time_to_sleep "
				"[number_of_times_each_philosopher_must_eat]\n", STDERR_FILENO);
		return (-1);
	}
	return (0);
}

int	fill_var(unsigned n, t_args *args, size_t i)
{
	if (i == 0)
		if (n < 2)
		{
			ft_putstr_fd("Atleast two philosophers are required\n", STDERR_FILENO);
			return (-1);
		}
		else
			args->n_philos = n;
	else if (i == 1)
		args->time_to_die = n;
	else if (i == 2)
		args->time_to_eat = n;
	else if (i == 3)
		args->time_to_sleep = n;
	else if (i == 4)
		args->n_times_to_eat = n;
	return (0);
}

int check_valid_args(char **argv, t_args *args)
{
	size_t		i;
	int			error;
	unsigned	n;

	i = 0;
	while (argv[i])
	{
		error = TRUE;
		n = atou_error(argv[i], &error);
		if (error || fill_var(n, args, i) == -1)
		{
			ft_putstr_fd("Invalid argument: ", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putstr_fd("\n", STDERR_FILENO);
			return (-1);
		}
		i++;
	}
	return (0);
}

unsigned	get_time_diff(struct timeval *start, struct timeval *curr)
{
	unsigned	ms;

	ms = (curr->tv_sec - start->tv_sec) * 1000;
	ms += (curr->tv_usec - start->tv_usec) / 1000;
	return (ms);
}

int		pickup_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	gettimeofday(&curr, NULL);
	printf("%u -> %u is thinking\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_lock(ph->first_fork);
	gettimeofday(&curr, NULL);
	printf("%u -> %u has taken a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_lock(ph->second_fork);
	gettimeofday(&curr, NULL);
	printf("%u -> %u has taken a fork\n", get_time_diff(start, &curr), ph->id);
	return (0);
}

void	drop_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	pthread_mutex_unlock(ph->second_fork);
	gettimeofday(&curr, NULL);
	printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->first_fork);
	printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
}

void	*dine_philo(void *vars)
{
	t_philo			*ph;
	struct timeval	curr;
	struct timeval	*start;

	ph = vars;
	start = &ph->args->start_time;
	while (TRUE)
	{
		pickup_forks(ph);
		gettimeofday(&curr, NULL);
		ph->last_eat_time = curr;
		printf("%u -> %u is eating\n", get_time_diff(start, &curr), ph->id);
		usleep(ph->args->time_to_eat * 1000);
		drop_forks(ph);
		gettimeofday(&curr, NULL);
		printf("%u -> %u is sleeping\n", get_time_diff(start, &curr), ph->id);
		usleep(ph->args->time_to_sleep * 1000);
	}
	return (NULL);
}

void	*monitor_philos(void *vars)
{
	struct timeval	curr;
	unsigned		time_diff;
	t_args			*args;
	t_philo			*ph;
	t_monitor		*mon;

	mon = vars;
	ph = mon->ph;
	args = mon->args;
	while (TRUE)
	{
		for (unsigned i = 0; i < args->n_philos; i++)
		{
			gettimeofday(&curr, NULL);
			time_diff = get_time_diff(&ph->last_eat_time, &curr);
			if (time_diff >= ph[i].args->time_to_die)
			{
				mon->stop_simulation = 1;
				return (&ph[i].id);
			}
		}
		usleep(5000);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_args			args;
	t_philo			*ph;
	t_monitor		mon;
	pthread_mutex_t	*forks;
	unsigned		*id;
	struct timeval	curr;

	memset(&args, 0, sizeof(args));
	if (check_args_n(argc, argv[0]) == -1)
		return (1);
	if (argc == 6)
		args.limit_times_to_eat = TRUE;
	if (check_valid_args(argv + 1, &args) == -1)
		return (1);
	forks = malloc(sizeof(*forks) * args.n_philos);
	ph = malloc(sizeof(*ph) * args.n_philos);
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_mutex_init(&forks[i], NULL);
	mon.stop_simulation = 0;
	gettimeofday(&args.start_time, NULL);
	for (unsigned i = 0; i < args.n_philos; i++)
	{
		ph[i].id = i + 1;
		ph[i].args = &args;
		ph[i].first_fork = &forks[(i + i % 2) % args.n_philos];
		ph[i].second_fork = &forks[(i + !(i % 2)) % args.n_philos];
		ph[i].last_eat_time = args.start_time;
		ph[i].stop_simulation = &mon.stop_simulation;
		pthread_create(&ph[i].thread, NULL, &dine_philo, &ph[i]);
	}
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_detach(ph[i].thread);
	mon.args = &args;
	mon.ph = ph;
	pthread_create(&mon.thread, NULL, &monitor_philos, &mon);
	pthread_join(mon.thread, (void **)&id);
	gettimeofday(&curr, NULL);
	printf("%u -> %u died\n", get_time_diff(&args.start_time, &curr), *id);
	return (0);
	/*
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_mutex_destroy(&forks[i]);
	free(ph);
	free(forks);
	return (0);
	*/
}
