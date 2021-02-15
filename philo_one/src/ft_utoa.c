/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 15:18:14 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/15 15:37:56 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static void	ft_swap(char *a, char *b)
{
	char	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	ft_reverse_str(char *str, unsigned start, unsigned end)
{
	while (start < end)
		ft_swap(str + start++, str + end--);
}

unsigned	ft_utoa(unsigned n, char *buffer)
{
	unsigned	i;
	
	i = 0;

	while (n)
	{
		buffer[i++] = n % 10 + '0';
		n /= 10;
	}
	if (!i)
		buffer[i++] = '0';
	buffer[i] = '\0';
	ft_reverse_str(buffer, 0, i - 1);
	return (i);
}
