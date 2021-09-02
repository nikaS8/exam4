#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int ft_strlen(char *str)
{
	int i = 0;
	while(str[i] != '\0')
		i++;
	return i;
}

void exit_fatal(void)
{
	write(2, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
}

void error_execve_file(char *str)
{
	write(2, "error: cannot execute executable_that_failed ", ft_strlen("error: cannot execute executable_that_failed "));
	write(2, str, ft_strlen(str));
	write(2, "\n", 1);
}

void error_cd_args(void)
{
	write(2, "error: cd: bad arguments\n", ft_strlen("error: cd: bad arguments\n"));
}

void error_cd_directory(char *str)
{
	write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
	write(2, str, ft_strlen(str));
	write(2, "/n", 1);
}

void ft_cd(char **str)
{
	int i = 0;
	while(str[i])
		i++;
	if(i != 2)
	{
		error_cd_args();
		return ;
	}
	if(chdir(str[1]))
		error_cd_directory(str[1]);
}

int main(int args, char **argv, char** env)
{
	int i = 1;
	int len;
	int m;
	char **line = NULL;
	int is_pipe = 0;
	int save1 = 0;
	int save0 = 0;
	int openned = 0;
	int fd[2];
	pid_t pid;
	int status;

	if(args > 1)
	{
		while(i < args)
		{
			if((strcmp(argv[i], ";") == 0) || (strcmp(argv[i], "|") == 0))
			{
				i++;
				continue;
			}
			len = 0;
			while((argv[i + len]) && (strcmp(argv[i + len], ";") != 0) && (strcmp(argv[i + len], "|") != 0))
				len++;
			if(!(line = (char **) malloc(sizeof(char *) * (len + 1))))
				exit_fatal();
			line[len] = NULL;
			m = 0;
			while(m < len)
			{
				line[m] = argv[i];
				m++;
				i++;
			}
			if(argv[i] && strcmp(argv[i], "|") == 0)
				is_pipe = 1;
			else
				is_pipe = 0;
			if(argv[i] && strcmp(argv[i], "cd") == 0)
				ft_cd(line);
			if(openned)
			{
				save0 = dup(0);
				dup2(fd[0], 0);
				close(fd[0]);
			}
			if(is_pipe)
			{
				if(pipe(fd))
					exit_fatal();
				save1 = dup(1);
				dup2(fd[1], 1);
				close(fd[1]);
			}
			pid = fork();
			if(pid < 0)
				exit_fatal();
			else if(pid == 0)
			{
				if(execve(line[0], line, env))
					error_execve_file(line[0]);
				exit(0);
			}
			else
				waitpid(pid, &status, 0);
			if(openned)
			{
				dup2(save0, 0);
				close(save0);
			}
			if(is_pipe)
			{
				dup2(save1, 1);
				close(save1);
				openned = 1;
			}
			free(line);
		}
	}
	return(0);
}
