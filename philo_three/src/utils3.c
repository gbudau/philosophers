/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 23:38:55 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/24 14:23:40 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

int		allocate_memory(pid_t **philos, t_args *args,
											t_monitor_dining_complete *mon_dc)
{
	*philos = malloc(sizeof(pid_t) * args->n_philos);
	if (philos == NULL)
		return (-1);
	if (args->limit_times_to_eat)
	{
		mon_dc->dining_complete = malloc(sizeof(sem_t *) * args->n_philos);
		if (mon_dc->dining_complete == NULL)
			return (-1);
	}
	return (0);
}

sem_t	*sem_open_unlink(const char *name, int value)
{
	sem_t	*new_sem;

	new_sem = sem_open(name, SEM_OPEN_FLAG, SEM_MODE_FLAG, value);
	sem_unlink(name);
	return (new_sem);
}
