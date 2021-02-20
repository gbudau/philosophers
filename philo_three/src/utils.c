/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 17:04:46 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/20 16:33:01 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

void		open_semaphores(sem_t **forks, t_monitor *mon, t_args *args)
{
	int			oflag;
	mode_t		mode;

	oflag = O_CREAT | O_EXCL;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	*forks = sem_open("/forks", oflag, mode, args->n_philos);
	mon->print_status = sem_open("/print_status", oflag, mode, 1);
}

void		create_philo_proc(sem_t *forks, t_monitor *mon, t_args *args, pid_t *philos)
{
	unsigned	i;
	unsigned	j;
	pid_t		pid;
	t_philo		ph;
	char		*sem_name;

	gettimeofday(&args->start_time, NULL);
	i = 0;
	while (i < args->n_philos)
	{
		pid = fork();
		if (pid == -1)
		{
			j = 0;
			while (j < i)
				kill(philos[j++], SIGKILL);
			exit (1);
		}
		if (pid == 0)
		{
			ph.id = i + 1;
			ph.args = args;
			ph.forks = forks;
			ph.print_status = mon->print_status;
			ph.last_eat_time = args->start_time;
			// Check if current philosopher is starving
			sem_name = create_sem_name("/check_starvation", ph.id);
			mon->check_starvation = sem_open(sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
			sem_unlink(sem_name);
			ph.check_starvation = mon->check_starvation;
			mon->ph = &ph;
			mon->args = args;
			pthread_create(&mon->thread, NULL, monitor_self, mon);
			pthread_detach(mon->thread);
			dine_philo(&ph);
		}
		philos[i] = pid;
		i++;
	}
	if (waitpid(-1, NULL, 0) > 0)
	{
		i = 0;
		while (i < args->n_philos)
			kill(philos[i++], SIGKILL);
	}
	exit (0);
}

void		unlink_semaphores(void)
{
	sem_unlink("/forks");
	sem_unlink("/print_status");
}
