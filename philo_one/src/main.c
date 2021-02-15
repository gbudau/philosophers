/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/15 16:04:38 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"
#include <stdio.h>

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
	if (check_args(argc, argv, &args) == -1)
		return (1);
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
