/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/24 18:13:25 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

static void	pickup_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	pthread_mutex_lock(ph->first_fork);
	pthread_mutex_lock(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_lock(ph->second_fork);
	pthread_mutex_lock(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	pthread_mutex_unlock(ph->print_status);
}

static void	drop_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	pthread_mutex_unlock(ph->second_fork);
	pthread_mutex_lock(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	pthread_mutex_unlock(ph->print_status);
	pthread_mutex_unlock(ph->first_fork);
	pthread_mutex_lock(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	pthread_mutex_unlock(ph->print_status);
}

static void	philo_think(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	pthread_mutex_lock(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "is thinking");
	pthread_mutex_unlock(ph->print_status);
}

void		*dine_philo(void *vars)
{
	t_philo			*ph;

	ph = vars;
	philo_think(ph);
	while (TRUE)
	{
		pickup_forks(ph);
		eat_spaghetti(ph);
		drop_forks(ph);
		philo_sleep(ph);
		philo_think(ph);
		ft_sleep_ms(1);
	}
	return (NULL);
}

int			main(int argc, char **argv)
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
