#ifndef __COLOR_H_
#define __COLOR_H_

#define RED(STR)	"\033[31;1m"STR"\033[0m"
#define GREEN(STR)	"\033[32;1m"STR"\033[0m"
#define YELLOW(STR)	"\033[33;1m"STR"\033[0m"
#define BLUE(STR)	"\033[34;1m"STR"\033[0m"
#define PINK(STR)	"\033[35;1m"STR"\033[0m"
#define CYAN(STR)	"\033[36;1m"STR"\033[0m"

#define REDSET		"\x1b[31m"
#define	GREENSET	"\x1b[32m"
#define	YELLOWSET	"\x1b[33m"
#define	BLUESET		"\x1b[34m"
#define	PINKSET		"\x1b[35m"
#define	CYANSET		"\x1b[36m"
#define RESET		"\x1b[0m"		

#endif /* __COLOR_H__ */
