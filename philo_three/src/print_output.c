/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_output.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 18:38:36 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/16 17:34:55 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

size_t			ft_strlen(const char *str)
{
	const char *start;

	start = str;
	while (*str)
		str++;
	return (str - start);
}

void			ft_putstr_fd(const char *str, int fd)
{
	(void)!write(fd, str, ft_strlen(str));
}

unsigned		ft_strcpy(char *dst, const char *src)
{
	unsigned	i;

	i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (i);
}

void			ft_print_status(unsigned ms, unsigned id, const char *status)
{
	char		buffer[64];
	unsigned	i;

	i = ft_utoa(ms, buffer);
	buffer[i++] = ' ';
	i += ft_utoa(id, buffer + i);
	buffer[i++] = ' ';
	i += ft_strcpy(buffer + i, status);
	i += ft_strcpy(buffer + i, "\n");
	(void)!write(STDOUT_FILENO, buffer, i);
}
