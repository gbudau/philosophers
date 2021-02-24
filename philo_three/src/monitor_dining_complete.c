/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_dining_complete.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 01:08:09 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/24 15:58:32 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static void	*set_dining_complete(void *vars)
{
	t_monitor_dining_complete	*mon;
	unsigned					i;
	t_args						*args;

	mon = vars;
	args = mon->args;
	i = 0;
	while (i < args->n_philos)
	{
		sem_wait(mon->dining_complete[i]);
		i++;
	}
	exit(clean_all_philos(mon->philos, args->n_philos, 0));
	return (NULL);
}

void		create_and_detach_monitor_thread(t_monitor_dining_complete *mon_dc)
{
	pthread_t						thread_id;

	pthread_create(&thread_id, NULL, &set_dining_complete, mon_dc);
	pthread_detach(thread_id);
}
