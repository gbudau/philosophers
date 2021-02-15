/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/15 21:22:37 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

int		pickup_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	gettimeofday(&curr, NULL);
	pthread_mutex_lock(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "is thinking");
	pthread_mutex_unlock(ph->print_status);
	usleep(1000);
	pthread_mutex_lock(ph->first_fork);
	gettimeofday(&curr, NULL);
	pthread_mutex_lock(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_lock(ph->second_fork);
	gettimeofday(&curr, NULL);
	pthread_mutex_lock(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
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
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_unlock(ph->first_fork);
	pthread_mutex_lock(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
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
		ft_print_status(get_time_diff(start, &curr), ph->id, "is eating");
		pthread_mutex_unlock(ph->print_status);
		usleep(ph->args->time_to_eat * 1000);
		drop_forks(ph);
		gettimeofday(&curr, NULL);
		pthread_mutex_lock(ph->print_status);
		ft_print_status(get_time_diff(start, &curr), ph->id, "is sleeping");
		pthread_mutex_unlock(ph->print_status);
		usleep(ph->args->time_to_sleep * 1000);
	}
	return (NULL);
}

void	*monitor_philos(void *vars)
{
	t_args			*args;
	t_philo			*ph;
	t_monitor		*mon;
	unsigned		*id;

	mon = vars;
	ph = mon->ph;
	args = mon->args;
	while (TRUE)
	{
		id = check_starvation(ph, args);
		if (id)
			return ((void *)id);
		if (args->limit_times_to_eat)
			if (is_dining_complete(ph, args))
				return (NULL);
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

	memset(&args, 0, sizeof(args));
	memset(&mon, 0, sizeof(mon));
	if (check_args(argc, argv, &args) == -1)
		return (1);
	if (allocate_memory(&ph, &forks, &mon, &args) == -1)
		return (1);
	initialize_mutexes(forks, &mon, &args);
	create_and_detach_philo_threads(ph, forks, &mon, &args);
	create_and_join_monitor_thread(&mon, ph, &args);
	return (0);
}
