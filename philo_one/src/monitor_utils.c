/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 21:02:09 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/23 00:47:32 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

unsigned	*check_starvation(t_philo *ph, t_args *args)
{
	unsigned		time_diff;
	unsigned		i;
	struct timeval	curr;

	i = 0;
	while (i < args->n_philos)
	{
		gettimeofday(&curr, NULL);
		pthread_mutex_lock(ph[i].check_starvation);
		time_diff = get_time_diff(&ph[i].last_eat_time, &curr);
		if (time_diff >= ph[i].args->time_to_die)
			return (&ph[i].id);
		pthread_mutex_unlock(ph[i].check_starvation);
		i++;
	}
	return (NULL);
}

int			is_dining_complete(t_philo *ph, t_args *args)
{
	unsigned	i;

	i = 0;
	while (i < args->n_philos)
	{
		pthread_mutex_lock(ph[i].check_dining_complete);
		if (ph[i].dining_complete == FALSE)
		{
			pthread_mutex_unlock(ph[i].check_dining_complete);
			return (FALSE);
		}
		pthread_mutex_unlock(ph[i].check_dining_complete);
		i++;
	}
	return (TRUE);
}

void		*monitor_philos(void *vars)
{
	t_args			*args;
	t_philo			*ph;
	t_monitor		*mon;
	unsigned		*id;

	mon = vars;
	ph = mon->ph;
	args = mon->args;
	while (TRUE)
	{
		id = check_starvation(ph, args);
		if (id)
			return ((void *)id);
		if (args->limit_times_to_eat)
			if (is_dining_complete(ph, args))
				return (NULL);
		usleep(5000);
	}
	return (NULL);
}
