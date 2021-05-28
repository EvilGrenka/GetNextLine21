/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rnoriko <rnoriko@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 13:33:36 by rnoriko           #+#    #+#             */
/*   Updated: 2021/05/28 11:35:50 by rnoriko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static t_gnl_bonus	*ft_create_new_node(int fd)
{
	t_gnl_bonus	*new_node;

	new_node = (t_gnl_bonus *) malloc(sizeof(t_gnl_bonus));
	if (!new_node)
		return (NULL);
	new_node -> fd = fd;
	new_node -> save_buffer = ft_strdup("");
	new_node -> next = NULL;
	return (new_node);
}

static int	ft_del_node(t_gnl_bonus **head, t_gnl_bonus *current_node,
	int readed_bytes)
{
	t_gnl_bonus	*tmp;

	if (!head || !(*head))
		return (readed_bytes);
	tmp = *head;
	if (tmp == current_node && !tmp->next)
		*head = NULL;
	else if (tmp == current_node)
		*head = (*head)->next;
	else
	{
		while (tmp->next != current_node)
			tmp = tmp->next;
		tmp->next = current_node->next;
		tmp = current_node;
	}	
	if (tmp->save_buffer)
		free(tmp->save_buffer);
	free(tmp);
	return (readed_bytes);
}

static int	ft_save_buffer_node(int fd, char *buffer, t_gnl_bonus **head,
	t_gnl_bonus*current_node)
{
	char		*control_leaks;
	int			readed_bytes;

	readed_bytes = read(fd, buffer, BUFFER_SIZE);
	while (readed_bytes)
	{
		if (readed_bytes == -1)
			return (ft_del_node(head, current_node, readed_bytes));
		buffer[readed_bytes] = '\0';
		if (current_node -> save_buffer)
		{
			control_leaks = current_node -> save_buffer;
			current_node -> save_buffer
				= ft_strjoin(current_node -> save_buffer, buffer);
			free(control_leaks);
		}
		else
			current_node -> save_buffer = ft_strdup(buffer);
		if (ft_strchr(buffer, '\n'))
			break ;
		readed_bytes = read(fd, buffer, BUFFER_SIZE);
	}
	return (readed_bytes);
}

static char	*ft_get_line_and_reminder(char **line, char *save_buffer,
	int readed_bytes)
{
	char	*control_leaks;
	size_t	i;

	i = 0;
	while (save_buffer[i] != '\n' && save_buffer[i])
		i++;
	if (i < ft_strlen(save_buffer))
	{
		*line = ft_substr(save_buffer, 0, i);
		control_leaks = save_buffer;
		save_buffer = ft_substr(save_buffer, i + 1, ft_strlen(save_buffer));
		free(control_leaks);
	}
	else if (!readed_bytes)
	{
		*line = save_buffer;
		save_buffer = NULL;
	}
	return (save_buffer);
}

int	get_next_line(int fd, char **line)
{
	static t_gnl_bonus	*head;
	t_gnl_bonus			*current_node;
	char				buffer[BUFFER_SIZE + 1];
	int					readed_bytes;

	if (!line || BUFFER_SIZE <= 0)
		return (-1);
	if (!head)
		head = ft_create_new_node(fd);
	current_node = head;
	while (current_node && current_node -> fd != fd )
		current_node = current_node -> next;
	if (!current_node)
		current_node = ft_create_new_node(fd);
	readed_bytes = ft_save_buffer_node(fd, buffer, &head, current_node);
	if (readed_bytes == -1)
		return (readed_bytes);
	if (!readed_bytes && !current_node->save_buffer)
		*line = ft_strdup("");
	else
		current_node -> save_buffer = ft_get_line_and_reminder
			(line, current_node -> save_buffer, readed_bytes);
	if (!readed_bytes && !current_node->save_buffer)
		return (ft_del_node(&head, current_node, readed_bytes));
	return (1);
}