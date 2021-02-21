/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/16 19:10:36 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/22 00:24:50 by gbudau           ###   ########.fr       */
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
