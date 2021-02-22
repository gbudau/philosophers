/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dine_philo_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 21:43:16 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/23 00:12:36 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_two.h"

void	increment_eat_count(t_philo *ph)
{
	ph->eat_count++;
	if (ph->eat_count == ph->args->n_times_to_eat)
	{
		sem_wait(ph->check_dining_complete);
		ph->dining_complete = TRUE;
		sem_post(ph->check_dining_complete);
	}
}

void	eat_spaghetti(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	gettimeofday(&curr, NULL);
	sem_wait(ph->check_starvation);
	ph->last_eat_time = curr;
	if (ph->args->limit_times_to_eat)
		increment_eat_count(ph);
	sem_post(ph->check_starvation);
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "is eating");
	sem_post(ph->print_status);
	usleep(ph->args->time_to_eat * 1000);
}

void	philo_sleep(t_philo *ph)
{
	struct timeval	curr;
	struct timeval	*start;

	start = &ph->args->start_time;
	sem_wait(ph->print_status);
	gettimeofday(&curr, NULL);
	ft_print_status(get_time_diff(start, &curr), ph->id, "is sleeping");
	sem_post(ph->print_status);
	usleep(ph->args->time_to_sleep * 1000);
}
