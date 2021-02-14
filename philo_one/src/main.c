/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/14 21:54:30 by gbudau           ###   ########.fr       */
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
	pthread_mutex_lock(ph->print_status);
	printf("%u -> %u is thinking\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_lock(ph->first_fork);
	gettimeofday(&curr, NULL);
	pthread_mutex_lock(ph->print_status);
	printf("%u -> %u has taken a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_lock(ph->second_fork);
	gettimeofday(&curr, NULL);
	pthread_mutex_lock(ph->print_status);
	printf("%u -> %u has taken a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->print_status);
	return (0);
}

void	drop_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	pthread_mutex_unlock(ph->second_fork);
	gettimeofday(&curr, NULL);
	pthread_mutex_lock(ph->print_status);
	printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_unlock(ph->first_fork);
	pthread_mutex_lock(ph->print_status);
	printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->print_status);
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
		pthread_mutex_lock(ph->check_starvation);
		ph->last_eat_time = curr;
		if (ph->args->limit_times_to_eat)
		{
			ph->eat_count++;
			if (ph->eat_count == ph->args->n_times_to_eat)
			{
				pthread_mutex_lock(ph->check_dining_complete);
				ph->dining_complete = TRUE;
				pthread_mutex_unlock(ph->check_dining_complete);
			}
		}
		pthread_mutex_unlock(ph->check_starvation);
		pthread_mutex_lock(ph->print_status);
		printf("%u -> %u is eating\n", get_time_diff(start, &curr), ph->id);
		pthread_mutex_unlock(ph->print_status);
		usleep(ph->args->time_to_eat * 1000);
		drop_forks(ph);
		gettimeofday(&curr, NULL);
		pthread_mutex_lock(ph->print_status);
		printf("%u -> %u is sleeping\n", get_time_diff(start, &curr), ph->id);
		pthread_mutex_unlock(ph->print_status);
		usleep(ph->args->time_to_sleep * 1000);
		usleep(1000);
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
			pthread_mutex_lock(ph[i].check_starvation);
			time_diff = get_time_diff(&ph[i].last_eat_time, &curr);
			if (time_diff >= ph[i].args->time_to_die)
				return (&ph[i].id);
			pthread_mutex_unlock(ph[i].check_starvation);
		}
		if (args->limit_times_to_eat)
		{
			mon->dining_complete_all = TRUE;
			for (unsigned i = 0; i < args->n_philos; i++)
			{
				pthread_mutex_lock(ph[i].check_dining_complete);
				if (ph[i].dining_complete == FALSE)
				{
					mon->dining_complete_all = FALSE;
					pthread_mutex_unlock(ph[i].check_dining_complete);
					break ;
				}
				pthread_mutex_unlock(ph[i].check_dining_complete);
			}
			if (mon->dining_complete_all == TRUE)
				return (NULL);
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
	memset(&mon, 0, sizeof(mon));
	if (check_args_n(argc, argv[0]) == -1)
		return (1);
	if (argc == 6)
		args.limit_times_to_eat = TRUE;
	if (check_valid_args(argv + 1, &args) == -1)
		return (1);
	if (args.limit_times_to_eat && !args.n_times_to_eat)
		return (0);
	forks = malloc(sizeof(*forks) * args.n_philos);
	mon.check_starvation = malloc(sizeof(*mon.check_starvation) * args.n_philos);
	if (args.limit_times_to_eat)
		mon.check_dining_complete = malloc(sizeof(*mon.check_dining_complete) * args.n_philos);
	ph = malloc(sizeof(*ph) * args.n_philos);
	memset(ph, 0, sizeof(*ph) * args.n_philos);
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_mutex_init(&forks[i], NULL);
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_mutex_init(&mon.check_starvation[i], NULL);
	pthread_mutex_init(&mon.print_status, NULL);
	gettimeofday(&args.start_time, NULL);
	for (unsigned i = 0; i < args.n_philos; i++)
	{
		ph[i].id = i + 1;
		ph[i].args = &args;
		ph[i].first_fork = &forks[(i + i % 2) % args.n_philos];
		ph[i].second_fork = &forks[(i + !(i % 2)) % args.n_philos];
		ph[i].last_eat_time = args.start_time;
		ph[i].check_starvation = &mon.check_starvation[i];
		if (args.limit_times_to_eat)
			ph[i].check_dining_complete = &mon.check_dining_complete[i];
		ph[i].print_status = &mon.print_status;
		pthread_create(&ph[i].thread, NULL, &dine_philo, &ph[i]);
	}
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_detach(ph[i].thread);
	mon.args = &args;
	mon.ph = ph;
	pthread_create(&mon.thread, NULL, &monitor_philos, &mon);
	pthread_join(mon.thread, (void **)&id);
	pthread_mutex_lock(&mon.print_status);
	if (id)
	{
		gettimeofday(&curr, NULL);
		printf("%u -> %u died\n", get_time_diff(&args.start_time, &curr), *id);
	}
	return (0);
}
