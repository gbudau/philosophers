/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 17:04:46 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/21 23:17:38 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

// TODO Rename to create_forks or something like that
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
		printf("Parent monitor %s\n", sem_name);
		errno = 0;
		mon_dc[i].dining_complete = sem_open(sem_name, oflag, mode, 0);
		if (errno)
			perror("ERROR parent monitor semaphore");

		// TODO Remove this
		int test_value;
		sem_getvalue(mon_dc[i].dining_complete, &test_value);
		printf("BBB Sem value is %d\n", test_value);

		sem_unlink(sem_name);
		i++;
	}
}

void		clean_all_philos(pid_t	*philos, unsigned count)
{
	unsigned	i;

	i = 0;
	while (i < count)
		kill(philos[i++], SIGKILL);
}

void		create_philo_proc(sem_t *forks, t_args *args, pid_t *philos,
											t_monitor_dining_complete *mon_dc)
{
	unsigned	i;
	pid_t		pid;
	
	gettimeofday(&args->start_time, NULL);
	i = 0;
	while (i < args->n_philos)
	{
		pid = fork();
		if (pid == -1)
		{
			clean_all_philos(philos, i);
			exit(1);
		}
		if (pid == 0)
		{
			// TODO This variables are used only in the child process
			t_philo		ph;
			char		*sem_name;
			t_monitor	mon;

			memset(&ph, 0, sizeof(ph));
			ph.id = i + 1;
			ph.args = args;
			ph.forks = forks;
			ph.last_eat_time = args->start_time;
			// Semaphore used to protect writing on stdout after a philosopher dies
			mon.print_status = sem_open("/print_status", O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
			sem_unlink("/print_status");
			ph.print_status = mon.print_status;
			// Semaphore used to check if current philosopher is starving
			sem_name = create_sem_name("/check_starvation", ph.id);
			mon.check_starvation = sem_open(sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
			sem_unlink(sem_name);
			ph.check_starvation = mon.check_starvation;
			// Semaphore used to check if current philosopher has eat atleast number_of_times_each_philo_must_eat
			sem_name = create_sem_name("/check_dining_complete", ph.id);
			mon.check_dining_complete = sem_open(sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
			sem_unlink(sem_name);
			ph.check_dining_complete = mon.check_dining_complete;
			// Semaphore used to check how many forks the current philosopher picked up
			sem_name = create_sem_name("/check_has_forks", ph.id);
			mon.check_has_forks = sem_open(sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 1);
			sem_unlink(sem_name);
			ph.check_has_forks = mon.check_has_forks;
			// Semaphore used to signal when current philosopher has eat atleast number_of_times_each_philo_must_eat to the parent process
			ph.dining_complete = mon_dc[i].dining_complete;
#if 0

			sem_name = create_sem_name("/dining_complete", ph.id);
			sem_unlink(sem_name);
			// TODO remove errno
			errno = 0;

			printf("CHILD %s\n", sem_name);
			if ((ph.dining_complete = sem_open(sem_name, 0)) == SEM_FAILED)
				if (errno)
				{
					perror("Child semaphore");
					exit(1);
				}

			// TODO Remove this
			if (ph.dining_complete != SEM_FAILED)
			{
				int test_value;
				sem_getvalue(ph.dining_complete, &test_value);
				printf("AAA Sem value is %d\n", test_value);
			}

			sem_unlink(sem_name);
#endif

			mon.ph = &ph;
			mon.args = args;
			pthread_create(&mon.thread, NULL, &monitor_self, &mon);
			pthread_detach(mon.thread);
			dine_philo(&ph);
		}
		philos[i] = pid;
		i++;
	}
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
