/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:22 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/08 21:53:35 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H
# include <stddef.h>
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <sys/time.h>

enum		e_bool
{
	FALSE,
	TRUE
};

size_t		ft_strlen(const char *str);
void		ft_putstr_fd(const char *str, int fd);
unsigned	atou_error(const char *str, int *error);

#endif
