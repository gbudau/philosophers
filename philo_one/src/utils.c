/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 17:04:46 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/15 20:07:18 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

unsigned	get_time_diff(struct timeval *start, struct timeval *curr)
{
	unsigned	ms;

	ms = (curr->tv_sec - start->tv_sec) * 1000;
	ms += (curr->tv_usec - start->tv_usec) / 1000;
	return (ms);
}

int			allocate_memory(t_philo **ph, pthread_mutex_t **forks,
												t_monitor *mon, t_args *args)
{
	*ph = malloc(sizeof(t_philo) * args->n_philos);
	if (*ph == NULL)
		return (-1);
	memset(*ph, 0, sizeof(t_philo) * args->n_philos);
	*forks = malloc(sizeof(pthread_mutex_t) * args->n_philos);
	if (*forks == NULL)
		return (-1);
	mon->check_starvation = malloc(sizeof(pthread_mutex_t) * args->n_philos);
	if (mon->check_starvation == NULL)
		return (-1);
	if (args->limit_times_to_eat)
	{
		mon->check_dining_complete = malloc(sizeof(pthread_mutex_t) *
																args->n_philos);
		if (mon->check_dining_complete == NULL)
			return (-1);
	}
	return (0);
}

void		initialize_mutexes(pthread_mutex_t *forks, t_monitor *mon,
																t_args *args)
{
	unsigned	i;
	
	i = 0;
	while (i < args->n_philos)
		pthread_mutex_init(&forks[i++], NULL);
	i = 0;
	while (i < args->n_philos)
		pthread_mutex_init(&mon->check_starvation[i++], NULL);
	if (args->limit_times_to_eat)
	{
		i = 0;
		while (i < args->n_philos)
			pthread_mutex_init(&mon->check_dining_complete[i++], NULL);
	}
	pthread_mutex_init(&mon->print_status, NULL);
}

void		create_and_detach_philo_threads(t_philo *ph, pthread_mutex_t *forks,
												t_monitor *mon, t_args *args)
{
	unsigned		i;

	gettimeofday(&args->start_time, NULL);
	i = 0;
	while (i < args->n_philos)
	{
		ph[i].id = i + 1;
		ph[i].args = args;
		ph[i].first_fork = &forks[(i + i % 2) % args->n_philos];
		ph[i].second_fork = &forks[(i + !(i % 2)) % args->n_philos];
		ph[i].last_eat_time = args->start_time;
		ph[i].check_starvation = &mon->check_starvation[i];
		if (args->limit_times_to_eat)
			ph[i].check_dining_complete = &mon->check_dining_complete[i];
		ph[i].print_status = &mon->print_status;
		pthread_create(&ph[i].thread, NULL, &dine_philo, &ph[i]);
		i++;
	}
	i = 0;
	while (i < args->n_philos)
		pthread_detach(ph[i++].thread);
}

void		create_and_join_monitor_thread(t_monitor *mon, t_philo *ph,
																t_args *args)
{
	struct timeval	curr;
	unsigned		*id;

	mon->args = args;
	mon->ph = ph;
	pthread_create(&mon->thread, NULL, &monitor_philos, mon);
	pthread_join(mon->thread, (void **)&id);
	pthread_mutex_lock(&mon->print_status);
	if (id)
	{
		gettimeofday(&curr, NULL);
		ft_print_status(get_time_diff(&args->start_time, &curr), *id, "died");
	}
}
