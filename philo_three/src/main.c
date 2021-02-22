/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/22 01:09:09 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static void	pickup_forks(t_philo *ph)
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
}

static void	drop_forks(t_philo *ph)
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

void		*dine_philo(void *vars)
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

static int	allocate_memory(pid_t **philos, t_args *args,
									t_super_monitor_dining_complete *super_mon,
											t_monitor_dining_complete **mon_dc)
{
	*philos = malloc(sizeof(pid_t) * args->n_philos);
	if (philos == NULL)
		return (-1);
	super_mon->is_dining_complete = malloc(sizeof(unsigned) * args->n_philos);
	if (super_mon->is_dining_complete == NULL)
		return (-1);
	super_mon->lock_dining_complete = malloc(sizeof(sem_t *) * args->n_philos);
	if (super_mon->lock_dining_complete == NULL)
		return (-1);
	*mon_dc = malloc(sizeof(t_monitor_dining_complete) * args->n_philos);
	if (mon_dc == NULL)
		return (-1);
	return (0);
}

int			main(int argc, char **argv)
{
	t_args							args;
	sem_t							*forks;
	pid_t							*philos;
	t_super_monitor_dining_complete	super_mon;
	t_monitor_dining_complete		*mon_dc;

	memset(&args, 0, sizeof(args));
	memset(&super_mon, 0, sizeof(super_mon));
	if (check_args(argc, argv, &args) == -1)
		return (1);
	if (allocate_memory(&philos, &args, &super_mon, &mon_dc) == -1)
		return (1);
	open_semaphores(&forks, &args, mon_dc);
	create_philo_proc(forks, &args, philos, mon_dc);
	super_mon.args = &args;
	super_mon.philos = philos;
	create_and_detach_monitor_threads(&args, &super_mon, mon_dc);
	wait_all_philos(philos, &args);
	return (0);
}
