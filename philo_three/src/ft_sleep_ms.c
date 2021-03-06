/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sleep_ms.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/24 17:56:18 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/24 18:27:30 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static unsigned	current_time(void)
{
	struct timeval	curr;
	unsigned		ms;

	gettimeofday(&curr, NULL);
	ms = curr.tv_sec * 1000;
	ms += curr.tv_usec / 1000;
	return (ms);
}

void			ft_sleep_ms(unsigned ms)
{
	unsigned	start;

	start = current_time();
	while (current_time() - start <= ms)
		usleep(100);
}
