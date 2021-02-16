/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 17:04:46 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/16 18:35:57 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_two.h"

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

int			allocate_memory(t_philo **ph, t_monitor *mon, t_args *args)
{
	*ph = malloc(sizeof(t_philo) * args->n_philos);
	if (*ph == NULL)
		return (-1);
	memset(*ph, 0, sizeof(t_philo) * args->n_philos);
	mon->check_starvation = malloc(sizeof(sem_t *) * args->n_philos);
	if (mon->check_starvation == NULL)
		return (-1);
	if (!args->limit_times_to_eat)
		return (0);
	mon->check_dining_complete = malloc(sizeof(sem_t *) * args->n_philos);
	if (mon->check_dining_complete == NULL)
		return (-1);
	return (0);
}

void		open_semaphores(sem_t **forks, t_monitor *mon, t_args *args)
{
	int			oflag;
	mode_t		mode;
	char		*sem_name;
	unsigned	i;

	oflag = O_CREAT | O_EXCL;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	*forks = sem_open("/forks", oflag, mode, args->n_philos);
	mon->print_status = sem_open("/print_status", oflag, mode, 1);
	i = 0;
	while (i < args->n_philos)
	{
		sem_name = create_sem_name("/check_starvation", i + 1);
		mon->check_starvation[i++] = sem_open(sem_name, oflag, mode, 1);
	}
	if (!args->limit_times_to_eat)
		return ;
	i = 0;
	while (i < args->n_philos)
	{
		sem_name = create_sem_name("/check_dining_complete", i + 1);
		mon->check_dining_complete[i++] = sem_open(sem_name, oflag, mode, 1);
	}
}

void		create_and_detach_philo_threads(t_philo *ph, sem_t *forks,
												t_monitor *mon, t_args *args)
{
	unsigned	i;

	gettimeofday(&args->start_time, NULL);
	i = 0;
	while (i < args->n_philos)
	{
		ph[i].id = i + 1;
		ph[i].args = args;
		ph[i].forks = forks;
		ph[i].last_eat_time = args->start_time;
		ph[i].check_starvation = mon->check_starvation[i];
		if (args->limit_times_to_eat)
			ph[i].check_dining_complete = mon->check_dining_complete[i];
		ph[i].print_status = mon->print_status;
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
	sem_wait(mon->print_status);
	if (id)
	{
		gettimeofday(&curr, NULL);
		ft_print_status(get_time_diff(&args->start_time, &curr), *id, "died");
	}
}

void		unlink_semaphores(t_args *args)
{
	unsigned	i;
	char		*sem_name;
	sem_unlink("/forks");
	sem_unlink("/print_status");

	i = 0;
	while (i < args->n_philos)
	{
		sem_name = create_sem_name("/check_starvation", i + 1);
		sem_unlink(sem_name);
		i++;
	}
	if (!args->limit_times_to_eat)
		return ;
	i = 0;
	while (i < args->n_philos)
	{
		sem_name = create_sem_name("/check_dining_complete", i + 1);
		sem_unlink(sem_name);
		i++;
	}
}
