/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/21 23:51:06 by gbudau           ###   ########.fr       */
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

void		philo_died_exit(t_philo *ph, t_args *args)
{
	struct timeval	curr;

	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(&args->start_time, &curr), ph->id, "died");
	exit(1);
}

void		*monitor_self(void *vars)
{
	t_args			*args;
	t_philo			*ph;
	t_monitor		*mon;
	int				dining_complete;

	mon = vars;
	ph = mon->ph;
	args = mon->args;
	dining_complete = FALSE;
	while (TRUE)
	{
		if (is_starving(ph, args))
			philo_died_exit(ph, args);
		if (!dining_complete && args->limit_times_to_eat &&
														is_dining_complete(ph))
		{
			dining_complete = TRUE;
			sem_post(ph->dining_complete);
		}
		usleep(5000);
	}
	return (NULL);
}


void		*set_dining_complete(void *vars)
{
	t_monitor_dining_complete	*mon;

	mon = vars;
	sem_wait(mon->dining_complete);
	sem_wait(mon->lock_dining_complete);
	*mon->is_dining_complete = TRUE;
	sem_post(mon->lock_dining_complete);
	return (NULL);
}

void		*exit_dining_complete(void *vars)
{
	t_super_monitor_dining_complete	*super_mon;
	unsigned						i;

	super_mon = vars;
	while (TRUE)
	{
		i = 0;
		while (i < super_mon->args->n_philos)
		{
			sem_wait(super_mon->lock_dining_complete[i]);
			if (super_mon->is_dining_complete[i] == FALSE)
			{
				sem_post(super_mon->lock_dining_complete[i]);
				break ;
			}
			sem_post(super_mon->lock_dining_complete[i]);
			i++;
		}
		if (i == super_mon->args->n_philos)
		{
			clean_all_philos(super_mon->philos, i);
			exit(0);
		}
		usleep(150000);
	}
}

void		create_and_detach_monitor_threads(t_args *args,
									t_super_monitor_dining_complete *super_mon,
											t_monitor_dining_complete *mon_dc)
{
	unsigned						i;
	char							*sem_name;
	pthread_t						thread_id;

	i = 0;
	while (i < args->n_philos)
	{
		sem_name = create_sem_name("/lock_dining_complete", i);
		super_mon->lock_dining_complete[i] = sem_open(sem_name, O_CREAT | O_EXCL,
									S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
		sem_unlink(sem_name);
		i++;
	}
	i = 0;
	while (i < args->n_philos)
	{
		mon_dc[i].args = args;
		mon_dc[i].is_dining_complete = &super_mon->is_dining_complete[i];
		mon_dc[i].lock_dining_complete = super_mon->lock_dining_complete[i];
		pthread_create(&thread_id, NULL, &set_dining_complete, &mon_dc[i]);
		pthread_detach(thread_id);
		i++;
	}
	pthread_create(&thread_id, NULL, &exit_dining_complete, super_mon);
	pthread_detach(thread_id);
}

int			allocate_memory(pid_t **philos, t_args *args,
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
