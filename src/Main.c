#include "Main.h"

int 
main(int argc, char **argv)
{
	static HttpseRulesetOptions roptions = kHttpseRulesetOptions;
	if(0 == httpse_getopt_long(argc, argv, &roptions))
	{
		return EXIT_FAILURE;
	}

	HttpseRuleset *ru = HttpseRuleset_init(&roptions);
	if(!ru)
	{
		return EXIT_FAILURE;
	}


	/* Remark: Write the ruleset to its destination */
	if(roptions.output)
	{
		FILE *outfile = fopen(roptions.output, "w+b");
		if(outfile)
		{
			fwrite(ru->ctx->data, 1, ru->ctx->size, outfile);
			fclose(outfile);
		}
	}
	else
	{
		fwrite(ru->ctx->data, 1, ru->ctx->size, stdout);
	}

	ru = HttpseRuleset_cleanup(ru);
	return 0;
}

void
print_usage(const char *me)
{
	fprintf(stdout,
	"Usage: %s [OPTION] ... [FILE]\n"
	"Automatically generate HTTPS Everywhere ruleset.\n"
	"\n"
	"Options\n"
	"     --name NAME                 Set NAME the output ruleset name\n"
	" -o, --output FILE               Write output to FILE instead of stdout\n"
	"     --platform PLATFORM         Set PLATFORM the output ruleset platform\n"
	"     --securecookie              Enable securecookie flag in the output\n"
	"     --skip-hsts-check           Whether to skip HSTS check"
	"\n"
	"Options (libcurl)\n"
	"     --capath DIR                CA directory to verify peer against (SSL)\n"
	"     --compressed                Request compressed response\n"
	"     --connect-timeout SECONDS   Maximum time allowed for connection\n"
	" -m, --max-time SECONDS          Maximum time allowed for the transfer\n"
	"     --tr-encoding               Request compressed transfer encoding\n"
	" -A, --useragent STRING          Send User-Agent STRING to server\n"
	" -v, --verbose                   Enable libcurl verbose mode\n"
	"\n"
	"Misc.\n"
	"     --num-threads NUM           Use NUM threads whenever possible\n"
	" -h, --help                      Display this help and exit\n",
	me);
}

int 
httpse_getopt_long(int argc, char **argv, HttpseRulesetOptions *roptions)
{
	struct option coptions[] = {
		/* options */
		{"name",            required_argument, NULL, 257},
		{"ouput",           required_argument, NULL, 'o'},
		{"platform",        required_argument, NULL, 258},
		{"securecookie",    no_argument,       NULL, 259},

		/* libcurl options */
		{"capath",          required_argument, NULL, 260},
		{"compressed",       no_argument,      NULL, 261},
		{"connect-timeout", required_argument, NULL, 262},
		{"max-time",        required_argument, NULL, 'm'},
		{"tr-encoding",     no_argument,       NULL, 263},
		{"useragent",       required_argument, NULL, 'A'},
		{"verbose",         no_argument,       NULL, 'v'},

		/* misc */
		{"num-threads",     required_argument, NULL, 264},
		{"help",            no_argument,       NULL, 'h'},
		{"skip-hsts-check", no_argument,       NULL, 265},
		{0, 0, 0, 0}
	};

	int index = -1;
	while(1)
	{
		int retval = getopt_long(argc, argv, "o:m:A:vh", coptions, &index);

		if(-1 == retval)
		{
			break;
		}

		switch(retval)
		{
		case 'o':
			roptions->output = optarg;
			break;
		
		case 'm':
			roptions->options.timeout = atol(optarg);
			break;
		
		case 'A':
			roptions->options.useragent = optarg;
			break;

		case 'v':
			roptions->options.verbose = 1L;
			break;
		
		case 'h':
			print_usage(*argv);
			exit(0);

		case 257:
			roptions->name = optarg;
			break;
		
		case 258:
			roptions->platform = optarg;
			break;

		case 259:
			roptions->securecookie = 1L;
			break;
		
		case 260:
			roptions->options.capath = optarg;
			break;

		case 261:
			roptions->options.ac_encoding = 1L;
			break;

		case 262:
			roptions->options.connect_timeout = atol(optarg);
			break;

		case 263:
			roptions->options.tr_encoding = 1L;
			break;

		case 264:
			roptions->num_threads = atol(optarg);
			break;
		
		case 265:
			roptions->options.skip_hsts_check = 1L;
			break;

		default:
			break;
		}
	}

	if(optind < argc)
	{
		roptions->path = argv[optind];
	}
	else
	{
		fprintf(stderr,
			"%s: missing file operand\n"
			"%s: try 'xhttpse --help' for more information\n,",
			argv[0], argv[0]);
		return 0;
	}
	return 1;
}
