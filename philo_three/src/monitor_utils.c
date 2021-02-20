/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 21:02:09 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/20 15:46:20 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

int			is_starving(t_philo *ph, t_args *args)
{
	unsigned		time_diff;
	struct timeval	curr;

	(void)args;
	gettimeofday(&curr, NULL);
	sem_wait(ph->check_starvation);
	time_diff = get_time_diff(&ph->last_eat_time, &curr);
	if (time_diff >= ph->args->time_to_die)
		return (TRUE);
	sem_post(ph->check_starvation);
	return (FALSE);
}

/*
int			is_dining_complete(t_philo *ph, t_args *args)
{
	unsigned	i;

	i = 0;
	while (i < args->n_philos)
	{
		sem_wait(ph[i].check_dining_complete);
		if (ph[i].dining_complete == FALSE)
		{
			sem_post(ph[i].check_dining_complete);
			return (FALSE);
		}
		sem_post(ph[i].check_dining_complete);
		i++;
	}
	return (TRUE);
}
*/
