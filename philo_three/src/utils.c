/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 17:04:46 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/22 00:23:00 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

void		open_semaphores(sem_t **forks, t_args *args,
											t_monitor_dining_complete *mon_dc)
{
	int			oflag;
	mode_t		mode;
	unsigned	i;
	char		*sem_name;

	oflag = O_CREAT | O_EXCL;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	*forks = sem_open("/forks", oflag, mode, args->n_philos);
	sem_unlink("/forks");
	i = 0;
	while (i < args->n_philos)
	{
		sem_name = create_sem_name("/dining_complete", i + 1);
		mon_dc[i].dining_complete = sem_open(sem_name, oflag, mode, 0);
		sem_unlink(sem_name);
		i++;
	}
}

void		clean_all_philos(pid_t *philos, unsigned count)
{
	unsigned	i;

	i = 0;
	while (i < count)
		kill(philos[i++], SIGKILL);
}

void		open_philosopher_sem(t_philo *ph, t_monitor *mon)
{
	int		oflag;
	int		mode;
	char	*sem_name;

	oflag = O_CREAT | O_EXCL;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	mon->print_status = sem_open("/print_status", oflag, mode, 1);
	sem_unlink("/print_status");
	ph->print_status = mon->print_status;
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

void		run_philosopher(sem_t *forks, t_args *args, unsigned id,
											t_monitor_dining_complete *mon_dc)
{
	t_philo		ph;
	t_monitor	mon;

	memset(&ph, 0, sizeof(ph));
	ph.id = id;
	ph.args = args;
	ph.forks = forks;
	ph.last_eat_time = args->start_time;
	open_philosopher_sem(&ph, &mon);
	ph.dining_complete = mon_dc->dining_complete;
	mon.ph = &ph;
	mon.args = args;
	pthread_create(&mon.thread, NULL, &monitor_self, &mon);
	pthread_detach(mon.thread);
	dine_philo(&ph);
}

void		create_philo_proc(sem_t *forks, t_args *args, pid_t *philos,
											t_monitor_dining_complete *mon_dc)
{
	unsigned	i;
	pid_t		pid;

	gettimeofday(&args->start_time, NULL);
	i = 0;
	while (i < args->n_philos)
	{
		pid = fork();
		if (pid == -1)
		{
			clean_all_philos(philos, i);
			exit(1);
		}
		if (pid == 0)
			run_philosopher(forks, args, i + 1, &mon_dc[i]);
		philos[i] = pid;
		i++;
	}
}

void		wait_all_philos(pid_t *philos, t_args *args)
{
	pid_t		pid;
	unsigned	i;
	int			wstatus;

	if ((pid = waitpid(-1, &wstatus, 0)) > 0)
	{
		if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus))
		{
			i = 0;
			while (i < args->n_philos)
			{
				if (pid != philos[i])
					kill(philos[i], SIGKILL);
				i++;
			}
			exit(1);
		}
		else
		{
			while (waitpid(-1, NULL, 0) > 0)
				;
		}
	}
}
