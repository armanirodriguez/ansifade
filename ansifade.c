/**
 * ANSIFADE - Create beautiful fade gradiants using ANSI color codes!
 * @author Armani Rodriguez
 * GitHub: https://github.com/armanirodriguez
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <argp.h>

/* Argp documentation */
const char *argp_program_bug_address =
	"rodrig52@students.rowan.edu";
static char doc[] =
	"ANSIFade -- Create color gradiants using ANSI color codes\n"
	"You may enter color sequences as a string or as a RGB sequence\n\n"
	"Example: ./ansifade red blue OR ./ansifade red 0,0,255\n\n"
	"Preset colors: red, blue, green, yellow, lime, cyan, pink, purple, white, grey";
static char args_doc[]= 
	"Color1 Color2";

/* Preset color RGB values in hex */
#define COLOR_PINK 		0xff00e6
#define COLOR_PURPLE 		0xa200ff
#define COLOR_BLUE 		0x0000ff
#define COLOR_CYAN 		0x00ffff
#define COLOR_LIME		0x00ff00
#define COLOR_GREEN		0x25c468
#define COLOR_WHITE		0xffffff
#define COLOR_GREY		0xbabfbc
#define COLOR_RED		0xff0000
#define COLOR_YELLOW		0xffff00

#define R(COLOR) ((COLOR & 0xff0000) >> 16)
#define G(COLOR) ((COLOR & 0xff00) >> 8)
#define B(COLOR) (COLOR & 0xff)

#define SEQLEN		24   /* Length of ANSI color sequence */
#define NUMCOLORS 	9    /* Number of preset colors */

#define TERM_RESET "\x1b[0m" /* ANSI Sequence to reset terminal */

void color_putc(char c, int r, int g, int b, FILE* stream);
int strtocol(char *str);
void fade_line(char *line, ssize_t len, int startcolor, int endcolor, FILE* stream);
static error_t parse_opt (int k, char *arg, struct argp_state *state);

/* Argp options */
static struct argp_option opts[] = {
  {"whitelist",  'w',"CHARS",   0,  "Only color specified characters" },
  {"blacklist",  'b',"CHARS",   0,  "Don't color specified characters"},
  {"output",     'o',"OUTFILE", 0,  "Save output to a file" },
  { 0 }
};

static struct argp argp = { 
	opts, 
	parse_opt, 
	args_doc, 
	doc 
};

struct arguments {
	char	*args[2];
	char	*whitelist;
	char	*blacklist;
	char	*outfile;
} args;

/* Color Map */
struct color {
	char* 	name;
	int 	rgb;
} colors[] = {
	"red", COLOR_RED,
	"yellow", COLOR_YELLOW,
	"green", COLOR_GREEN,
	"lime", COLOR_LIME,
	"blue", COLOR_BLUE,
	"cyan", COLOR_CYAN,
	"grey", COLOR_GREY,
	"gray", COLOR_GREY,
	"white", COLOR_WHITE
};


int main(int argc, char* argv[]){
	
	/* Default options values */
	args.blacklist = NULL;
	args.whitelist = NULL;
	args.outfile = NULL;
	/* Parse arguments */
	(void)argp_parse(&argp, argc, argv, 0, 0, &args);

	int color1, color2;
	FILE* of = stdout;
	char *line = NULL;
	ssize_t len;
	ssize_t bufsize = 0;
	color1 = strtocol(args.args[0]);
	color2 = strtocol(args.args[1]);
	if (color1 == INT_MIN){
		fprintf(stderr, "%s: Invalid color: %s\n", argv[0], args.args[0]);
		exit(EXIT_FAILURE);
	} else if (color2 == INT_MIN){
		fprintf(stderr, "%s: Invalid color: %s\n", argv[0], args.args[1]);
		exit(EXIT_FAILURE);
	}
	if (args.outfile != NULL){
		if((of = fopen(args.outfile,"w")) == NULL){
			fprintf(stderr,"Error opening file %s: %s\n",args.outfile,strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	while((len = getline(&line,&bufsize,stdin))>0){
		fade_line(line,len, color1, color2, of);
	}
	free(line);
	if(len == -1 && !feof(stdin))
		/* Getline failed for reason other than EOF */
		fprintf(stderr,"Error on getline: %s\n",strerror(errno));
	if (of != stdout)
		(void)fclose(of);
}
/**
 * converts string to integer representation of the color
 * @return integer representation of the color
 */
int strtocol(char *str){
	for (int i=0; str[i]; str[i]= tolower(str[i]), i++)
		;
	/* Check if str is in color map */
	for(int i=0;i<NUMCOLORS;i++){
		if(!strcmp(str,colors[i].name))
			return colors[i].rgb;
	}
	/* Otherwise parse RBG values */
	unsigned int r, g, b;
	r = g = b = INT_MIN;
	sscanf(str,"%d,%d,%d",&r,&g,&b);
	if (r == INT_MIN || g == INT_MIN || b == INT_MIN ||
		r > 255 || g > 255 || b > 255)
		return INT_MIN;
	return (r << 16) | (g << 8) | b;
}

/**
 * prints a single character to stream colored using 
 * ANSI rgb color code
 * @param c - the character
 * @param r - the red value
 * @param g - the green value
 * @param b - the blue value
 * @param stream - stream to print to
 */
void color_putc(char c, int r, int g, int b, FILE* stream){
	fprintf(stream,"\x1b[38;2;%u;%u;%um%c",r,g,b,c);
}

/**
 * Fades entire line and prints to output stream
 * @param line line to fade
 * @param len length of the line
 * @param startcolor the left color in the gradiant
 * @param endcolor the right color in the gradiant
 * @param stream the stream to output to
 */
void fade_line(char *line, ssize_t len, int startcolor, int endcolor, FILE* stream){
	int r, g, b, change_r,change_g, change_b;
	char c;
	r = R(startcolor);
	g = G(startcolor);
	b = B(startcolor);
	change_r = (R(endcolor) - r) / len;
	change_g = (G(endcolor) - g) / len;
	change_b = (B(endcolor) - b) / len;
	while(--len){
		c = *line++;
		if (args.whitelist == NULL & args.blacklist == NULL)
			/* No whitelist or blacklist */
			color_putc(c,r,g,b,stream);
		else if (args.whitelist != NULL && strchr(args.whitelist, c) != NULL)
			/* Character is whitelisted */
			color_putc(c,r,g,b,stream);
		else if (args.blacklist != NULL && strchr(args.blacklist, c) == NULL)
			/* Character is not blacklisted */
			color_putc(c,r,g,b,stream);
		else
			/* Reset terminal color */
			printf(TERM_RESET"%c",c);
		r += change_r;
		g += change_g;
		b += change_b;
	}
	puts(TERM_RESET);
}

/* Option parser */
static error_t parse_opt (int k, char *arg, struct argp_state *state){
	struct arguments *args = state->input;
	switch (k){
		case 'w':
			args->whitelist = arg;
			break;
		case 'b':
			args->blacklist = arg;
			break;
		case 'o':
			args->outfile = arg;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num >= 2)
				argp_usage(state);
			args->args[state->arg_num] = arg;
			break;
		case ARGP_KEY_END:
			if (state->arg_num < 2)
				argp_usage(state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
