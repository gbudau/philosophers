/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_dining_complete.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 01:08:09 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/22 01:13:25 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static void	*set_dining_complete(void *vars)
{
	t_monitor_dining_complete	*mon;

	mon = vars;
	sem_wait(mon->dining_complete);
	sem_wait(mon->lock_dining_complete);
	*mon->is_dining_complete = TRUE;
	sem_post(mon->lock_dining_complete);
	return (NULL);
}

static void	*exit_dining_complete(void *vars)
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
		super_mon->lock_dining_complete[i++] = sem_open(sem_name,
															O_CREAT | O_EXCL,
									S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
		sem_unlink(sem_name);
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
