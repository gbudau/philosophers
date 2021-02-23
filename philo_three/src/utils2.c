/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/16 19:10:36 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/23 19:46:31 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

unsigned	get_time_diff(struct timeval *start, struct timeval *curr)
{
	unsigned	ms;

	ms = (curr->tv_sec - start->tv_sec) * 1000;
	ms += (curr->tv_usec - start->tv_usec) / 1000;
	return (ms);
}

char		*create_sem_name(const char *str, unsigned id)
{
	static char	sem_name[64];
	char		philo_id[12];
	unsigned	i;

	i = ft_strcpy(sem_name, str);
	ft_utoa(id, philo_id);
	ft_strcpy(sem_name + i, philo_id);
	return (sem_name);
}

void		clean_all_philos(pid_t *philos, unsigned count)
{
	unsigned	i;

	i = 0;
	while (i < count)
		kill(philos[i++], SIGKILL);
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
		mon_dc->dining_complete[i] = sem_open(sem_name, oflag, mode, 0);
		sem_unlink(sem_name);
		i++;
	}
}
