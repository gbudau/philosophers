/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 17:04:46 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/23 19:48:42 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static void	open_philosopher_sem(t_philo *ph, t_monitor *mon,
															t_status_philo *sp)
{
	int		oflag;
	int		mode;
	char	*sem_name;

	oflag = O_CREAT | O_EXCL;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	mon->print_status = sp->print_status;
	ph->print_status = sp->print_status;
	sem_name = create_sem_name("/check_starvation", ph->id);
	mon->check_starvation = sem_open(sem_name, oflag, mode, 1);
	sem_unlink(sem_name);
	ph->check_starvation = mon->check_starvation;
	sem_name = create_sem_name("/check_dining_complete", ph->id);
	mon->check_dining_complete = sem_open(sem_name, oflag, mode, 1);
	sem_unlink(sem_name);
	ph->check_dining_complete = mon->check_dining_complete;
	sem_name = create_sem_name("/check_has_forks", ph->id);
	mon->check_has_forks = sem_open(sem_name, oflag, mode, 1);
	sem_unlink(sem_name);
	ph->check_has_forks = mon->check_has_forks;
}

static void	philo_died_exit(t_philo *ph, t_args *args)
{
	struct timeval	curr;

	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(&args->start_time, &curr), ph->id, "died");
	exit(1);
}

static void	*monitor_self(void *vars)
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

static void	run_philosopher(sem_t *forks, t_args *args, t_status_philo *sp,
											sem_t *dining_complete)
{
	t_philo		ph;
	t_monitor	mon;

	memset(&ph, 0, sizeof(ph));
	ph.id = sp->id;
	ph.args = args;
	ph.forks = forks;
	ph.last_eat_time = args->start_time;
	open_philosopher_sem(&ph, &mon, sp);
	ph.dining_complete = dining_complete;
	mon.ph = &ph;
	mon.args = args;
	pthread_create(&mon.thread, NULL, &monitor_self, &mon);
	pthread_detach(mon.thread);
	dine_philo(&ph);
}

void		create_philo_proc(sem_t *forks, t_args *args, pid_t *philos,
											t_monitor_dining_complete *mon_dc)
{
	unsigned		i;
	pid_t			pid;
	t_status_philo	sp;
	sem_t			*print_status;

	gettimeofday(&args->start_time, NULL);
	print_status = sem_open_unlink("/print_status", 1);
	i = 0;
	while (i < args->n_philos)
	{
		if (i == args->n_philos / 2)
			usleep(1000);
		if ((pid = fork()) == -1)
		{
			clean_all_philos(philos, i);
			exit(1);
		}
		if (pid == 0)
		{
			sp.id = i + 1;
			sp.print_status = print_status;
			run_philosopher(forks, args, &sp, mon_dc->dining_complete[i]);
		}
		philos[i++] = pid;
	}
}
