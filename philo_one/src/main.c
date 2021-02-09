/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/09 23:05:23 by gbudau           ###   ########.fr       */
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
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
}				t_philo;

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

int		lock_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;
	int				left_locked;
	int				right_locked;

	start = &ph->args->start_time;
	gettimeofday(&curr, NULL);
	if (get_time_diff(&ph->last_eat_time, &curr) >= ph->args->time_to_die)
		return (-1);
	left_locked = !pthread_mutex_lock(ph->left_fork);
	if (left_locked)
	{
		gettimeofday(&curr, NULL);
		printf("%u -> %u has taken a fork\n", get_time_diff(start, &curr), ph->id);
		if (get_time_diff(&ph->last_eat_time, &curr) >= ph->args->time_to_die)
		{
			printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
			pthread_mutex_unlock(ph->left_fork);
			return (-1);
		}
		right_locked = !pthread_mutex_lock(ph->right_fork);
		gettimeofday(&curr, NULL);
		if (right_locked)
		{
			printf("%u -> %u has taken a fork\n", get_time_diff(start, &curr), ph->id);
			return (0);
		}
		else
		{
			printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
			pthread_mutex_unlock(ph->left_fork);
		}
	}
	return (0);
}

void	unlock_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	pthread_mutex_unlock(ph->right_fork);
	gettimeofday(&curr, NULL);
	printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
	pthread_mutex_unlock(ph->left_fork);
	gettimeofday(&curr, NULL);
	printf("%u -> %u has dropped a fork\n", get_time_diff(start, &curr), ph->id);
}

void	*run_philo(void *vars)
{
	t_philo			*ph;
	struct timeval	curr;
	struct timeval	*start;
	unsigned		diff_before_sleep;

	ph = vars;
	start = &ph->args->start_time;
	while (TRUE)
	{
		gettimeofday(&curr, NULL);
		printf("%u -> %u is thinking\n", get_time_diff(start, &curr), ph->id);
		if (lock_forks(ph) == -1)
		{
			gettimeofday(&curr, NULL);
			printf("%u -> %u died\n", get_time_diff(start, &curr), ph->id);
			return (NULL);
		}
		gettimeofday(&curr, NULL);
		ph->last_eat_time = curr;
		printf("%u -> %u is eating\n", get_time_diff(start, &curr), ph->id);
		usleep(ph->args->time_to_eat * 1000);
		unlock_forks(ph);
		gettimeofday(&curr, NULL);
		diff_before_sleep = get_time_diff(&ph->last_eat_time, &curr) + ph->args->time_to_sleep;
		if (diff_before_sleep >= ph->args->time_to_die)
		{
			printf("%u -> %u is sleeping\n", get_time_diff(start, &curr), ph->id);
			usleep((diff_before_sleep - ph->args->time_to_sleep) * 1000);
			gettimeofday(&curr, NULL);
			printf("%u -> %u died\n", get_time_diff(start, &curr), ph->id);
			return (NULL);
		}
		printf("%u -> %u is sleeping\n", get_time_diff(start, &curr), ph->id);
		usleep(ph->args->time_to_sleep * 1000);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_args			args;
	t_philo			*ph;
	pthread_mutex_t	*forks;

	memset(&args, 0, sizeof(args));
	if (check_args_n(argc, argv[0]) == -1)
		return (1);
	if (argc == 6)
		args.limit_times_to_eat = TRUE;
	if (check_valid_args(argv + 1, &args) == -1)
		return (1);
	gettimeofday(&args.start_time, NULL);
	forks = malloc(sizeof(*forks) * args.n_philos);
	ph = malloc(sizeof(*ph) * args.n_philos);
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_mutex_init(&forks[i], NULL);
	for (unsigned i = 0; i < args.n_philos; i++)
	{
		ph[i].id = i + 1;
		ph[i].args = &args;
		ph[i].left_fork = &forks[i];
		ph[i].right_fork = &forks[(i + 1) % args.n_philos];
		ph[i].last_eat_time = args.start_time;
		pthread_create(&ph[i].thread, NULL, &run_philo, &ph[i]);
	}
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_join(ph[i].thread, NULL);
	for (unsigned i = 0; i < args.n_philos; i++)
		pthread_mutex_destroy(&forks[i]);
	free(ph);
	free(forks);
	return (0);
}
