/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/24 18:28:46 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static void	pickup_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	sem_wait(ph->forks);
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	sem_post(ph->print_status);
	sem_wait(ph->forks);
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has taken a fork");
	sem_post(ph->print_status);
}

static void	drop_forks(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	sem_post(ph->forks);
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	sem_post(ph->print_status);
	sem_post(ph->forks);
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "has dropped a fork");
	sem_post(ph->print_status);
}

static void	philo_think(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "is thinking");
	sem_post(ph->print_status);
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
		ft_usleep(1);
	}
	return (NULL);
}

int			main(int argc, char **argv)
{
	t_args							args;
	sem_t							*forks;
	pid_t							*philos;
	t_monitor_dining_complete		mon_dc;

	memset(&args, 0, sizeof(args));
	memset(&mon_dc, 0, sizeof(mon_dc));
	if (check_args(argc, argv, &args) == -1)
		return (1);
	if (allocate_memory(&philos, &args, &mon_dc) == -1)
		return (1);
	open_semaphores(&forks, &args, &mon_dc);
	create_philo_proc(forks, &args, philos, &mon_dc);
	if (args.limit_times_to_eat)
	{
		mon_dc.args = &args;
		mon_dc.philos = philos;
		create_and_detach_monitor_thread(&mon_dc);
	}
	wait_all_philos(philos, &args);
	return (0);
}
