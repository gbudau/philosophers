/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/16 19:01:25 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_two.h"

int		pickup_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	gettimeofday(&curr, NULL);
	sem_wait(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "is thinking");
	sem_post(ph->print_status);
	usleep(1000);
	sem_wait(ph->forks);
	gettimeofday(&curr, NULL);
	sem_wait(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	sem_post(ph->print_status);
	sem_wait(ph->forks);
	gettimeofday(&curr, NULL);
	sem_wait(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	sem_post(ph->print_status);
	return (0);
}

void	drop_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	sem_post(ph->forks);
	gettimeofday(&curr, NULL);
	sem_wait(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	sem_post(ph->print_status);
	sem_post(ph->forks);
	sem_wait(ph->print_status);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	sem_post(ph->print_status);
}

void	*dine_philo(void *vars)
{
	t_philo			*ph;

	ph = vars;
	while (TRUE)
	{
		pickup_forks(ph);
		eat_spaghetti(ph);
		drop_forks(ph);
		philo_sleep(ph);
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

int		main(int argc, char **argv)
{
	t_args		args;
	t_philo		*ph;
	t_monitor	mon;
	sem_t		*forks;

	memset(&args, 0, sizeof(args));
	memset(&mon, 0, sizeof(mon));
	if (check_args(argc, argv, &args) == -1)
		return (1);
	if (allocate_memory(&ph, &mon, &args) == -1)
		return (1);
	open_semaphores(&forks, &mon, &args);
	unlink_semaphores(&args);
	create_and_detach_philo_threads(ph, forks, &mon, &args);
	create_and_join_monitor_thread(&mon, ph, &args);
	return (0);
}
