/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/20 16:33:24 by gbudau           ###   ########.fr       */
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

void		*monitor_self(void *vars)
{
	t_args			*args;
	t_philo			*ph;
	t_monitor		*mon;
	struct timeval	curr;

	mon = vars;
	ph = mon->ph;
	args = mon->args;
	while (TRUE)
	{
		if (is_starving(ph, args))
		{
			sem_wait(ph->print_status);
			gettimeofday(&curr, NULL);
			ft_print_status(get_time_diff(&args->start_time, &curr), ph->id, "died");
			exit(1);
		}
		usleep(5000);
	}
	return (NULL);
}

int			main(int argc, char **argv)
{
	t_args		args;
	t_monitor	mon;
	sem_t		*forks;
	pid_t		*philos;

	memset(&args, 0, sizeof(args));
	memset(&mon, 0, sizeof(mon));
	if (check_args(argc, argv, &args) == -1)
		return (1);
	philos = malloc(sizeof(pid_t) * args.n_philos);
	if (philos == NULL)
		return (1);
	open_semaphores(&forks, &mon, &args);
	unlink_semaphores();
	create_philo_proc(forks, &mon, &args, philos);
	return (0);
}
